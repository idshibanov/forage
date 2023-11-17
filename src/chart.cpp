#include "chart.h"
#include "renderer.h"

#include "SDL2_gfx.h"

Chart::Pie::Pie( const Point & center, const Point & size )
    : UIComponent( { center, size } )
{}

Chart::Pie::Pie( const Point & center, const Point & size, const std::vector<DataPoint> & data )
    : UIComponent( { center, size } )
    , _data(data)
{}

bool Chart::Pie::handleClick( const Point & target )
{
    const double radians = atan2( ( _rect._pos._y + _rect._size._y / 2 ) - target._y, ( _rect._pos._x + _rect._size._x / 2 ) - target._x );
    double clickAngle = radians * 180 / M_PI - 180;
    double startAngle = ( 180.0 / _data.size() + 90 ) * -1;
    if ( clickAngle < startAngle )
        clickAngle += 360;

    for ( DataPoint & dp : _data ) {
        const double endAngle = startAngle + dp.value * 360;
        if ( clickAngle >= startAngle && clickAngle <= endAngle ) {
            dp.active = !dp.active;
            return true;
        }
        startAngle = endAngle;
    }
    return false;
}

void Chart::Pie::setData( const std::vector<DataPoint> & data )
{
    _data = data;
}

void Chart::Pie::update( float deltaTime ) {
    for ( DataPoint & dp : _data ) {
        if (dp.active) {
            if ( dp.offset < 20 ) {
                dp.offset += deltaTime * 30;
            }
            else {
                dp.offset = 20;
            }
        }
        else {
            if ( dp.offset > 0 ) {
                dp.offset -= deltaTime * 30;
            }
            else {
                dp.offset = 0;
            }
        }
    }
}

void Chart::Pie::render()
{
    if ( _hidden || _data.empty() ) {
        return;
    }

    const double angleOffset = 180.0 / _data.size();
    double startAngle = ( angleOffset + 90 ) * -1;

    for ( size_t id = 0; id < _data.size(); id++ ) {
        DataPoint & dp = _data[id];
        const double endAngle = startAngle + dp.value * 360;
        Rect target;
        target._pos._x = _rect._pos._x + _rect._size._x / 2;
        target._pos._y = _rect._pos._y + _rect._size._y / 2;
        target._size._x = _rect._size._x / 2 + dp.offset;
        target._size._y = _rect._size._y / 2 + dp.offset;
        RenderEngine::DrawPieSlice( target, startAngle, endAngle, dp.color );
        startAngle = endAngle;
    }
}
