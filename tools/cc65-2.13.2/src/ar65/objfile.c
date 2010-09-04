/*****************************************************************************/
/*									     */
/*				   objfile.c				     */
/*									     */
/*		  Object file handling for the ar65 archiver		     */
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
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#include <string.h>
#include <errno.h>
#if defined(__WATCOMC__) || defined(_MSC_VER) || defined(__MINGW32__)
/* The Windows compilers have the file in the wrong directory */
#  include <sys/utime.h>
#else
#  include <sys/types.h>     		/* FreeBSD needs this */
#  include <utime.h>
#endif
#include <time.h>
#include <sys/stat.h>

/* common */
#include "xmalloc.h"

/* ar65 */
#include "error.h"
#include "objdata.h"
#include "fileio.h"
#include "library.h"
#include "objfile.h"



/*****************************************************************************/
/*			     	     Code				     */
/*****************************************************************************/



static const char* GetModule (const char* Name)
/* Get a module name from the file name */
{
    /* Make a module name from the file name */
    const char* Module = Name + strlen (Name);
    while (Module > Name) {
	--Module;
	if (*Module == '/' || *Module == '\\') {
	    ++Module;
	    break;
	}
    }
    if (*Module == 0) {
	Error ("Cannot make module name from `%s'", Name);
    }
    return Module;
}



