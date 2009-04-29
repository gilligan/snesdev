#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include "tile.h"

#define SNES_MAP_MODE 0
#define GB_MAP_MODE 1

using namespace std;

class MapAttribute {
   
    public:
        
        MapAttribute( int m=SNES_MAP_MODE, int tNr=0, int pNr=0, int v=0, int h=0, int p=0 )
        {
            tileNr = tNr;
            palNr  = pNr;
            vFlip  = v;
            hFlip  = h;
            prior  = p;
            mode = m;
        }

        int tileNr;
        int palNr;
        int vFlip;
        int hFlip;
        int prior;
        int mode;

        QTextStream & operator<<(  QTextStream & os )

        {
            if (mode==SNES_MAP_MODE){

            int lo = ( tileNr & 0xff );
            int hi = ( (tileNr & 0x300) >> 8 ) | ( palNr << 2 ) | ( prior << 5 ) | ( hFlip << 6 ) | ( vFlip << 7 );

             os << "$" << hex << lo <<", $" << hex << hi;

            }

            if (mode==GB_MAP_MODE){
                os << "$" << (tileNr & 0xff);
            }

           return os;
        }

};

class TileMap {

    public:
        
        TileMap();
        bool addTile( Tile* t, int palNum, bool mapFlag );
        unsigned int getTileCount() { return tileCount; };
        unsigned int getMapSize() { return mapSize; };
        void setTileOffset( int ofs ) { tileOffset = ofs; }
        void setEmptyTile( bool f ) { hasEmptyTile = f; }

        int mode;
        Tile & getTile( unsigned int index ) { return *tiles[index]; };
        MapAttribute  getAttribute( unsigned int index ) { return *map[index]; };

    private:

         int findTile( Tile* t );
        
         std::vector<MapAttribute*> map;
         std::vector<Tile*> tiles;

         int tileCount;
         int tileOffset;
         int mapSize;
         bool hasEmptyTile;

};
#endif // TILEMAP_H
