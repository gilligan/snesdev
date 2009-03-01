#ifndef SNESPAL_H
#define SNESPAL_H

#include <vector>

#include <qimage.h>
#include <qcolor.h>

using namespace std;

class SNESCol {

    SNESCol( uint val ) { 
   
    }
    
    uint BGR;
    uint B;
    uint G;
    uint R;
};

class SNESPal {

    public:

        SNESPal( QImage*  img, unsigned int c );
        ~SNESPal() {};

        uint getColor( uint num );
        static uint packBGR( uint rgbVal );
        static uint getLoBGRNibble( uint bgrVal );
        static uint getHiBGRNibble( uint bgrVal );
        
        QTextStream & operator<<( QTextStream & os );

    private:

        const int numColors;
        vector<uint> colTableBGR15bit;
};

#endif // SNESPAL_H