void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name)
/* Read the header of the object file checking the signature */
{
    H->Magic	  = Read32 (Obj);
    if (H->Magic != OBJ_MAGIC) {
	Error ("`%s' is not an object file", Name);
    }
    H->Version	  = Read16 (Obj);
    if (H->Version != OBJ_VERSION) {
	Error ("Object file `%s' has wrong version", Name);
    }
    H->Flags   	    = Read16 (Obj);
    H->OptionOffs   = Read32 (Obj);
    H->OptionSize   = Read32 (Obj);
    H->FileOffs     = Read32 (Obj);
    H->FileSize     = Read32 (Obj);
    H->SegOffs 	    = Read32 (Obj);
    H->SegSize 	    = Read32 (Obj);
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



void ObjWriteHeader (FILE* Obj, ObjHeader* H)
/* Write the header of the object file */
{
    Write32 (Obj, H->Magic);
    Write16 (Obj, H->Version);
    Write16 (Obj, H->Flags);
    Write32 (Obj, H->OptionOffs);
    Write32 (Obj, H->OptionSize);
    Write32 (Obj, H->FileOffs);
    Write32 (Obj, H->FileSize);
    Write32 (Obj, H->SegOffs);
    Write32 (Obj, H->SegSize);
    Write32 (Obj, H->ImportOffs);
    Write32 (Obj, H->ImportSize);
    Write32 (Obj, H->ExportOffs);
    Write32 (Obj, H->ExportSize);
    Write32 (Obj, H->DbgSymOffs);
    Write32 (Obj, H->DbgSymSize);
    Write32 (Obj, H->LineInfoOffs);
    Write32 (Obj, H->LineInfoSize);
    Write32 (Obj, H->StrPoolOffs);
    Write32 (Obj, H->StrPoolSize);
    Write32 (Obj, H->AssertOffs);
    Write32 (Obj, H->AssertSize);
    Write32 (Obj, H->ScopeOffs);
    Write32 (Obj, H->ScopeSize);
}



void ObjAdd (const char* Name)
/* Add an object file to the library */
{
    struct stat StatBuf;
    const char* Module;
    ObjHeader H;
    ObjData* O;
    unsigned I;

    /* Open the object file */
    FILE* Obj = fopen (Name, "rb");
    if (Obj == 0) {
	Error ("Could not open `%s': %s", Name, strerror (errno));
    }

    /* Get the modification time of the object file. There a race condition 
     * here, since we cannot use fileno() (non standard identifier in standard
     * header file), and therefore not fstat. When using stat with the    
     * file name, there's a risk that the file was deleted and recreated
     * while it was open. Since mtime and size are only used to check
     * if a file has changed in the debugger, we will ignore this problem
     * here.
     */
    if (stat (Name, &StatBuf) != 0) {
	Error ("Cannot stat object file `%s': %s", Name, strerror (errno));
    }

    /* Read and check the header */
    ObjReadHeader (Obj, &H, Name);

    /* Make a module name from the file name */
    Module = GetModule (Name);

    /* Check if we already have a module with this name */
    O = FindObjData (Module);
    if (O == 0) {
	/* Not found, create a new entry */
	O = NewObjData ();
    } else {
	/* Found - check the file modification times of the internal copy
	 * and the external one.
	 */
	if (difftime ((time_t)O->MTime, StatBuf.st_mtime) > 0.0) {
	    Warning ("Replacing module `%s' by older version", O->Name);
	}
    }

    /* Initialize the object module data structure */
    O->Name   	  = xstrdup (Module);
    O->Flags  	  = OBJ_HAVEDATA;
    O->MTime  	  = StatBuf.st_mtime;
    O->ImportSize = H.ImportSize;
    O->Imports	  = xmalloc (O->ImportSize);
    O->ExportSize = H.ExportSize;
    O->Exports	  = xmalloc (O->ExportSize);

    /* Read imports and exports */
    fseek (Obj, H.ImportOffs, SEEK_SET);
    ReadData (Obj, O->Imports, O->ImportSize);
    fseek (Obj, H.ExportOffs, SEEK_SET);
    ReadData (Obj, O->Exports, O->ExportSize);

    /* Read the string pool */
    fseek (Obj, H.StrPoolOffs, SEEK_SET);
    O->StringCount = ReadVar (Obj);
    O->Strings     = xmalloc (O->StringCount * sizeof (char*));
    for (I = 0; I < O->StringCount; ++I) {
        O->Strings[I] = ReadStr (Obj);
    }

    /* Skip the object file header */
    O->Start = ftell (NewLib);
    fseek (NewLib, OBJ_HDR_SIZE, SEEK_CUR);

    /* Copy the remaining sections */
    fseek (Obj, H.DbgSymOffs, SEEK_SET);
    H.DbgSymOffs = LibCopyTo (Obj, H.DbgSymSize) - O->Start;
    fseek (Obj, H.OptionOffs, SEEK_SET);
    H.OptionOffs = LibCopyTo (Obj, H.OptionSize) - O->Start;
    fseek (Obj, H.SegOffs, SEEK_SET);
    H.SegOffs = LibCopyTo (Obj, H.SegSize) - O->Start;
    fseek (Obj, H.FileOffs, SEEK_SET);
    H.FileOffs = LibCopyTo (Obj, H.FileSize) - O->Start;
    fseek (Obj, H.LineInfoOffs, SEEK_SET);
    H.LineInfoOffs = LibCopyTo (Obj, H.LineInfoSize) - O->Start;
    fseek (Obj, H.AssertOffs, SEEK_SET);
    H.AssertOffs = LibCopyTo (Obj, H.AssertSize) - O->Start;
    fseek (Obj, H.ScopeOffs, SEEK_SET);
    H.ScopeOffs = LibCopyTo (Obj, H.ScopeSize) - O->Start;

    /* Calculate the amount of data written */
    O->Size = ftell (NewLib) - O->Start;

    /* Clear the remaining header fields */
    H.ImportOffs  = H.ImportSize  = 0;
    H.ExportOffs  = H.ExportSize  = 0;
    H.StrPoolOffs = H.StrPoolSize = 0;

    /* Seek back and write the updated header */
    fseek (NewLib, O->Start, SEEK_SET);
    ObjWriteHeader (NewLib, &H);

    /* Now seek again to end of file */
    fseek (NewLib, 0, SEEK_END);

    /* Done, close the file (we read it only, so no error check) */
    fclose (Obj);
}



void ObjExtract (const char* Name)
/* Extract a module from the library */
{
    unsigned long ImportStart;
    unsigned long ExportStart;
    unsigned long StrPoolStart;
    unsigned long StrPoolSize;
    struct utimbuf U;
    ObjHeader H;
    FILE* Obj;
    unsigned I;

    /* Make a module name from the file name */
    const char* Module = GetModule (Name);

    /* Try to find the module in the library */
    ObjData* O = FindObjData (Module);

    /* Bail out if the module does not exist */
    if (O == 0) {
    	Error ("Module `%s' not found in library", Module);
    }

    /* Open the output file */
    Obj = fopen (Name, "w+b");
    if (Obj == 0) {
    	Error ("Cannot open target file `%s': %s", Name, strerror (errno));
    }

    /* Copy anything to the new file that has no special handling */
    LibCopyFrom (O->Start, O->Size, Obj);

    /* Write imports and exports */
    ImportStart = ftell (Obj);
    WriteData (Obj, O->Imports, O->ImportSize);
    ExportStart = ftell (Obj);
    WriteData (Obj, O->Exports, O->ExportSize);

    /* Write the string pool */
    StrPoolStart = ftell (Obj);
    WriteVar (Obj, O->StringCount);
    for (I = 0; I < O->StringCount; ++I) {
        WriteStr (Obj, O->Strings[I]);
    }
    StrPoolSize = ftell (Obj) - StrPoolStart;

    /* Seek back and read the header */
    fseek (Obj, 0, SEEK_SET);
    ObjReadHeader (Obj, &H, Name);

    /* Update the header fields */
    H.ImportOffs  = ImportStart;
    H.ImportSize  = O->ImportSize;
    H.ExportOffs  = ExportStart;
    H.ExportSize  = O->ExportSize;
    H.StrPoolOffs = StrPoolStart;
    H.StrPoolSize = StrPoolSize;

    /* Write the changed header */
    fseek (Obj, 0, SEEK_SET);
    ObjWriteHeader (Obj, &H);

    /* Close the file */
    if (fclose (Obj) != 0) {
	Error ("Problem closing object file `%s': %s", Name, strerror (errno));
    }

    /* Set access and modification time */
    U.actime = O->MTime;
    U.modtime = O->MTime;
    if (utime (Name, &U) != 0) {
	Error ("Cannot set mod time on `%s': %s", Name, strerror (errno));
    }
}



