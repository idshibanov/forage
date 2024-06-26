#pragma once
#include <vector>

struct MapTile
{
    int x = 0;
    int y = 0;

    virtual ~MapTile() {}
};

class MapBase
{
protected:
    size_t _width = 0;
    size_t _height = 0;

    MapBase( size_t w, size_t h )
        : _width( w )
        , _height( h )
    {}

public:
    size_t getWidth() const;
    size_t getHeight() const;
    bool isValid( size_t index ) const;
    virtual const MapTile & getTile( size_t index ) const = 0;
};

class HexMapManager : public MapBase
{
public:
    HexMapManager( int size );
    ~HexMapManager();

    void generateMap();
    bool isOnMap( int x, int y );
    bool isOccupied( int x, int y );
    void setOccupied( int x, int y, bool occupied );
    MapTile * getTileAt( int x, int y );
    std::vector<MapTile *> findPath( MapTile * startTile, MapTile * endTile );
    std::vector<MapTile *> getNeighbors( MapTile * tile );
    int getDistance( int index1, int index2 );
    int getDistance( MapTile * left, MapTile * right );

private:
    std::vector<std::vector<MapTile *> > _tiles;
};
