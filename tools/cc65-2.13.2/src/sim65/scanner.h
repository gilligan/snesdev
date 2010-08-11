/*****************************************************************************/
/*                                                                           */
/*				   scanner.h				     */
/*                                                                           */
/*            Configuration file scanner for the sim65 6502 simulator        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef SCANNER_H
#define SCANNER_H



/* common */
#include "attrib.h"



/*****************************************************************************/
/*     	       	       	       	     Data     				     */
/*****************************************************************************/



/* Config file tokens */
typedef enum {
    CFGTOK_NONE,
    CFGTOK_INTCON,
    CFGTOK_STRCON,
    CFGTOK_IDENT,
    CFGTOK_LCURLY,
    CFGTOK_RCURLY,
    CFGTOK_SEMI,
    CFGTOK_COMMA,
    CFGTOK_EQ,
    CFGTOK_COLON,
    CFGTOK_DOT,
    CFGTOK_DOTDOT,
    CFGTOK_EOF,

    /* Primary blocks */
    CFGTOK_CPU,
    CFGTOK_MEMORY,

    /* CPU block */
    CFGTOK_TYPE,
    CFGTOK_ADDRSPACE,

    /* Special identifiers */
    CFGTOK_TRUE,
    CFGTOK_FALSE

} cfgtok_t;



/* Mapping table entry, special identifier --> token */
typedef struct IdentTok IdentTok;
struct IdentTok {
    const char*	Ident;	     	/* Identifier */
    cfgtok_t	Tok;	     	/* Token for identifier */
};
#define ENTRY_COUNT(s) 	(sizeof (s) / sizeof (s [0]))



/* Current token and attributes */
#define CFG_MAX_IDENT_LEN  255
extern cfgtok_t		CfgTok;
extern char    	       	CfgSVal [CFG_MAX_IDENT_LEN+1];
extern unsigned long	CfgIVal;

/* Error location */
extern unsigned        	CfgErrorLine;
extern unsigned        	CfgErrorCol;



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



void CfgWarning (const char* Format, ...) attribute((format(printf,1,2)));
/* Print a warning message adding file name and line number of the config file */

void CfgError (const char* Format, ...) attribute((format(printf,1,2)));
/* Print an error message adding file name and line number of the config file */

void CfgNextTok (void);
/* Read the next token from the input stream */

void CfgConsume (cfgtok_t T, const char* Msg);
/* Skip a token, print an error message if not found */

void CfgConsumeSemi (void);
/* Consume a semicolon */

void CfgConsumeColon (void);
/* Consume a colon */

void CfgConsumeRCurly (void);
/* Consume a right curly brace */

void CfgOptionalComma (void);
/* Consume a comma if there is one */

void CfgOptionalAssign (void);
/* Consume an equal sign if there is one */

void CfgAssureInt (void);
/* Make sure the next token is an integer */

void CfgAssureStr (void);
/* Make sure the next token is a string constant */

void CfgAssureIdent (void);
/* Make sure the next token is an identifier */

void CfgRangeCheck (unsigned long Lo, unsigned long Hi);
/* Check the range of CfgIVal */

void CfgSpecialToken (const IdentTok* Table, unsigned Size, const char* Name);
/* Map an identifier to one of the special tokens in the table */

void CfgBoolToken (void);
/* Map an identifier or integer to a boolean token */

void CfgSetName (const char* Name);
/* Set a name for a config file */

const char* CfgGetName (void);
/* Get the name of the config file */

void CfgSetBuf (const char* Buf);
/* Set a memory buffer for the config */

int CfgAvail (void);
/* Return true if we have a configuration available */

void CfgOpenInput (void);
/* Open the input file if we have one */

void CfgCloseInput (void);
/* Close the input file if we have one */



/* End of scanner.h */
#endif



