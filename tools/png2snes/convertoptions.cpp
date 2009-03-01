#include "convertoptions.h"

ConvertOptions::ConvertOptions( AnyOption* o )
{
    opt = o;
}

void ConvertOptions::convert()
{
        
}

bool ConvertOptions::validate()
{
    // check for mandatory arguments
    if ( (opt->getValue("bpp") == NULL && !opt->getFlag("7")) || opt->getArgc() != 1 )
        return false;

    // get bpp and filename
    bpp = atoi( opt->getValue("bpp") );
    pngFilename = opt->getArgv(0);
    pngImage = new QImage(pngFilename);

   if ( opt->getFlag("gb"))
   {
       if ( bpp!=2 )
           return false;

   }

   if ( opt->getFlag("m") )
   {
      generate_map=true; 
   } 
   else generate_map=false;

   if ( opt->getFlag("z") )
   {
       emptyTile=true;
   }
   else emptyTile=false;

    // get pal number   
    if ( opt->getValue("p") != NULL )
        palNr = atoi( opt->getValue("p") );
    else palNr = 0;

    // get color to be set transparent ( first pal entry )
    if ( opt->getValue("t") != NULL )
    {
        transpCol = atoi( opt->getValue("t") );
        if ( transpCol > pngImage->numColors() )
            return false;
    }
    else transpCol = 0;

    // if no name for the label is defined default to "gfx"
    if ( opt->getValue("l") != NULL )
        outputLabel = opt->getValue("l");
    else outputLabel = "gfx";

    // if no offset value for the map is defined default to 0
    if ( opt->getValue("ofs") != NULL )
        tileOfs = atoi( opt->getValue("ofs") );
    else tileOfs = 0;

    // if no output filename is defined output to stdout
    if ( opt->getValue("o") != NULL )
    {
        QFile* file = new QFile( opt->getValue("o") );
        file->open( IO_WriteOnly );
        outDev = new QTextStream( file );
        cout<<"Writing output to "<<file->name()<<endl;
    }
    else outDev = new QTextStream(stdout,IO_WriteOnly);

    return true;
}

bool validateImage( QImage*  srcImage )
{
    // load and check source image
    // 

    if ( srcImage->isNull() )
    {
        cerr<<"Failed to load image!"<<endl;
        cerr<<"Make sure the image is in a supported format"<<endl;
        cerr<<"Supported Formats are :"<<endl;
        cerr<<"PNG, BMP, XBM, XPM, PNM"<<endl;
        return -1;
    }

    if ( srcImage->numColors() == 0 )
    {
        cerr<<"Error: Image does not have a color table"<<endl;
        cerr<<"Convert from RGB to indexed format!"<<endl;
        return -1;
    }

    if ( srcImage->numColors() > 256 )
    {
        cerr<<"Error: Image has more than 256 colors!"<<endl;
        cerr<<"Reduce colors!"<<endl;
        return -1;
    }

    if ( ( srcImage->width() % 8 ) != 0 || ( srcImage->height() % 8 ) != 0 )
    {
        cerr<<"Error: width and height of image have to be mulitples of 8 !"<<endl;
        cerr<<"Resize Image!"<<endl;
        return -1;
    }
    return true;
}
