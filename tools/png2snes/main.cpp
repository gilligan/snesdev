#include <iostream>
#include <math.h>

#include <qimage.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>

#include "anyoption.h"
#include "convertoptions.h"
#include "tile.h"
#include "tileconverter.h"

using namespace std;

int main( int argc, char** argv )
{
    int bpp;
    int offset;
    int palNr;
    QString label;
    QString pngFilename,outputFilename;
    QTextStream* output;
    ConvertOptions* convOpt;



    // init AnyOption command line browsing ( http://www.hackorama.com/anyoption )
    // 
    AnyOption *opt = new AnyOption();
    opt->noPOSIX();

    opt->addUsage( "Usage: png2snes [OPTIONS] [png file]" );
    opt->addUsage( "Use --help for a complete list of options" );

    opt->setCommandFlag("help", 'h');    // display help
    opt->setCommandFlag("m",'m');    // enable/disable map creation
    opt->setCommandFlag("m7",'7');       // enable/disable mode7
    opt->setCommandFlag("z",'z');        // enable/disable the empty first(zero) tile
    opt->setCommandFlag("gb",'g');       // enable gamboy map output

    opt->setCommandOption("bpp");        // set bitplanes count
    opt->setCommandOption("ofs");        // add offset to tileno.
    opt->setCommandOption("l");          // set label for output
    opt->setCommandOption("o");          // set output name
    opt->setCommandOption("p");          // set pal number
    opt->setCommandOption("t");          // swap given color to first pal entry

    opt->setCommandOption("x");          // set x-cord of rect
    opt->setCommandOption("y");          // set y-cord of rect
    opt->setCommandOption("w");          // set width of rect
    opt->setCommandOption("h");          // set height of rect

    opt->processCommandArgs(argc,argv);

    if ( opt->getFlag("help") || opt->getFlag('h') )
    {
        opt->printUsage();

        cout << "m       - enable map output [default:enabled]"<<endl;
        cout << "g       - enable gameboy output"<<endl;
        cout << "m7      - enable mode7 output" << endl;
        cout << "z       - enable empty first tile" << endl;
        cout << "bpp     - bit per plane (2,4,8) [*]"   << endl;
        cout << "ofs     - offset value in tilemap" << endl;
        cout << "l       - label name" << endl;
        cout << "o       - name of output file" << endl;
        cout << "p       - palette number" << endl;
        cout << "t       - set transparent color" << endl<<endl;
        cout << "x       - set x-coord of rect" << endl;
        cout << "y       - set y-coord of rect" << endl;
        cout << "w       - set width of rect" << endl;
        cout << "h       - set height of rect" << endl;
        cout << "[*] = required option"<<endl;
        return 0;
    }

   convOpt = new ConvertOptions( opt );
   
   if ( !convOpt->validate() )
   {
       cout << "Error in command line" << endl;
       opt->printUsage();
       return -1;
   }

   
   TileConverter* converter = new TileConverter( convOpt );
   converter->convert();

}
