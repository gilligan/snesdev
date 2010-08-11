/*****************************************************************************/
/*                                                                           */
/*				   litpool.h				     */
/*                                                                           */
/*		Literal string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org	                                             */
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



#ifndef LITPOOL_H
#define LITPOOL_H



#include <stdio.h>

/* common */
#include "strbuf.h"



/*****************************************************************************/
/* 				     Data				     */
/*****************************************************************************/



extern unsigned LiteralPoolLabel;		/* Pool asm label */



/*****************************************************************************/
/*		 	    	     Code				     */
/*****************************************************************************/



void InitLiteralPool (void);
/* Initialize the literal pool */

void TranslateLiteralPool (unsigned Offs);
/* Translate the literals starting from the given offset into the target
 * charset.
 */

void DumpLiteralPool (void);
/* Dump the literal pool */

unsigned GetLiteralPoolOffs (void);
/* Return the current offset into the literal pool */

void ResetLiteralPoolOffs (unsigned Offs);
/* Reset the offset into the literal pool to some earlier value, effectively
 * removing values from the pool.
 */

void AddLiteralChar (char C);
/* Add one character to the literal pool */

unsigned AddLiteral (const char* S);
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool for this string.
 */

const char* GetLiteral (unsigned Offs);
/* Get a pointer to the literal with the given offset in the pool */

void GetLiteralStrBuf (StrBuf* Target, unsigned Offs);
/* Copy the string starting at Offs and lasting to the end of the buffer
 * into Target.
 */

void PrintLiteralPoolStats (FILE* F);
/* Print statistics about the literal space used */



/* End of litpool.h */
#endif




