/*
 * Here is a sample of how to use AnyOption to
 * parse comand line argumnets and an ptions file
 *
 * Create  sample.txt as follows
 *
 *      # sample options file
 *      # this is a comment 
 *      convert  
 *      width : 42 
 *      title : This is a test title.
 *
 * Run the sample with any combination of the options
 *
 *      foo -convert --height 200 arg1 arg2 
 */

#include "anyoption.h"

void simple( int argc, char* argv[] );
void detailed( int argc, char* argv[] );

int
main( int argc, char* argv[] )
{
	/* Either call simple() or detailed()  		*/
	/* You cannot call both examples together 	*/
	/* since *argv[]  gets consumed by AnyOption :) */

        //simple( argc, argv );
        detailed( argc, argv ); 
        return 0 ;
}

/*
 * very simple usage 
 */

void
simple( int argc, char* argv[] )
{

        AnyOption *opt = new AnyOption();
        opt->noPOSIX(); /* use simpler option type */

        opt->setOption(  "width" );
        opt->setOption(  "height" );
        opt->setFlag( "convert");
        opt->setCommandOption(  "name" );
        opt->setFileOption(  "title" );

        if (  ! opt->processFile( "sample.txt" ) )
                cout << "Failed processing the resource file" << endl ;
        opt->processCommandArgs( argc, argv );

	cout << "THE OPTIONS : " << endl << endl ;
	if( opt->getValue( "width" ) != NULL )
        	cout << "width  : " << opt->getValue( "width" ) << endl ;
	if( opt->getValue( "height" ) != NULL )
        	cout << "height : " << opt->getValue( "height" ) << endl ;
	if( opt->getValue( "name" ) != NULL )
        	cout << "name   : " << opt->getValue( "name" ) << endl ;
	if( opt->getValue( "title" ) != NULL )
        	cout << "title  : " << opt->getValue( "title" ) << endl ;
        if( opt->getFlag( "convert" ) )  
		cout << "convert : set " << endl ;
        cout << endl ;

	cout << "THE ARGUMENTS : " << endl << endl ;
	for( int i = 0 ; i < opt->getArgc() ; i++ ){
		cout << opt->getArgv( i ) << endl  ;
	}
	cout << endl;

        delete opt;

}

/*
 * detailed  usage  with all kind of options 
 */

void
detailed( int argc, char* argv[] )
{

        /* 1. CREATE AN OBJECT */
        AnyOption *opt = new AnyOption();

        /* 2. SET PREFERENCES  */
        //opt->noPOSIX(); /* do not check for POSIX style character options */
        //opt->setVerbose(); /* print warnings about unknown options */
        //opt->noUsage(); /* stop printing Usage */

        /* 3. SET THE USAGE/HELP   */
        opt->addUsage( "Usage: foo [OPTIONS]... [IMAGE FOLDER] " );
        opt->addUsage( "Unknown options/arguments encountered " );
        opt->addUsage( "Use -h or --help for a complete list of options" );

        /* 4. SET THE OPTION STRINGS/CHARACTERS */
        opt->setFlag(  "help", 'h' ); /* for help */
        opt->setOption(  "width", 'w' );
        opt->setOption(  "height" );
        opt->setFlag( "convert");
        opt->setCommandOption(  "name", 'n' );
        opt->setCommandFlag(  "help" );
        opt->setFileOption(  "title" );

        /* 5. PROVIDE THE COMMANDLINE AND RESOURCE FILE */
        if (  ! opt->processFile( "sample.txt" ) )
                cout << "Failed processing the resource file" << endl ;
        opt->processCommandArgs( argc, argv );

        /* 6. GET THE VALUES */
        /* help */
        if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ){
                opt->printUsage();
		/* print help here */
        }
	cout << "THE OPTIONS : " << endl << endl ;
	if( opt->getValue( 'w' ) != NULL )
        	cout << "w      : " << opt->getValue( 'w' ) << endl ;
	if( opt->getValue( "width" ) != NULL )
        	cout << "width  : " << opt->getValue( "width" ) << endl ;
	if( opt->getValue( "height" ) != NULL )
        	cout << "height : " << opt->getValue( "height" ) << endl ;
	if( opt->getValue( "name" ) != NULL )
       	 	cout << "name   : " << opt->getValue( "name" ) << endl ;
	if( opt->getValue( 'n' ) != NULL )
        	cout << "n      : " << opt->getValue( 'n' ) << endl ;
	if( opt->getValue( "title" ) != NULL )
        	cout << "title  : " << opt->getValue( "title" ) << endl ;
        if( opt->getFlag( "convert" ) )  
		cout << "convert : set " << endl ;
        cout << endl ;

	/* 7. GET THE ACTUAL ARGUMNETS AFTER THE OPTIONS */
	cout << "THE ARGUMENTS : " << endl << endl ;
	for( int i = 0 ; i < opt->getArgc() ; i++ ){
		cout << opt->getArgv( i ) << endl ;
	}
	cout << endl;

        /* 7. DONE */
        delete opt;

}
