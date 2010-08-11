/*****************************************************************************/
/*									     */
/*				   objfile.c				     */
/*									     */
/*		   Object file handling for the ld65 linker		     */
/*									     */
/*									     */
/*									     */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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
/*    distribution.   							     */
/*		      							     */
/*****************************************************************************/



#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>		/* EMX needs this */
#include <sys/stat.h>

/* common */
#include "fname.h"
#include "xmalloc.h"

/* ld65 */
#include "asserts.h"
#include "dbgsyms.h"
#include "error.h"
#include "exports.h"
#include "fileinfo.h"
#include "fileio.h"
#include "lineinfo.h"
#include "objdata.h"
#include "objfile.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static unsigned GetModule (const char* Name)
/* Get a module name index from the file name */
{
    /* Make a module name from the file name */
    const char* Module = FindName (Name);
    if (*Module == 0) {
	Error ("Cannot make module name from `%s'", Name);
    }
    return GetStringId (Module);
}



static void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name)
/* Read the header of the object file checking the signature */
{
    H->Version	  = Read16 (Obj);
    if (H->Version != OBJ_VERSION) {
       	Error ("Object file `%s' has wrong version, expected %08X, got %08X",
               Name, OBJ_VERSION, H->Version);
    }
    H->Flags	    = Read16 (Obj);
    H->OptionOffs   = Read32 (Obj);
    H->OptionSize   = Read32 (Obj);
    H->FileOffs     = Read32 (Obj);
    H->FileSize     = Read32 (Obj);
    H->SegOffs	    = Read32 (Obj);
    H->SegSize	    = Read32 (Obj);
    H->ImportOffs   = Read32 (Obj);
    H->ImportSize   = Read32 (Obj);
    H->ExportOffs   = Read32 (Obj);
    H->ExportSize   = Read32 (Obj);
    H->DbgSymOffs   = Read32 (Obj);
    H->DbgSymSize   = Read32 (Obj);
    H->LineInfoOffs = Read32 (Obj);
    H->LineInfoSize = Read32 (Obj);
    H->StrPoolOffs  = Read32 (Obj);
    H->StrPoolSize  = Read32 (Obj);
    H->AssertOffs   = Read32 (Obj);
    H->AssertSize   = Read32 (Obj);
    H->ScopeOffs    = Read32 (Obj);
    H->ScopeSize    = Read32 (Obj);
}



void ObjReadFiles (FILE* F, unsigned long Pos, ObjData* O)
/* Read the files list from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->FileCount  = ReadVar (F);
    O->Files      = xmalloc (O->FileCount * sizeof (O->Files[0]));
    for (I = 0; I < O->FileCount; ++I) {
       	O->Files[I] = ReadFileInfo (F, O);
    }
}



void ObjReadSections (FILE* F, unsigned long Pos, ObjData* O)
/* Read the section data from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->SectionCount = ReadVar (F);
    O->Sections     = xmalloc (O->SectionCount * sizeof (O->Sections[0]));
    for (I = 0; I < O->SectionCount; ++I) {
	O->Sections [I] = ReadSection (F, O);
    }
}



void ObjReadImports (FILE* F, unsigned long Pos, ObjData* O)
/* Read the imports from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->ImportCount = ReadVar (F);
    O->Imports     = xmalloc (O->ImportCount * sizeof (O->Imports[0]));
    for (I = 0; I < O->ImportCount; ++I) {
   	O->Imports [I] = ReadImport (F, O);
    }
}



void ObjReadExports (FILE* F, unsigned long Pos, ObjData* O)
/* Read the exports from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->ExportCount = ReadVar (F);
    O->Exports     = xmalloc (O->ExportCount * sizeof (O->Exports[0]));
    for (I = 0; I < O->ExportCount; ++I) {
	O->Exports [I] = ReadExport (F, O);
    }
}



void ObjReadDbgSyms (FILE* F, unsigned long Pos, ObjData* O)
/* Read the debug symbols from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->DbgSymCount = ReadVar (F);
    O->DbgSyms	   = xmalloc (O->DbgSymCount * sizeof (O->DbgSyms[0]));
    for (I = 0; I < O->DbgSymCount; ++I) {
	O->DbgSyms [I] = ReadDbgSym (F, O);
    }
}



void ObjReadLineInfos (FILE* F, unsigned long Pos, ObjData* O)
/* Read the line infos from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->LineInfoCount = ReadVar (F);
    O->LineInfos     = xmalloc (O->LineInfoCount * sizeof (O->LineInfos[0]));
    for (I = 0; I < O->LineInfoCount; ++I) {
       	O->LineInfos[I] = ReadLineInfo (F, O);
    }
}



void ObjReadStrPool (FILE* F, unsigned long Pos, ObjData* O)
/* Read the string pool from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->StringCount = ReadVar (F);
    O->Strings     = xmalloc (O->StringCount * sizeof (O->Strings[0]));
    for (I = 0; I < O->StringCount; ++I) {
        O->Strings[I] = ReadStr (F);
    }
}



void ObjReadAssertions (FILE* F, unsigned long Pos, ObjData* O)
/* Read the assertions from a file at the given offset */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->AssertionCount = ReadVar (F);
    O->Assertions     = xmalloc (O->AssertionCount * sizeof (O->Assertions[0]));
    for (I = 0; I < O->AssertionCount; ++I) {
        O->Assertions[I] = ReadAssertion (F, O);
    }
}



