#ifndef TILE_H
#define TILE_H

#include <iostream>
#include <qimage.h>
#include <qmemarray.h>
#include <qtextstream.h>

using namespace std;

class Tile
{
 
    public:

        Tile(int bit);
        Tile( QImage & img, int x, int y, int bit, bool m7=false );
        ~Tile();

        bool operator==( Tile & t ) const;

        QTextStream & operator<< ( QTextStream & os );


    private:

        bool mode7;
        int bitPlanesCount;
        QMemArray<int>* pixelData;
        unsigned int paletteNr;

};
#endif // TILE_H
