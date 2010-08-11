/*****************************************************************************/
/*                                                                           */
/*				   filepath.c				     */
/*                                                                           */
/*                    File search path handling for ld65                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



/* common */
#include "searchpath.h"

/* ld65 */
#include "filepath.h"



/*****************************************************************************/
/*	       	     	     	     Code      		     		     */
/*****************************************************************************/



void InitSearchPaths (void)
/* Initialize the path search list */
{
    /* Always search all stuff in the current directory */
    AddSearchPath ("", SEARCH_LIB | SEARCH_OBJ | SEARCH_CFG);

    /* Add some compiled in search paths if defined at compile time */
#if defined(LD65_LIB)
    AddSearchPath (LD65_LIB, SEARCH_LIB);
#endif
#if defined(LD65_OBJ)
    AddSearchPath (LD65_OBJ, SEARCH_OBJ);
#endif
#if defined(LD65_CFG)
    AddSearchPath (LD65_CFG, SEARCH_CFG);
#endif

    /* Add specific paths from the environment */
    AddSearchPathFromEnv ("LD65_CFG", SEARCH_CFG);
    AddSearchPathFromEnv ("LD65_LIB", SEARCH_LIB);
    AddSearchPathFromEnv ("LD65_OBJ", SEARCH_OBJ);

    /* Add paths relative to a main directory defined in an env var */
    AddSubSearchPathFromEnv ("CC65_HOME", "cfg", SEARCH_CFG);
    AddSubSearchPathFromEnv ("CC65_HOME", "lib", SEARCH_LIB);
    AddSubSearchPathFromEnv ("CC65_HOME", "obj", SEARCH_OBJ);
}



