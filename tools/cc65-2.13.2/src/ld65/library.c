/*****************************************************************************/
/*					   				     */
/*				   library.c				     */
/*					   				     */
/*	    Library data structures and helpers for the ld65 linker	     */
/*					   				     */
/*					   				     */
/*					   				     */
/* (C) 1998-2005 Ullrich von Bassewitz                                       */
/*               R�merstra�e 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "exprdefs.h"
#include "filepos.h"
#include "libdefs.h"
#include "objdefs.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "exports.h"
#include "fileio.h"
#include "library.h"
#include "objdata.h"
#include "objfile.h"
#include "spool.h"



/*****************************************************************************/
/*	  			     Data				     */
/*****************************************************************************/



/* Library data structure */
typedef struct Library Library;
struct Library {
    Library*    Next;
    unsigned    Name;           /* String id of the name */
    FILE*       F;              /* Open file stream */
    LibHeader   Header;         /* Library header */
    unsigned    ModCount;       /* Number of modules in the library */
    ObjData**   Modules;        /* Modules */
};

/* List of open libraries */
static Collection OpenLibs = STATIC_COLLECTION_INITIALIZER;

/* Flag for library grouping */
static int Grouping = 0;



/*****************************************************************************/
/*                              struct Library                               */
/*****************************************************************************/



static Library* NewLibrary (FILE* F, const char* Name)
/* Create a new Library structure and return it */
{
    /* Allocate memory */
    Library* L = xmalloc (sizeof (*L));

    /* Initialize the fields */
    L->Next     = 0;
    L->Name     = GetStringId (Name);
    L->F        = F;
    L->ModCount = 0;
    L->Modules  = 0;

    /* Return the new struct */
    return L;
}



static void FreeLibrary (Library* L)
/* Free a library structure */
{
    /* Close the library file */
    if (fclose (L->F) != 0) {
        Error ("Error closing `%s': %s", GetString (L->Name), strerror (errno));
    }

    /* Free the module index */
    xfree (L->Modules);

    /* Free the library structure */
    xfree (L);
}



/*****************************************************************************/
/*	       		 Reading file data structures			     */
/*****************************************************************************/



static void LibSeek (Library* L, unsigned long Offs)
/* Do a seek in the library checking for errors */
{
    if (fseek (L->F, Offs, SEEK_SET) != 0) {
        Error ("Seek error in `%s' (%lu): %s",
               GetString (L->Name), Offs, strerror (errno));
    }
}



static void LibReadHeader (Library* L)
/* Read a library header */
{
    /* Read the remaining header fields (magic is already read) */
    L->Header.Magic   = LIB_MAGIC;
    L->Header.Version = Read16 (L->F);
    if (L->Header.Version != LIB_VERSION) {
       	Error ("Wrong data version in `%s'", GetString (L->Name));
    }
    L->Header.Flags   = Read16 (L->F);
    L->Header.IndexOffs = Read32 (L->F);
}



static void LibReadObjHeader (Library* L, ObjData* O)
/* Read the header of the object file checking the signature */
{
    O->Header.Magic = Read32 (L->F);
    if (O->Header.Magic != OBJ_MAGIC) {
	Error ("Object file `%s' in library `%s' is invalid",
	       GetObjFileName (O), GetString (L->Name));
    }
    O->Header.Version = Read16 (L->F);
    if (O->Header.Version != OBJ_VERSION) {
	Error ("Object file `%s' in library `%s' has wrong version",
	       GetObjFileName (O), GetString (L->Name));
    }
    O->Header.Flags    	   = Read16 (L->F);
    O->Header.OptionOffs   = Read32 (L->F);
    O->Header.OptionSize   = Read32 (L->F);
    O->Header.FileOffs	   = Read32 (L->F);
    O->Header.FileSize	   = Read32 (L->F);
    O->Header.SegOffs	   = Read32 (L->F);
    O->Header.SegSize	   = Read32 (L->F);
    O->Header.ImportOffs   = Read32 (L->F);
    O->Header.ImportSize   = Read32 (L->F);
    O->Header.ExportOffs   = Read32 (L->F);
    O->Header.ExportSize   = Read32 (L->F);
    O->Header.DbgSymOffs   = Read32 (L->F);
    O->Header.DbgSymSize   = Read32 (L->F);
    O->Header.LineInfoOffs = Read32 (L->F);
    O->Header.LineInfoSize = Read32 (L->F);
    O->Header.StrPoolOffs  = Read32 (L->F);
    O->Header.StrPoolSize  = Read32 (L->F);
    O->Header.AssertOffs   = Read32 (L->F);
    O->Header.AssertSize   = Read32 (L->F);
    O->Header.ScopeOffs    = Read32 (L->F);
    O->Header.ScopeSize    = Read32 (L->F);
}



