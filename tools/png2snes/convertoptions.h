#ifndef CONVERT_OPTIONS_H
#define CONVERT_OPTIONS_H

#include "qimage.h"
#include "qfile.h"
#include "qtextstream.h"
#include "anyoption.h"

using namespace std;

class ConvertOptions{

    public:

        ConvertOptions( AnyOption* o );
        bool validate();
        bool validateImage( QString f );
        void convert();

        QTextStream* getOutDev()   { return outDev; }
        unsigned int getOffset()   { return tileOfs; }
        unsigned int imageHeight() { return pngImage->height(); }
        unsigned int imageWidth()  { return pngImage->width();  }
        QImage*      getImagePtr() { return pngImage; }
        unsigned int getBpp()      { return bpp; }
        unsigned int getPalNr()    { return palNr; }
        unsigned int getTranspCol(){ return transpCol; }
        QString      getLabel()    { return outputLabel; }

        bool         createMap()  { return generate_map; }
        bool         insertBlank() { return emptyTile; }
        bool         gameboyMode() { return gbMode;}

        

    private:

        AnyOption* opt;
        QTextStream* outDev;
        QString pngFilename;
        QString outputLabel;
        QImage* pngImage;

        unsigned int bpp;
        unsigned int palNr;
        unsigned int transpCol;
        unsigned int tileOfs;

        bool generate_map;
        bool emptyTile;
        bool gbMode;
        


    protected:

};

#endif // CONVERT_OPTIONS_H
