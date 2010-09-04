/*****************************************************************************/
/*                                                                           */
/*                               searchpath.h                                */
/*                                                                           */
/*                    Search path path handling for ld65                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "searchpath.h"
#include "strbuf.h"
#include "xmalloc.h"



/*****************************************************************************/
/*	      	     	      	     Data		     		     */
/*****************************************************************************/



static char* SearchPaths[MAX_SEARCH_PATHS];



/*****************************************************************************/
/*	      	     	      	     Code		     		     */
/*****************************************************************************/



static char* Add (char* Orig, const char* New)
/* Create a new path from Orig and New, delete Orig, return the result */
{
    unsigned OrigLen, NewLen;
    char* NewPath;

    /* Get the length of the original string */
    OrigLen = Orig? strlen (Orig) : 0;

    /* Get the length of the new path */
    NewLen = strlen (New);

    /* Check for a trailing path separator and remove it */
    if (NewLen > 0 && (New [NewLen-1] == '\\' || New [NewLen-1] == '/')) {
    	--NewLen;
    }

    /* Allocate memory for the new string */
    NewPath = (char*) xmalloc (OrigLen + NewLen + 2);

    /* Copy the strings */
    memcpy (NewPath, Orig, OrigLen);
    memcpy (NewPath+OrigLen, New, NewLen);
    NewPath [OrigLen+NewLen+0] = ';';
    NewPath [OrigLen+NewLen+1] = '\0';

    /* Delete the original path */
    xfree (Orig);

    /* Return the new path */
    return NewPath;
}



static char* Find (const char* Path, const char* File)
/* Search for a file in a list of directories. If found, return the complete
 * name including the path in a malloced data area, if not found, return 0.
 */
{
    const char* P;
    StrBuf PathName = AUTO_STRBUF_INITIALIZER;

    /* Initialize variables */
    P = Path;

    /* Handle a NULL pointer as replacement for an empty string */
    if (P == 0) {
	P = "";
    }

    /* Start the search */
    while (*P) {
	/* Clear the string buffer */
	SB_Clear (&PathName);

        /* Copy the next path element into the buffer */
     	while (*P != '\0' && *P != ';') {
	    SB_AppendChar (&PathName, *P++);
     	}

	/* Add a path separator and the filename */
       	if (SB_NotEmpty (&PathName)) {
     	    SB_AppendChar (&PathName, '/');
	}
	SB_AppendStr (&PathName, File);
    	SB_Terminate (&PathName);

	/* Check if this file exists */
       	if (access (SB_GetBuf (&PathName), 0) == 0) {
	    /* The file exists, return its name */
	    char* Name = xstrdup (SB_GetBuf (&PathName));
	    SB_Done (&PathName);
	    return Name;
	}

	/* Skip a list separator if we have one */
	if (*P == ';') {
	    ++P;
	}
    }

    /* Not found */
    SB_Done (&PathName);
    return 0;
}



void AddSearchPath (const char* NewPath, unsigned Where)
/* Add a new search path to the existing one */
{
    /* Allow a NULL path */
    if (NewPath) {
        unsigned I;
        for (I = 0; I < MAX_SEARCH_PATHS; ++I) {
            unsigned Mask = (0x01U << I);
            if (Where & Mask) {
                SearchPaths[I] = Add (SearchPaths[I], NewPath);
            }
        }
    }
}



void AddSearchPathFromEnv (const char* EnvVar, unsigned Where)
/* Add a search path from an environment variable */
{
    AddSearchPath (getenv (EnvVar), Where);
}



void AddSubSearchPathFromEnv (const char* EnvVar, const char* SubDir, unsigned Where)
/* Add a search path from an environment variable, adding a subdirectory to
 * the environment variable value.
 */
{
    StrBuf Dir = AUTO_STRBUF_INITIALIZER;

    const char* EnvVal = getenv (EnvVar);
    if (EnvVal == 0) {
    	/* Not found */
    	return;
    }

    /* Copy the environment variable to the buffer */
    SB_CopyStr (&Dir, EnvVal);

    /* Add a path separator if necessary */
    if (SB_NotEmpty (&Dir)) {
	if (SB_LookAtLast (&Dir) != '\\' && SB_LookAtLast (&Dir) != '/') {
	    SB_AppendChar (&Dir, '/');
	}
    }

    /* Add the subdirectory */
    SB_AppendStr (&Dir, SubDir);

    /* Terminate the string */
    SB_Terminate (&Dir);

    /* Add the search path */
    AddSearchPath (SB_GetConstBuf (&Dir), Where);

    /* Free the temp buffer */
    SB_Done (&Dir);
}



void ForgetAllSearchPaths (unsigned Where)
/* Forget all search paths in the given lists. */
{
    unsigned I;
    for (I = 0; I < MAX_SEARCH_PATHS; ++I) {
        unsigned Mask = (0x01U << I);
        if (Where & Mask) {
            xfree (SearchPaths[I]);
            SearchPaths[I] = 0;
        }
    }
}



char* SearchFile (const char* Name, unsigned Where)
/* Search for a file in a list of directories. Return a pointer to a malloced
 * area that contains the complete path, if found, return 0 otherwise.
 */
{
    unsigned I;
    for (I = 0; I < MAX_SEARCH_PATHS; ++I) {
        unsigned Mask = (0x01U << I);
        if (Where & Mask) {
            char* Path = Find (SearchPaths[I], Name);
            if (Path) {
                /* Found the file */
                return Path;
            }
        }
    }
    return 0;
}



