/*****************************************************************************/
/*                                                                           */
/*				   symdefs.h   	       	       	       	     */
/*                                                                           */
/*		 Symbol definitions for the bin65 binary utils		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               R�merstra�e 52                                              */
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



/* Object file tags for imports and exports */



#ifndef SYMDEFS_H
#define SYMDEFS_H



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Number of module constructor/destructor declarations for an export */
#define EXP_CONDES_MASK	        0x07

#define IS_EXP_CONDES(x)	(((x) & EXP_CONDES_MASK) != 0)
#define GET_EXP_CONDES_COUNT(x)	((x) & EXP_CONDES_MASK)
#define INC_EXP_CONDES_COUNT(x) ((x)++)

/* Export value type */
#define EXP_CONST     	        0x00   	/* Mask bit for const values */
#define EXP_EXPR      	        0x10   	/* Mask bit for expr values */
#define EXP_MASK_VAL  	        0x10   	/* Value mask */

#define IS_EXP_CONST(x)	        (((x) & EXP_MASK_VAL) == EXP_CONST)
#define IS_EXP_EXPR(x) 	        (((x) & EXP_MASK_VAL) == EXP_EXPR)

/* Export usage */
#define EXP_EQUATE              0x00   	/* Mask bit for an equate */
#define EXP_LABEL               0x20    /* Mask bit for a label */
#define EXP_MASK_LABEL          0x20    /* Value mask */

#define IS_EXP_EQUATE(x)        (((x) & EXP_MASK_LABEL) == EXP_EQUATE)
#define IS_EXP_LABEL(x)         (((x) & EXP_MASK_LABEL) == EXP_LABEL)



/* End of symdefs.h */

#endif



