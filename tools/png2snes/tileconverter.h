#ifndef TILE_CONVERTER_H
#define TILE_CONVERTER_H

#include <iostream>

#include <qimage.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qtextstream.h>

#include "convertoptions.h"
#include "tile.h"
#include "tileptrlist.h"
#include "snespal.h"
#include "tilemap.h"

using namespace std;

class TileConverter 
{

    public:

        TileConverter( QImage & img, int bit, QString l, int ofs, int pal, QTextStream & output );
        TileConverter( ConvertOptions* o );
        ~TileConverter();

        void grabTiles();
        void grabPalette( unsigned int c );
        bool convert();
        void output();
        void report();

    private:

        void swapColor(uint);

        SNESPal* snesPal;
        TileMap tileMap;
        Tile* emptyTile;
        ConvertOptions* opt;

        int bitPlanes;
        int palette[256*2];
        QString label;
        int tileOffset;
        int palNumber;

    protected:
    
};

#endif // TILE_CONVERTER_H