void ObjReadScopes (FILE* F, unsigned long Pos, ObjData* O)
/* Read the scope table from a file at the given offset */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->ScopeCount = ReadVar (F);
    O->Scopes     = xmalloc (O->ScopeCount * sizeof (O->Scopes[0]));
    for (I = 0; I < O->ScopeCount; ++I) {
        O->Scopes[I] = 0;       /* ReadScope (F, O); ### not implemented */
    }
}



void ObjAdd (FILE* Obj, const char* Name)
/* Add an object file to the module list */
{
    /* Create a new structure for the object file data */
    ObjData* O = NewObjData ();

    /* The magic was already read and checked, so set it in the header */
    O->Header.Magic = OBJ_MAGIC;

    /* Read and check the header */
    ObjReadHeader (Obj, &O->Header, Name);

    /* Initialize the object module data structure */
    O->Name  = GetModule (Name);

    /* Read the string pool from the object file */
    ObjReadStrPool (Obj, O->Header.StrPoolOffs, O);

    /* Read the files list from the object file */
    ObjReadFiles (Obj, O->Header.FileOffs, O);

    /* Read the imports list from the object file */
    ObjReadImports (Obj, O->Header.ImportOffs, O);

    /* Read the object file exports and insert them into the exports list */
    ObjReadExports (Obj, O->Header.ExportOffs, O);

    /* Read the object debug symbols from the object file */
    ObjReadDbgSyms (Obj, O->Header.DbgSymOffs, O);

    /* Read the line infos from the object file */
    ObjReadLineInfos (Obj, O->Header.LineInfoOffs, O);

    /* Read the assertions from the object file */
    ObjReadAssertions (Obj, O->Header.AssertOffs, O);

    /* Read the scope table from the object file */
    ObjReadScopes (Obj, O->Header.ScopeOffs, O);

    /* Read the segment list from the object file. This must be last, since
     * the expressions stored in the code may reference segments or imported
     * symbols.
     */
    ObjReadSections (Obj, O->Header.SegOffs, O);

    /* Mark this object file as needed */
    O->Flags |= OBJ_REF;

    /* Done, close the file (we read it only, so no error check) */
    fclose (Obj);

    /* Insert the imports and exports to the global lists */
    InsertObjGlobals (O);

    /* Insert the object into the list of all used object files */
    InsertObjData (O);

    /* All references to strings are now resolved, so we can delete the module
     * string pool.
     */
    FreeObjStrings (O);
}