static ObjData* ReadIndexEntry (Library* L)
/* Read one entry in the index */
{
    /* Create a new entry and insert it into the list */
    ObjData* O	= NewObjData ();

    /* Module name */
    O->Name = ReadStr (L->F);

    /* Module flags/MTime/Start/Size */
    O->Flags	= Read16 (L->F);
    O->MTime    = Read32 (L->F);
    O->Start	= Read32 (L->F);
    Read32 (L->F);			/* Skip Size */

    /* Read the string pool */
    ObjReadStrPool (L->F, FileGetPos (L->F), O);

    /* Skip the export size, then read the exports */
    (void) ReadVar (L->F);
    ObjReadExports (L->F, FileGetPos (L->F), O);

    /* Skip the import size, then read the imports */
    (void) ReadVar (L->F);
    ObjReadImports (L->F, FileGetPos (L->F), O);

    /* Done */
    return O;
}



static void LibReadIndex (Library* L)
/* Read the index of a library file */
{
    unsigned I;

    /* Seek to the start of the index */
    LibSeek (L, L->Header.IndexOffs);

    /* Read the object file count and allocate memory */
    L->ModCount = ReadVar (L->F);
    L->Modules  = xmalloc (L->ModCount * sizeof (L->Modules[0]));

    /* Read all entries in the index */
    for (I = 0; I < L->ModCount; ++I) {
       	L->Modules[I] = ReadIndexEntry (L);
    }
}



/*****************************************************************************/
/*  	   	  	       High level stuff				     */
/*****************************************************************************/



static void LibCheckExports (ObjData* O)
/* Check if the exports from this file can satisfy any import requests. If so,
 * insert the imports and exports from this file and mark the file as added.
 */
{
    unsigned I;

    /* Check all exports */
    for (I = 0; I < O->ExportCount; ++I) {
	if (IsUnresolved (O->Exports[I]->Name)) {
	    /* We need this module */
	    O->Flags |= OBJ_REF;    	    break;
    	}
    }

    /* If we need this module, insert the imports and exports */
    if (O->Flags & OBJ_REF) {
        InsertObjGlobals (O);
    }
}



static void LibOpen (FILE* F, const char* Name)
/* Open the library for use */
{
    /* Create a new library structure */
    Library* L = NewLibrary (F, Name);

    /* Read the remaining header fields (magic is already read) */
    LibReadHeader (L);

    /* Seek to the index position and read the index */
    LibReadIndex (L);

    /* Add the library to the list of open libraries */
    CollAppend (&OpenLibs, L);
}



