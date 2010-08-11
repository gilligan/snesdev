/*****************************************************************************/
/*                                                                           */
/*				    error.c				     */
/*                                                                           */
/*		    Error handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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
#include <stdlib.h>
#include <stdarg.h>

/* common */
#include "print.h"

/* cc65 */
#include "global.h"
#include "input.h"
#include "lineinfo.h"
#include "scanner.h"
#include "stmt.h"
#include "error.h"



/*****************************************************************************/
/*   	  			     Data  		     		     */
/*****************************************************************************/



/* Count of errors/warnings */
unsigned ErrorCount	= 0;
unsigned WarningCount	= 0;



/*****************************************************************************/
/*	    	       		     Code		     		     */
/*****************************************************************************/



static void IntWarning (const char* Filename, unsigned LineNo, const char* Msg, va_list ap)
/* Print warning message - internal function. */
{
    if (!IS_Get (&WarnDisable)) {
       	fprintf (stderr, "%s(%u): Warning: ", Filename, LineNo);
     	vfprintf (stderr, Msg, ap);
     	fprintf (stderr, "\n");

        if (Line) {
     	    Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
        }
     	++WarningCount;
    }
}



void Warning (const char* Format, ...)
/* Print warning message. */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetInputName (CurTok.LI), GetInputLine (CurTok.LI), Format, ap);
    va_end (ap);
}



void LIWarning (const LineInfo* LI, const char* Format, ...)
/* Print a warning message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetInputName (LI), GetInputLine (LI), Format, ap);
    va_end (ap);
}



void PPWarning (const char* Format, ...)
/* Print warning message. For use within the preprocessor. */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetCurrentFile(), GetCurrentLine(), Format, ap);
    va_end (ap);
}



static void IntError (const char* Filename, unsigned LineNo, const char* Msg, va_list ap)
/* Print an error message - internal function*/
{
    fprintf (stderr, "%s(%u): Error: ", Filename, LineNo);
    vfprintf (stderr, Msg, ap);
    fprintf (stderr, "\n");

    if (Line) {
        Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }
    ++ErrorCount;
    if (ErrorCount > 10) {
       	Fatal ("Too many errors");
    }
}



void Error (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetInputName (CurTok.LI), GetInputLine (CurTok.LI), Format, ap);
    va_end (ap);
}



void LIError (const LineInfo* LI, const char* Format, ...)
/* Print an error message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetInputName (LI), GetInputLine (LI), Format, ap);
    va_end (ap);
}



void PPError (const char* Format, ...)
/* Print an error message. For use within the preprocessor.  */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetCurrentFile(), GetCurrentLine(), Format, ap);
    va_end (ap);
}



void Fatal (const char* Format, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;

    const char* FileName;
    unsigned    LineNum;
    if (CurTok.LI) {
     	FileName = GetInputName (CurTok.LI);
     	LineNum  = GetInputLine (CurTok.LI);
    } else {
     	FileName = GetCurrentFile ();
     	LineNum  = GetCurrentLine ();
    }

    fprintf (stderr, "%s(%u): Fatal: ", FileName, LineNum);

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Line) {
        Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }
    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print a message about an internal compiler error and die. */
{
    va_list ap;

    const char* FileName;
    unsigned    LineNum;
    if (CurTok.LI) {
     	FileName = GetInputName (CurTok.LI);
     	LineNum  = GetInputLine (CurTok.LI);
    } else {
     	FileName = GetCurrentFile ();
     	LineNum  = GetCurrentLine ();
    }

    fprintf (stderr, "%s(%u): Internal compiler error:\n",
     	     FileName, LineNum);

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Line) {
        fprintf (stderr, "\nInput: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }

    /* Use abort to create a core dump */
    abort ();
}



void ErrorReport (void)
/* Report errors (called at end of compile) */
{
    Print (stdout, 1, "%u errors, %u warnings\n", ErrorCount, WarningCount);
}



