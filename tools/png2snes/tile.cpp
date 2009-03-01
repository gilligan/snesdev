#include "tile.h"

Tile::Tile(int bit)
{
    pixelData = new QMemArray<int>(64);
    pixelData->fill(0);

    bitPlanesCount = bit;
    mode7 = false;

}

Tile::Tile( QImage & img , int x, int y, int bit, bool m7 )
{

    pixelData = new QMemArray<int>(64);
    bitPlanesCount = bit;
    mode7=m7;

    int i=0;
    
    for ( int dy=0;dy<8;dy++) 
        for ( int dx=0;dx<8;dx++ )
           pixelData->at(i++) = img.pixelIndex( x+dx ,y+dy );

}

Tile::~Tile()
{
}

bool Tile::operator==( Tile & t ) const
{
    return ( *pixelData == *t.pixelData );
}


QTextStream & Tile::operator<< ( QTextStream & os )
{
    int bitMask=1;
    int planes=bitPlanesCount;
    
    unsigned int line=0;
    unsigned int bitplanes[planes][64];

    int tmp;

    if ( mode7 )
    {
        for ( int i=0; i<64; i++ )
        {
            if ( i%8 == 0 )
                os << endl << ".db ";
            else 
                os << ", ";

           os << "$" << hex << pixelData->at(i);
        }

        return os;
    }

    // create  bitplanes
    // 
    for ( int i=0; i<planes; i++ )
    {
        for ( int j=0; j<64; j++ )
            bitplanes[i][j] = ( pixelData->at(j) & bitMask ) >> i;
        
        bitMask = bitMask << 1;
    }


    // pack planes to bytes and output
    //
    int buffer[planes][8];
    unsigned int x=0;
    
    for ( int i=0; i<planes; i++ )
    {
        for ( int j=0; j<8; j++ )
        {
            line=0;
            
            for ( int k=0; k<8; k++ )
                line = ( line | ( bitplanes[i][(j*8)+k] << (7-k) ) );

            buffer[i][j] = line;

        }
    }

    int i=0;


    if ( planes == 2 )
    {
        // Description: 
        // bitplanes 1 & 2 are stored interwined row by row
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=0; plane<2; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }
       
    }
    
    if ( planes == 4 )
    {
        // Description:
        // First, bitplanes 1 & 2 are stored interwined row by row
        // Then,  bitplanes 3 & 4 are stored interwined row by row
 
        // get bitplanes 1 & 2
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=0; plane<2; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }

        // get planes 3 & 4
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=2; plane<4; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }
    }

    if ( planes == 8 )
    {
        // Description:
        // First, bitplanes 1 & 2 are stored interwined row by row
        // Then,  bitplanes 3 & 4 are stored interwined row by row
        // Then,  bitplanes 5 & 6 are stored interwined row by row
        // Then,  bitplanes 7 & 8 are storer interwined row by row

        // get bitplanes 1 & 2
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=0; plane<2; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }

        // get planes 3 & 4
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=2; plane<4; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }

        // get bitplanes 5 & 6 
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=4; plane<6; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }

        // get planes 7 & 8
        for ( int scanline=0; scanline<8; scanline++ )
        {
            for ( int plane=6; plane<8; plane++ )
            {
                if ( i%8 == 0 )
                    os << endl << ".db ";
                else if ( i!=0 )
                    os << ", ";

                os << "$" << hex << buffer[plane][scanline];
                i++;
            }
        }
    }

    return os;
}
