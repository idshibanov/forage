#pragma once

#include "point.h"
#include "ui_styling.h"

#include <memory>
#include <string>
#include <vector>

class UIComponent
{
protected:
    Rect _rect;
    bool _hidden = false;

public:
    enum ClickBaseEvent : int
    {
        NO_EVENT = 0,
        BASIC_EVENT = 1
    };

    UIComponent( const Rect & dimensions );
    virtual ~UIComponent() = default;

    virtual void update( float deltaTime ) = 0;
    virtual void render() = 0;

    virtual int handleEvent( const Point & click, int event )
    {
        return NO_EVENT;
    }

    bool isHidden() const
    {
        return _hidden;
    }

    void setHidden( bool isHidden )
    {
        _hidden = isHidden;
    }

    const Rect & getRect() const
    {
        return _rect;
    }
};

class UIContainer : public UIComponent
{
protected:
    std::vector<std::shared_ptr<UIComponent> > _items;

public:
    UIContainer( const Rect & dimensions );

    virtual void update( float deltaTime );
    virtual void render();
    virtual int handleEvent( const Point & click, int event );

    void addElement( std::shared_ptr<UIComponent> element );
    virtual std::shared_ptr<UIComponent> getElement( const Point & position );
    void updateRect();
};

// A basic text label element
class Label : public UIComponent
{
public:
    Label( const Point & position, const std::string & text );
    Label( const Point & position, const std::string & text, StandardFont font, StandardColor color );

    void setColor( StandardColor color );
    virtual void setText( const std::string & text );

    virtual void update( float deltaTime ) override {}
    virtual void render() override;

protected:
    std::string _text;
    StandardFont _font = StandardFont::REGULAR;
    StandardColor _color = StandardColor::WHITE;
};

class CenteringLabel : public Label
{
    Point _expectedSize;

public:
    CenteringLabel( const Point & position, const std::string & text, int width );
    CenteringLabel( const Rect & dimensions, const std::string & text );
    CenteringLabel( const Rect & dimensions, const std::string & text, StandardFont font, StandardColor color );
    virtual void render() override;
};

class Button : public UIComponent
{
    std::string _label;
    Style _style;
    bool _isHovered = false;
    bool _isPressed = false;

public:
    Button( int x, int y, int width, int height, const std::string & label );
    Button( const Rect & dimensions, const std::string & label, const Style & style );
    Button( const Point & position, int width, int height, const std::string & label, const Style & style );
    virtual ~Button() = default;

    virtual int handleEvent( const Point & click, int event ) override;
    virtual void update( float deltaTime ) override;
    virtual void render() override;

    void setPressed( bool value );
    void setHovered( bool value );
    void setStyle( const Style & style );
    void setLabel( const std::string & label );
};

class Window : public UIComponent
{
public:
    Window( Rect rect, const std::string & title );
    ~Window();

    void update( float deltaTime );
    void render();

    void setStyle( const Style & style );
    void addComponent( std::shared_ptr<UIComponent> component );

    std::shared_ptr<UIComponent> getElement( const Point & click );

private:
    Style _style;
    std::string _title;
    std::vector<std::shared_ptr<UIComponent> > _components;
};
