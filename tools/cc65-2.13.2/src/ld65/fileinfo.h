/*****************************************************************************/
/*                                                                           */
/*	      			  fileinfo.h                                 */
/*                                                                           */
/*			  Source file info structure                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#ifndef FILEINFO_H
#define FILEINFO_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "filepos.h"

/* ld65 */
#include "objdata.h"



/*****************************************************************************/
/*  	   	 		     Data                                    */
/*****************************************************************************/



typedef struct FileInfo FileInfo;
struct FileInfo {
    unsigned        Name;      	       	  /* File name index */
    unsigned long   MTime;                /* Time of last modification */
    unsigned long   Size;                 /* Size of the file */
};



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



FileInfo* ReadFileInfo (FILE* F, ObjData* O);
/* Read a file info from a file and return it */



/* End of fileinfo.h */
#endif



