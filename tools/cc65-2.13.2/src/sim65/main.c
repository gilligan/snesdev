/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*                              sim65 main program                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

/* common */
#include "abend.h"
#include "cmdline.h"
#include "print.h"
#include "version.h"
#include "xmalloc.h"

/* sim65 */
#include "chip.h"
#include "chippath.h"
#include "config.h"
#include "cpucore.h"
#include "cputype.h"
#include "error.h"
#include "global.h"
#include "memory.h"
#include "scanner.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
{
    printf ("Usage: %s [options] file\n"
            "Short options:\n"
            "  -C name\t\tUse simulator config file\n"
            "  -L dir\t\tSet a chip directory search path\n"
            "  -V\t\t\tPrint the simulator version number\n"
            "  -d\t\t\tDebug mode\n"
            "  -h\t\t\tHelp (this text)\n"
            "  -v\t\t\tIncrease verbosity\n"
            "\n"
            "Long options:\n"
            "  --chipdir dir\t\tSet a chip directory search path\n"
            "  --config name\t\tUse simulator config file\n"
            "  --cpu type\t\tSet cpu type\n"
            "  --debug\t\tDebug mode\n"
            "  --help\t\tHelp (this text)\n"
            "  --verbose\t\tIncrease verbosity\n"
            "  --version\t\tPrint the simulator version number\n",
            ProgName);
}



static void OptChipDir (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --chipdir option */
{
    struct dirent* E;

    /* Get the length of the directory name */
    unsigned DirLen = strlen (Arg);

    /* Open the directory */
    DIR* D = opendir (Arg);
    if (D == 0) {
        AbEnd ("Cannot read directory `%s': %s", Arg, strerror (errno));
    }

    /* Read in all files and treat them as libraries */
    while ((E = readdir (D)) != 0) {

	char*  Name;
        struct stat S;

	/* ### Ignore anything buy *.so files */
	unsigned NameLen = strlen (E->d_name);
	if (NameLen <= 3) {
	    continue;
	}
	if (strcmp (E->d_name + NameLen - 3, ".so") != 0) {
	    continue;
	}

        /* Create the full file name */
        Name = xmalloc (DirLen + 1 + NameLen + 1);
        strcpy (Name, Arg);
        strcpy (Name + DirLen, "/");
        strcpy (Name + DirLen + 1, E->d_name);

        /* Stat the file */
        if (stat (Name, &S) != 0) {
            Warning ("Cannot stat `%s': %s", Name, strerror (errno));
            xfree (Name);
            continue;
        }

        /* Check if this is a regular file */
        if (S_ISREG (S.st_mode)) {
            /* Treat it as a library */
            LoadChipLibrary (Name);
        }

        /* Free the name */
        xfree (Name);
    }

    /* Close the directory */
    closedir (D);
}



static void OptCPU (const char* Opt, const char* Arg)
/* Handle the --cpu option */
{
    if (strcmp (Arg, "6502") == 0) {
       	CPU = CPU_6502;
    } else if (strcmp (Arg, "65C02") == 0) {
	CPU = CPU_65C02;
    } else {
       	AbEnd ("Invalid argument for %s: `%s'", Opt, Arg);
    }
}



static void OptConfig (const char* Opt attribute ((unused)), const char* Arg)
/* Define the config file */
{
    if (CfgAvail ()) {
	Error ("Cannot use -C twice");
    }
    CfgSetName (Arg);
}



static void OptDebug (const char* Opt attribute ((unused)),
	   	      const char* Arg attribute ((unused)))
/* Simulator debug mode */
{
    Debug = 1;
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptVerbose (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr, "sim65 V%s\n", GetVersionAsString ());
}



int main (int argc, char* argv[])
{
    /* Program long options */
    static const LongOpt OptTab[] = {
       	{ "--chipdir", 	       	1,     	OptChipDir    	    	},
       	{ "--config",  	       	1,     	OptConfig    	    	},
        { "--cpu",     	       	1, 	OptCPU 	     		},
       	{ "--debug",           	0,     	OptDebug     		},
	{ "--help", 	 	0, 	OptHelp	     		},
	{ "--verbose",	       	0, 	OptVerbose   	       	},
	{ "--version",	       	0,	OptVersion   	       	},
    };

    unsigned I;

    /* Initialize the output file name */
    const char* InputFile  = 0;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "sim65");

    /* Parse the command line */
    I = 1;
    while (I < ArgCount) {

       	/* Get the argument */
       	const char* Arg = ArgVec[I];

       	/* Check for an option */
       	if (Arg [0] == '-') {

       	    switch (Arg [1]) {

	   	case '-':
		    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
	 	    break;

		case 'd':
		    OptDebug (Arg, 0);
		    break;

  		case 'h':
		case '?':
	       	    OptHelp (Arg, 0);
		    break;

		case 'v':
		    OptVerbose (Arg, 0);
		    break;

		case 'C':
		    OptConfig (Arg, GetArg (&I, 2));
		    break;

		case 'L':
		    OptChipDir (Arg, GetArg (&I, 2));
		    break;

       	       	case 'V':
       	       	    OptVersion (Arg, 0);
       	       	    break;

       	       	default:
       	       	    UnknownOption (Arg);
       	       	    break;
       	    }
       	} else {
       	    if (InputFile) {
       	       	fprintf (stderr, "additional file specs ignored\n");
       	    } else {
       	       	InputFile = Arg;
	    }
	}

	/* Next argument */
	++I;
    }

    /* Sort the already loaded chips */
    SortChips ();

    /* Check if we have a valid configuration */
    if (!CfgAvail ()) {
       	Error ("Simulator configuration missing");
    }

    /* Initialize the simulated CPU memory */
    MemInit ();

    /* Read the config file */
    CfgRead ();

    CPUInit ();

    while (1) {
        CPURun ();
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}



