#include "particle.h"

namespace
{
    const int SUBDIVISION = 1000;
}

Particle::Particle( Point pos, Point target, int size, int speed, float lifetime )
    : position( pos.modMul( SUBDIVISION ) )
    , target( target.modMul( SUBDIVISION ) )
    , size( size )
    , speed( speed )
    , lifetime( lifetime )
{}

Point Particle::getDrawPosition() const
{
    return position.div( SUBDIVISION );
}

Rect Particle::getDrawRect() const
{
    return { getDrawPosition(), { size, size } };
}

void Particle::update( float delta )
{
    lifetime -= delta;

    Point diff = target - position;
    int max = std::max( std::abs( diff._x ), std::abs( diff._y ) );
    if ( max != 0 && max > speed ) {
        // normalize
        float vectorX = (float)diff._x / max;
        float vectorY = (float)diff._y / max;
        position._x += vectorX * speed;
        position._y += vectorY * speed;
    }
}