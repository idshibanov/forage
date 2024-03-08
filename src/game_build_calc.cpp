#include "game_build_calc.h"
#include "binding.h"
#include "input.h"
#include "renderer.h"
#include "rpg_generation.h"

namespace
{
    class SkillCounter : public UIContainer
    {
        ValueBinding _binding;

    public:
        SkillCounter( Point position, int width, RPG::Character::Skills skill, ValueBinding binding )
            : UIContainer( { position._x, position._y, 0, 0 } )
            , _binding( binding )
        {
            addElement( std::make_shared<Label>( Label( position, RPG::Character::GetSkillName( skill ) ) ) );
            position.modAdd( 150, 0 );

            Style skillBarStyle;
            skillBarStyle.font = StandardFont::SMALL;
            skillBarStyle.textColor = StandardColor::WHITE;
            skillBarStyle.backgroundColor = StandardColor::DARK_GREY;
            skillBarStyle.borderColor = StandardColor::REALM_PRECISION;
            skillBarStyle.borderWidth = 2;
            skillBarStyle.borderRadius = 5;

            addElement( std::make_shared<ProgressBar>( ProgressBar( { position._x, position._y, width, 31 }, binding, skillBarStyle ) ) );

            Style buttonStyle;
            buttonStyle.font = StandardFont::REGULAR;
            buttonStyle.textColor = StandardColor::WHITE;
            buttonStyle.backgroundColor = StandardColor::DARK_GREY;
            buttonStyle.borderColor = StandardColor::REALM_PRECISION;
            buttonStyle.borderWidth = 2;

            addElement( std::make_shared<Button>( Button( { position._x + width + 5, position._y, 31, 31 }, "+", buttonStyle ) ) );
            addElement( std::make_shared<Button>( Button( { position._x - 36, position._y, 31, 31 }, "-", buttonStyle ) ) );

            updateRect();
        }

        void handleClickEvent( const Point & click ) override
        {
            if ( _items[2]->getRect().contains( click ) ) {
                if ( ProgressBar * bar = dynamic_cast<ProgressBar *>( _items[1].get() ); bar != nullptr ) {
                    bar->setValue( bar->getValue() + 1 );
                }
            }
            else if ( _items[3]->getRect().contains( click ) ) {
                if ( ProgressBar * bar = dynamic_cast<ProgressBar *>( _items[1].get() ); bar != nullptr ) {
                    bar->setValue( bar->getValue() - 1 );
                }
            }
        }
    };
}

ModeBuildCalculator::ModeBuildCalculator( GameState & state )
    : _state( state )
    , _title( { 50, 10 }, "Character Builer" )
    , _bExit( RenderEngine::GetScreenSize()._x / 2 + 100, RenderEngine::GetScreenSize()._y - 80, 270, 60, "Return" )
    , _charName( { RenderEngine::GetScreenSize()._x / 2, 80 }, "Unknown" )
    , _bGenerateName( RenderEngine::GetScreenSize()._x / 2 - 100, RenderEngine::GetScreenSize()._y - 80, 100, 60, "Generate" )
    , skills( { 300, 500, 0, 0 } )
{
    name = GameModeName::BUILD_CALCULATOR;

    _character = _state.units.front();

    Point p = skills.getRect()._pos;
    skills.addElement( std::make_shared<SkillCounter>( p, 200, RPG::Character::CLOSE_COMBAT, _character.getSkillBinding( RPG::Character::CLOSE_COMBAT ) ) );
    skills.addElement(
        std::make_shared<SkillCounter>( p.modAdd( 0, 40 ), 200, RPG::Character::RANGED_COMBAT, _character.getSkillBinding( RPG::Character::RANGED_COMBAT ) ) );
    skills.addElement( std::make_shared<SkillCounter>( p.modAdd( 0, 40 ), 200, RPG::Character::DODGE, _character.getSkillBinding( RPG::Character::DODGE ) ) );
    skills.addElement( std::make_shared<SkillCounter>( p.modAdd( 0, 40 ), 200, RPG::Character::BLOCK, _character.getSkillBinding( RPG::Character::BLOCK ) ) );
    skills.addElement( std::make_shared<SkillCounter>( p.modAdd( 0, 40 ), 200, RPG::Character::STEALTH, _character.getSkillBinding( RPG::Character::STEALTH ) ) );
}

GameModeName ModeBuildCalculator::handleEvents()
{
    InputHandler & input = InputHandler::Get();

    if ( input.handleEvent() ) {
        if ( input.isSet( InputHandler::MOUSE_CLICKED ) ) {
            const Point & mouseClick = input.getClickPosition();
            if ( _bExit.getRect().contains( mouseClick ) ) {
                return GameModeName::CANCEL;
            }
            else if ( _bGenerateName.getRect().contains( mouseClick ) ) {
                _charName.setText( RPG::Generator::GetCharacterName() );
            }
            else if ( skills.getRect().contains( mouseClick ) ) {
                skills.handleClickEvent( mouseClick );
            }
        }
        return name;
    }
    return GameModeName::QUIT_GAME;
}

void ModeBuildCalculator::update( float deltaTime ) {}

void ModeBuildCalculator::render()
{
    _title.render();
    _bExit.render();

    RenderEngine::Draw( "assets/portaits/03087.png", { 10, 10, 256, 384 } );

    _charName.render();
    _bGenerateName.render();

    skills.render();
}
