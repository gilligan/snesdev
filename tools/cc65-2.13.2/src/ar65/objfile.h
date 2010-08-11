/*****************************************************************************/
/*                                                                           */
/*				   objfile.h				     */
/*                                                                           */
/*		  Object file handling for the ar65 archiver		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef OBJFILE_H
#define OBJFILE_H



#include <stdio.h>

#include "../common/objdefs.h"



/*****************************************************************************/
/*     	       	    		     Code				     */
/*****************************************************************************/



void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name);
/* Read the header of the object file checking the signature */

void ObjWriteHeader (FILE* Obj, ObjHeader* H);
/* Write the header of the object file */

void ObjAdd (const char* Name);
/* Add an object file to the library */

void ObjExtract (const char* Name);
/* Extract a module from the library */



/* End of objfile.h */

#endif



