#include "snespal.h"

SNESPal::SNESPal( QImage*  img, unsigned int col )
    :numColors( img->numColors() )
{
    QRgb* colTable = img->colorTable();
   
    int i; 
    
    for ( i=0; i<img->numColors(); i++ )
    {
        colTableBGR15bit.push_back( packBGR( colTable[i] ) );
    }

    if ( i<col )
    {   // fill remaning palette entries with 0s
        for ( int j=0; j < (col-i); j++ )
            colTableBGR15bit.push_back(0);
    }
    
}

uint SNESPal::getColor( uint num )
{
    return colTableBGR15bit.at( num );
}

uint SNESPal::getLoBGRNibble( uint bgrVal )
{
    return ( bgrVal & 0xff );
}

uint SNESPal::getHiBGRNibble( uint bgrVal )
{
    return ( ( bgrVal & 0x7f00 ) >> 8 );
}

uint SNESPal::packBGR( uint rgbVal )
{
          uint b = (int) (float)(((float)qBlue( rgbVal )/256) * 32 );
          uint g = (int) (float)(((float)qGreen( rgbVal )/256) * 32 );
          uint r = (int) (float)(((float)qRed( rgbVal )/256)*32 );

          //return   ( ( ( r<<10 ) | ( b<<5 )) | g );
          return ( ( ( b<<10 ) | ( g<<5)) | r );

}

QTextStream & SNESPal::operator <<( QTextStream & os )
{
    for ( int i=0; i<numColors; i++ )
    {
       if ( i%8 == 0 )
           os << endl << ".db ";
       
       else if ( i!=0 )
           os << ",";

       os << hex << "$" << getLoBGRNibble( colTableBGR15bit.at(i) ) << ",$" << getHiBGRNibble( colTableBGR15bit.at(i) );
    }
    return os;
}
