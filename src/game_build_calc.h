#pragma once

#include "game_state.h"
#include "gamemode.h"
#include "ui_components.h"

class Character;

class ModeBuildCalculator : public GameMode
{
    GameState & _state;
    RPG::Character _character;

    CenteringLabel _title;
    Button _bExit;

    Label _charName;
    Label _levelClass;
    Label _statPoints;
    Label _skillPoints;
    ProgressBar _health;
    Button _bGenerateName;
    Button _bNext;
    Button _bPrevious;

    UIContainer _attributes;
    UIContainer _physicalSkills;
    UIContainer _magicalSkills;

    void changeCharacter( RPG::Character other );
    void modifyEvent();
    void saveCharacter();

public:
    ModeBuildCalculator( GameState & state );
    virtual GameModeName handleEvents() override;
    virtual void update( float deltaTime ) override;
    virtual void render() override;
};
