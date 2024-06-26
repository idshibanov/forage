#include "renderer.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

#include "SDL2_gfx.h"
#include "ui_styling.h"

namespace
{
    RenderEngine engine;
    SDL_Color tintValue = { 255, 255, 255, 255 };
}

SDL_Rect convertRect( const Rect & rect )
{
    return { rect.pos.x, rect.pos.y, rect.size.x, rect.size.y };
}

bool RenderEngine::Initialize( Point logicalSize, double scaling )
{
    _logicalSize = logicalSize;

    // Initialize SDL2 and create window/renderer
    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return false;
    }

    _window = SDL_CreateWindow( "BDG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _logicalSize.x * scaling, _logicalSize.y * scaling, SDL_WINDOW_SHOWN );
    if ( !_window ) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    _renderer = SDL_CreateRenderer( _window, -1, SDL_RENDERER_ACCELERATED );
    if ( !_renderer ) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow( _window );
        SDL_Quit();
        return false;
    }

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" );
    SDL_RenderSetScale( _renderer, scaling, scaling );

    if ( !StandardStyles::loadAssets() ) {
        std::cout << "Error initializing standard fonts and colors" << std::endl;
        return false;
    }

    // Initialize any additional game state variables or components here
    _assets.initRenderer( _renderer );

    return false;
}

SDL_Renderer * RenderEngine::GetRenderer()
{
    return _renderer;
}

void RenderEngine::Present()
{
    SDL_RenderPresent( _renderer );
}

void RenderEngine::applyTint( StandardColor tint, float value )
{
    tintValue = *static_cast<SDL_Color *>( StandardStyles::getColor( tint ) );
    if ( value < 1.0f ) {
        tintValue.r = 255 - ( 255 - tintValue.r ) * value;
        tintValue.g = 255 - ( 255 - tintValue.g ) * value;
        tintValue.b = 255 - ( 255 - tintValue.b ) * value;
    }
}

Point RenderEngine::GetScreenSize()
{
    return engine._logicalSize;
}

Point RenderEngine::GetAnchorPoint( AnchorPoint anchor )
{
    switch ( anchor ) {
    case AnchorPoint::TOP_LEFT:
        return {};
    case AnchorPoint::TOP_CENTER:
        return { engine._logicalSize.x / 2, 0 };
    case AnchorPoint::TOP_RIGHT:
        return { engine._logicalSize.x, 0 };
    case AnchorPoint::CENTER_LEFT:
        return { 0, engine._logicalSize.y / 2 };
    case AnchorPoint::CENTER:
        return engine._logicalSize.div( 2 );
    case AnchorPoint::CENTER_RIGHT:
        return { engine._logicalSize.x, engine._logicalSize.y / 2 };
    case AnchorPoint::BOTTOM_LEFT:
        return { 0, engine._logicalSize.y };
    case AnchorPoint::BOTTOM_CENTER:
        return { engine._logicalSize.x / 2, engine._logicalSize.y };
    }
    return engine._logicalSize;
}

Rect RenderEngine::GetAnchorRect( AnchorPoint anchor, int width, int height )
{
    switch ( anchor ) {
    case AnchorPoint::TOP_CENTER:
        return { ( engine._logicalSize.x - width ) / 2, 0, width, height };
    case AnchorPoint::TOP_RIGHT:
        return { ( engine._logicalSize.x - width ), 0, width, height };
    case AnchorPoint::CENTER_LEFT:
        return { 0, ( engine._logicalSize.y - height ) / 2, width, height };
    case AnchorPoint::CENTER:
        return { ( engine._logicalSize.x - width ) / 2, ( engine._logicalSize.y - height ) / 2, width, height };
    case AnchorPoint::CENTER_RIGHT:
        return { ( engine._logicalSize.x - width ), ( engine._logicalSize.y - height ) / 2, width, height };
    case AnchorPoint::BOTTOM_LEFT:
        return { 0, engine._logicalSize.y - height, width, height };
    case AnchorPoint::BOTTOM_CENTER:
        return { ( engine._logicalSize.x - width ) / 2, engine._logicalSize.y - height, width, height };
    case AnchorPoint::BOTTOM_RIGHT:
        return { engine._logicalSize.x - width, engine._logicalSize.y - height, width, height };
    }
    return { 0, 0, width, height };
}

RenderEngine & RenderEngine::Get()
{
    return engine;
}

bool RenderEngine::Draw( const std::string & image, const Rect & target, bool flipped )
{
    SDL_Texture * texture = engine._assets.loadTexture( image );
    if ( !texture ) {
        return false;
    }

    SDL_SetTextureColorMod( texture, tintValue.r, tintValue.g, tintValue.b );

    SDL_Rect rect = convertRect( target );

    if ( !flipped ) {
        return SDL_RenderCopy( engine._renderer, texture, NULL, &rect ) == 0;
    }

    return SDL_RenderCopyEx( engine._renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_HORIZONTAL ) == 0;
}

