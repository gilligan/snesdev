#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include "tile.h"

using namespace std;

class MapAttribute {
   
    public:
        
        MapAttribute( int tNr, int pNr=0, int v=0, int h=0, int p=0 )
        {
            tileNr = tNr;
            palNr  = pNr;
            vFlip  = v;
            hFlip  = h;
            prior  = p;
        }

        int tileNr;
        int palNr;
        int vFlip;
        int hFlip;
        int prior;

        QTextStream & operator<<(  QTextStream & os )

        {
            int lo = ( tileNr & 0xff );
            int hi = ( (tileNr & 0x300) >> 8 ) | ( palNr << 2 ) | ( prior << 5 ) | ( hFlip << 6 ) | ( vFlip << 7 );

             os << "$" << hex << lo <<", $" << hex << hi;

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

        Tile & getTile( unsigned int index ) { return *tiles[index]; };
        MapAttribute  getAttribute( unsigned int index ) { return *map[index]; };

        //friend ostream & operator<<( ostream & os, const MapAttribute & m )
        /*QTextStream & operator<<(  QTextStream & os, MapAttribute & m )

        {
            int lo = ( m.tileNr & 0xff );
            int hi = ( (m.tileNr & 0x300) >> 8 ) | ( m.palNr << 2 ) | ( m.prior << 5 ) | ( m.hFlip << 6 ) | ( m.vFlip << 7 );

             os << "$" << hex << lo <<", $" << hex << hi;

           return os;
        }*/

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
