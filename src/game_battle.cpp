#include "game_battle.h"
#include "input.h"
#include "renderer.h"
#include "rpg_generation.h"

namespace
{
    const int BATTLE_TILE = 96;
    const int PADDING = 5;
    const int ANIM_DELAY = 100;
}

ModeBattle::ModeBattle( GameState & state )
    : _title( { 0, 0, RenderEngine::GetScreenSize().x, 50 }, "Battle" )
    , _bExit( RenderEngine::GetAnchorRect( AnchorPoint::BOTTOM_RIGHT, 270, 80 ), "Return", {} )
    , _arena( state.battle.playerForce, state.battle.otherForce )
{
    name = GameModeName::BATTLE;
}

GameModeName ModeBattle::handleEvents()
{
    InputHandler & input = InputHandler::Get();

    if ( input.handleEvent() ) {
        if ( input.isSet( InputHandler::MOUSE_CLICKED ) ) {
            const Point & mouseClick = input.getClickPosition();
            if ( _bExit.getRect().contains( mouseClick ) ) {
                return GameModeName::CANCEL;
            }
            else {
                _arena.executeTurn();
            }
        }
        else if ( input.consume( InputHandler::KEY_PRESSED ) ) {
            const char key = input.consumeKey( true );
            if ( key == '1' ) {

            }
        }
        return name;
    }
    return GameModeName::QUIT_GAME;
}

void ModeBattle::update( float deltaTime )
{
    _animTimer += deltaTime;

    for ( auto & unit : _arena.getAttackers().modifyCharacters( RPG::Force::ALL ) ) {
        unit.get().update( deltaTime );
    }
    for ( auto & unit : _arena.getDefenders().modifyCharacters( RPG::Force::ALL ) ) {
        unit.get().update( deltaTime );
    }
}

void ModeBattle::render()
{
    const RPG::Force & left = _arena.getAttackers();
    const RPG::Force & right = _arena.getDefenders();
    const Point & screenSize = RenderEngine::GetScreenSize();

    const int tileSize = BATTLE_TILE + PADDING;

    Rect target = { ( screenSize.x - BATTLE_TILE ) / 2, ( screenSize.y - BATTLE_TILE ) / 2, tileSize, tileSize };
    target.pos.x -= tileSize * 6;
    target.pos.y -= tileSize * 3;
    for ( int i = 0; i < 6; i++ ) {
        for ( int j = 0; j < 11; j++ ) {
            target.pos.x += tileSize;
            RenderEngine::Draw( j % 2 ? "assets/plains.png" : "assets/forest.png", target );
        }
        target.pos.x -= tileSize * 11;
        target.pos.y += tileSize;
    }

    renderForce( left, false );
    renderForce( right, true );

    _title.render();
    _bExit.render();
}

void ModeBattle::renderForce( const RPG::Force & target, bool mirror )
{
    const Point & screenSize = RenderEngine::GetScreenSize();
    const std::vector<RPG::Force::Position> positions = { RPG::Force::FRONT, RPG::Force::SIDE, RPG::Force::CENTER, RPG::Force::BACK };

    Rect drawArea = { { ( screenSize.x - BATTLE_TILE ) / 2, ( screenSize.y - BATTLE_TILE ) / 2 }, { BATTLE_TILE, BATTLE_TILE } };

    for ( auto & position : positions ) {
        const int offset = ( mirror ) ? BATTLE_TILE + PADDING : -BATTLE_TILE - PADDING;
        drawArea.pos.x += offset;

        auto units = target.getCharacters( position );
        if ( !units.empty() ) {
            const RPG::BattleUnit & unit = units.front().get();
            RenderEngine::Draw( unit.getSprite(), drawArea, mirror );
            if ( _arena.getCurrentUnit() && _arena.getCurrentUnit()->getId() == unit.getId() ) {
                RenderEngine::DrawPieSlice( { drawArea.pos.add( BATTLE_TILE / 2, 0 ), { BATTLE_TILE, BATTLE_TILE } }, 255, 285,
                                            StandardColor::REALM_PRECISION );
            }

            Point textPosition = drawArea.pos;
            if ( mirror ) {
                textPosition.x += BATTLE_TILE - 22;
            }
            RenderEngine::DrawText( std::to_string( unit.getCurrentHealth() ), textPosition );
        }
    }
}