bool RenderEngine::DrawTinted( const std::string & image, const Rect & target, StandardColor mask, bool flipped )
{
    SDL_Texture * texture = engine._assets.loadTexture( image );
    if ( !texture ) {
        return false;
    }

    SDL_Color * sdlColor = static_cast<SDL_Color *>( StandardStyles::getColor( mask ) );
    SDL_SetTextureColorMod( texture, sdlColor->r, sdlColor->g, sdlColor->b );

    SDL_Rect rect = convertRect( target );

    if ( !flipped ) {
        return SDL_RenderCopy( engine._renderer, texture, NULL, &rect ) == 0;
    }

    return SDL_RenderCopyEx( engine._renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_HORIZONTAL ) == 0;
}

bool RenderEngine::DrawRect( const Rect & target, StandardColor color )
{
    SDL_Color * sdlColor = static_cast<SDL_Color *>( StandardStyles::getColor( color ) );
    if ( !sdlColor ) {
        return false;
    }

    if ( SDL_SetRenderDrawColor( engine._renderer, sdlColor->r, sdlColor->g, sdlColor->b, sdlColor->a ) ) {
        return false;
    }

    SDL_Rect rect = convertRect( target );

    return SDL_RenderFillRect( engine._renderer, &rect ) == 0;
}

bool RenderEngine::DrawStyledRect( const Rect & target, const Style & style )
{
    SDL_Color * sdlColor = static_cast<SDL_Color *>( StandardStyles::getColor( style.backgroundColor ) );
    if ( !sdlColor ) {
        return false;
    }

    if ( SDL_SetRenderDrawColor( engine._renderer, sdlColor->r, sdlColor->g, sdlColor->b, sdlColor->a ) ) {
        return false;
    }

    SDL_Rect rect = convertRect( target );

    return SDL_RenderFillRect( engine._renderer, &rect ) == 0;
}

int RenderEngine::DrawText( const std::string & text, const Point & target )
{
    return DrawText( text, target, StandardFont::REGULAR, StandardColor::WHITE );
}

int RenderEngine::DrawText( const std::string & text, const Point & target, StandardFont font )
{
    return DrawText( text, target, font, StandardColor::WHITE );
}

int RenderEngine::DrawText( const std::string & text, const Point & target, StandardFont font, StandardColor color )
{
    SDL_Surface * textSurface = GetTextSurface( text, font, color );
    if ( !textSurface ) {
        return false;
    }

    SDL_Texture * textTexture = SDL_CreateTextureFromSurface( engine._renderer, textSurface );

    SDL_Rect textRect = { target.x, target.y, textSurface->w, textSurface->h };
    const bool success = SDL_RenderCopy( engine._renderer, textTexture, NULL, &textRect ) == 0;
    SDL_FreeSurface( textSurface );
    SDL_DestroyTexture( textTexture );

    return success ? textRect.w : 0;
}

SDL_Surface * RenderEngine::GetTextSurface( const std::string & text, StandardFont font, StandardColor color )
{
    SDL_Color * textColor = static_cast<SDL_Color *>( StandardStyles::getColor( color ) );
    if ( !textColor ) {
        return nullptr;
    }

    TTF_Font * fontPtr = static_cast<TTF_Font *>( StandardStyles::getFont( font ) );
    if ( !fontPtr ) {
        return nullptr;
    }

    return TTF_RenderText_Blended( fontPtr, text.c_str(), *textColor );
}

bool RenderEngine::DrawDestroySurface( SDL_Surface * surface, const Rect & target )
{
    if ( !surface ) {
        return false;
    }

    SDL_Texture * textTexture = SDL_CreateTextureFromSurface( engine._renderer, surface );

    SDL_Rect textRect = { target.pos.x, target.pos.y, target.size.x, target.size.y };
    const bool success = SDL_RenderCopy( engine._renderer, textTexture, NULL, &textRect ) == 0;
    SDL_FreeSurface( surface );
    SDL_DestroyTexture( textTexture );

    return success;
}

bool RenderEngine::DrawDestroyAlphaSurface( SDL_Surface * surface, const Rect & target, int alpha )
{
    if ( !surface ) {
        return false;
    }

    SDL_Texture * textTexture = SDL_CreateTextureFromSurface( engine._renderer, surface );

    SDL_SetTextureAlphaMod( textTexture, alpha );

    SDL_Rect textRect = { target.pos.x, target.pos.y, target.size.x, target.size.y };
    const bool success = SDL_RenderCopy( engine._renderer, textTexture, NULL, &textRect ) == 0;
    SDL_FreeSurface( surface );
    SDL_DestroyTexture( textTexture );

    return success;
}

bool RenderEngine::DrawPieSlice( const Rect & target, double startAngle, double endAngle, StandardColor color )
{
    SDL_Color * pieColor = static_cast<SDL_Color *>( StandardStyles::getColor( color ) );
    if ( !pieColor ) {
        return false;
    }
    return SDL::aaFilledPieRGBA( engine._renderer, target.pos.x, target.pos.y, target.size.x, target.size.y, startAngle, endAngle, false, pieColor->r, pieColor->g,
                                 pieColor->b, pieColor->a )
           == 0;
}

int RenderEngine::GetTextWidth( const std::string & text, StandardFont font )
{
    SDL_Surface * textSurface = GetTextSurface( text, font, StandardColor::WHITE );
    if ( !textSurface ) {
        return 0;
    }

    const int result = textSurface->w;
    SDL_FreeSurface( textSurface );

    return result;
}