static void LibResolve (void)
/* Resolve all externals from the list of all currently open libraries */
{
    unsigned I, J;
    unsigned Additions;

    /* Walk repeatedly over all open libraries until there's nothing more
     * to add.
     */
    do {

        Additions = 0;

        /* Walk over all libraries */
        for (I = 0; I < CollCount (&OpenLibs); ++I) {

            /* Get the next library */
            Library* L = CollAt (&OpenLibs, I);

            /* Walk through all modules in this library and check for each
             * module if there are unresolved externals in existing modules
             * that may be resolved by adding the module.
             */
            for (J = 0; J < L->ModCount; ++J) {

                /* Get the next module */
                ObjData* O = L->Modules[J];

                /* We only need to check this module if it wasn't added before */
                if ((O->Flags & OBJ_REF) == 0) {
                    LibCheckExports (O);
                    if (O->Flags & OBJ_REF) {
                        /* The routine added the file */
                        ++Additions;
                    }
                }
            }
        }

    } while (Additions > 0);

    /* We do know now which modules must be added, so we can load the data
     * for these modues into memory. Since we're walking over all modules
     * anyway, we will also remove data for unneeded modules.
     */
    for (I = 0; I < CollCount (&OpenLibs); ++I) {

        /* Get the next library */
        Library* L = CollAt (&OpenLibs, I);

        /* Walk over all modules in this library and add the files list and
         * sections for all referenced modules.
         */
        for (J = 0; J < L->ModCount; ++J) {

            /* Get the object data */
            ObjData* O = L->Modules[J];

            /* Is this object file referenced? */
            if (O->Flags & OBJ_REF) {

                /* Seek to the start of the object file and read the header */
                LibSeek (L, O->Start);
                LibReadObjHeader (L, O);

                /* Seek to the start of the files list and read the files list */
                ObjReadFiles (L->F, O->Start + O->Header.FileOffs, O);

                /* Seek to the start of the debug info and read the debug info */
                ObjReadDbgSyms (L->F, O->Start + O->Header.DbgSymOffs, O);

                /* Seek to the start of the line infos and read them */
                ObjReadLineInfos (L->F, O->Start + O->Header.LineInfoOffs, O);

                /* Read the assertions from the object file */
                ObjReadAssertions (L->F, O->Start + O->Header.AssertOffs, O);

                /* Read the scope table from the object file */
                ObjReadScopes (L->F, O->Start + O->Header.ScopeOffs, O);

                /* Seek to the start of the segment list and read the segments.
                 * This must be last, since the data here may reference other
                 * stuff.
                 */
                ObjReadSections (L->F, O->Start + O->Header.SegOffs, O);

                /* Remember from which library this module is */
                O->LibName = L->Name;

                /* All references to strings are now resolved, so we can delete
                 * the module string pool.
                 */
                FreeObjStrings (O);

                /* Insert the object into the list of all used object files */
                InsertObjData (O);

            } else {

                /* Unreferenced object file, remove it */
                FreeObjData (O);

            }
        }

        /* Close the file and delete the library data */
        FreeLibrary (L);
    }

    /* We're done with all open libraries, clear the OpenLibs collection */
    CollDeleteAll (&OpenLibs);
}



void LibAdd (FILE* F, const char* Name)
/* Add files from the library to the list if there are references that could
 * be satisfied.
 */
{
    /* Add the library to the list of open libraries */
    LibOpen (F, Name);

    /* If there is no library group open, just resolve all open symbols and
     * close the library. Otherwise we will do nothing because resolving will
     * be done when the group is closed.
     */
    if (!Grouping) {
        LibResolve ();
    }
}



void LibStartGroup (void)
/* Start a library group. Objects within a library group may reference each
 * other, and libraries are searched repeatedly until all references are
 * satisfied.
 */
{
    /* We cannot already have a group open */
    if (Grouping) {
        Error ("There's already a library group open");
    }

    /* Start a new group */
    Grouping = 1;
}



void LibEndGroup (void)
/* End a library group and resolve all open references. Objects within a
 * library group may reference each other, and libraries are searched
 * repeatedly until all references are satisfied.
 */
{
    /* We must have a library group open */
    if (!Grouping) {
        Error ("There's no library group open");
    }

    /* Resolve symbols, end the group */
    LibResolve ();
    Grouping = 0;
}



void LibCheckGroup (void)
/* Check if there are open library groups */
{
    if (Grouping) {
        Error ("Library group was never closed");
    }
}



