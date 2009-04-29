#include "tileconverter.h"


TileConverter::TileConverter( ConvertOptions*   o )
{
    opt = o;
    emptyTile = new Tile(opt->getBpp() );

    int delta=0;

    if ( opt->insertBlank() )
    {
        delta = 1;
        tileMap.setEmptyTile( true );
    }

    if ( opt->gameboyMode() ){
        tileMap.mode = GB_MAP_MODE;
    }

    tileMap.setTileOffset( opt->getOffset()+delta );

}

TileConverter::TileConverter( QImage & img, int bit, QString l, int ofs, int pal, QTextStream & output )
    :bitPlanes( bit ),label(l),tileOffset(ofs),palNumber(pal)
{
}

void TileConverter::grabTiles()
{
    
      for ( int y=0;y<opt->imageHeight();y+=8 )
         for ( int x=0;x<opt->imageWidth();x+=8 )
             tileMap.addTile( new Tile( *opt->getImagePtr(),x,y,opt->getBpp(),false ), opt->getPalNr(), opt->createMap() ); 
}

void TileConverter::grabPalette( unsigned int colNum )
{
   snesPal = new SNESPal( opt->getImagePtr(), colNum );  
}


bool TileConverter::convert()
{
   
    if ( opt->getTranspCol() != 0 )
        swapColor( opt->getTranspCol() );

    grabTiles();
    grabPalette( opt->getBpp() * opt->getBpp() );

    report();
    output();
    
    return true;
}

void TileConverter::swapColor( uint palEntry )
{
    printf("swapping color %d with 0 of %d\n",palEntry,opt->getImagePtr());
    QImage* img = opt->getImagePtr();
    uchar* p;
    uint height = img->height();
    uint width = img->width();
    
    for ( uint y=0; y<height; y++ )
    {
        p = img->scanLine(y);
        for ( uint x=0; x<width; x++ )
        {
            if ( p[x] == 0 )
                p[x] = palEntry;
            else if ( p[x] ==  palEntry )
                p[x] = 0;
        }
    }

    QRgb c1 = img->color(0);
    QRgb c2 = img->color(palEntry);

    img->setColor(0,c2);
    img->setColor(palEntry,c1);
}

void TileConverter::output()
{
    QTextStream* outDev = opt->getOutDev();

    *outDev << ".DEFINE " << QString( opt->getLabel() + QString("_tiles_len" )) << " " << opt->getBpp()*8*tileMap.getTileCount() << endl;

    if ( opt->createMap() )
        *outDev << ".DEFINE " << QString( opt->getLabel() + QString("_map_len" )) << " " << tileMap.getMapSize()*2 << endl;

    *outDev<<endl<< QString( opt->getLabel() + QString("_tiles:"))<<endl;
    
    if ( opt->insertBlank() )
    {
         *emptyTile << *outDev << endl;
    }


    for ( uint i=0; i<tileMap.getTileCount(); i++ )
    {
        tileMap.getTile(i) << *outDev;
        *outDev<<endl;
    }
    *outDev<<endl<< QString( opt->getLabel() + QString("_tiles_end:"))<<endl;


    if ( opt->createMap() )
    {
        *outDev<<endl<< QString( opt->getLabel() + QString("_map:"))<<endl;
        for ( uint i=0; i< (opt->imageWidth()/8 * opt->imageHeight()/8 ) ; i++ )
        {
            if ( i%16 == 0 )
                *outDev << endl << ".db ";
            else if ( i!= 0 )
                *outDev << ", ";
     
            (MapAttribute)tileMap.getAttribute(i) << *outDev;
        }
        *outDev<<endl<<endl<< QString( opt->getLabel() + QString("_map_end:")) << endl;
    }
    else
        *outDev<<endl;


    *outDev<<endl<< QString( opt->getLabel() + QString("_pal:"))<<endl;
    *(snesPal) << *outDev;
    *outDev<<endl<< endl << QString( opt->getLabel() + QString("_pal_end:")) << endl << endl;

}
    

void TileConverter::report()
{
    int m;
    m = opt->getBpp()*8;
    
    if ( opt->createMap() )
      cout << "** generated map   : " << opt->imageWidth() << "x" << opt->imageHeight() << " [ " << hex << tileMap.getMapSize()*2 << " bytes ]" << endl;

    cout << "** generated tiles : " << tileMap.getTileCount() << " [ " << hex << "$"<<m*tileMap.getTileCount() << " bytes ] " << endl<<endl;

}

TileConverter::~TileConverter()
{
}

