/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.h                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               R�merstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef LINEINFO_H
#define LINEINFO_H



/* common */
#include "coll.h"
#include "filepos.h"

/* ld65 */
#include "objdata.h"



/*****************************************************************************/
/*				   Forwards                                  */
/*****************************************************************************/



struct Segment;



/*****************************************************************************/
/*  	       	 		     Data                                    */
/*****************************************************************************/



typedef struct CodeRange CodeRange;
struct CodeRange {
    unsigned long Offs;
    unsigned long Size;
};



typedef struct LineInfo LineInfo;
struct LineInfo {
    struct FileInfo*    File;		      /* File struct for this line */
    FilePos             Pos;                  /* File position */
    Collection          Fragments;            /* Fragments for this line */
    Collection          CodeRanges;           /* Code ranges for this line */
};



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



LineInfo* ReadLineInfo (FILE* F, ObjData* O);
/* Read a line info from a file and return it */

void RelocLineInfo (struct Segment* S);
/* Relocate the line info for a segment. */



/* End of lineinfo.h */
#endif



