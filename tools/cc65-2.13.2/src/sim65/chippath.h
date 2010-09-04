/*****************************************************************************/
/*                                                                           */
/*                                  chippath.h                               */
/*                                                                           */
/*                Chip path handling for the sim65 6502 simulator            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef CHIPPATH_H
#define CHIPPATH_H



/*****************************************************************************/
/*     	     	    		     Code   				     */
/*****************************************************************************/



void AddChipPath (const char* NewPath);
/* Add a search path for chips */

char* FindChipLib (const char* LibName);
/* Find a chip library. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */



/* End of chippath.h */

#endif



