#pragma once

#include "gamemode.h"
#include "ui.h"

class ModeBattle : public GameMode
{
    Label _title;
    Button _bExit;

public:
    ModeBattle();
    virtual GameModeName handleEvents() override;
    virtual void update( float deltaTime ) override;
    virtual void render() override;
};
