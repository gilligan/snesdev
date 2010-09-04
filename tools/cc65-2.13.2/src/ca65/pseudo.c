/*****************************************************************************/
/*                                                                           */
/*				   pseudo.c				     */
/*                                                                           */
/*		Pseudo instructions for the ca65 macroassembler		     */
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
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* common */
#include "assertdefs.h"
#include "bitops.h"
#include "cddefs.h"
#include "coll.h"
#include "symdefs.h"
#include "tgttrans.h"
#include "xmalloc.h"

/* ca65 */
#include "anonname.h"
#include "asserts.h"
#include "condasm.h"
#include "dbginfo.h"
#include "enum.h"
#include "error.h"
#include "expr.h"
#include "feature.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "listing.h"
#include "macpack.h"
#include "macro.h"
#include "nexttok.h"
#include "objcode.h"
#include "options.h"
#include "pseudo.h"
#include "repeat.h"
#include "segment.h"
#include "sizeof.h"
#include "spool.h"
#include "struct.h"
#include "symbol.h"
#include "symtab.h"



/*****************************************************************************/
/*     	      	    	   	     Data				     */
/*****************************************************************************/



/* Keyword we're about to handle */
static StrBuf Keyword = STATIC_STRBUF_INITIALIZER;

/* Segment stack */
#define MAX_PUSHED_SEGMENTS     16
static Collection SegStack = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*				 Forwards				     */
/*****************************************************************************/



static void DoUnexpected (void);
/* Got an unexpected keyword */

static void DoInvalid (void);
/* Handle a token that is invalid here, since it should have been handled on
 * a much lower level of the expression hierarchy. Getting this sort of token
 * means that the lower level code has bugs.
 * This function differs to DoUnexpected in that the latter may be triggered
 * by the user by using keywords in the wrong location. DoUnexpected is not
 * an error in the assembler itself, while DoInvalid is.
 */



/*****************************************************************************/
/*	      	    	        Helper functions			     */
/*****************************************************************************/



static unsigned char OptionalAddrSize (void)
/* If a colon follows, parse an optional address size spec and return it.
 * Otherwise return ADDR_SIZE_DEFAULT.
 */
{
    unsigned AddrSize = ADDR_SIZE_DEFAULT;
    if (Tok == TOK_COLON) {
        NextTok ();
        AddrSize = ParseAddrSize ();
        if (!ValidAddrSizeForCPU (AddrSize)) {
            /* Print an error and reset to default */
            Error ("Invalid address size specification for current CPU");
            AddrSize = ADDR_SIZE_DEFAULT;
        }
        NextTok ();
    }
    return AddrSize;
}



static void SetBoolOption (unsigned char* Flag)
/* Read a on/off/+/- option and set flag accordingly */
{
    static const char* Keys[] = {
       	"OFF",
    	"ON",
    };

    if (Tok == TOK_PLUS) {
       	*Flag = 1;
	NextTok ();
    } else if (Tok == TOK_MINUS) {
	*Flag = 0;
	NextTok ();
    } else if (Tok == TOK_IDENT) {
       	/* Map the keyword to a number */
       	switch (GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]))) {
	    case 0:    	*Flag = 0; NextTok ();		        break;
	    case 1:	*Flag = 1; NextTok ();		        break;
	    default:	ErrorSkip ("`on' or `off' expected");	break;
	}
    } else if (TokIsSep (Tok)) {
	/* Without anything assume switch on */
	*Flag = 1;
    } else {
       	ErrorSkip ("`on' or `off' expected");
    }
}



static void ExportWithAssign (SymEntry* Sym, unsigned char AddrSize, unsigned Flags)
/* Allow to assign the value of an export in an .export statement */
{
    /* The name and optional address size spec may be followed by an assignment
     * or equal token.
     */
    if (Tok == TOK_ASSIGN || Tok == TOK_EQ) {

        /* Assignment means the symbol is a label */
        if (Tok == TOK_ASSIGN) {
            Flags |= SF_LABEL;
        }

        /* Skip the assignment token */
        NextTok ();

        /* Define the symbol with the expression following the '=' */
        SymDef (Sym, Expression(), ADDR_SIZE_DEFAULT, Flags);

    }

    /* Now export the symbol */
    SymExport (Sym, AddrSize, Flags);
}



static void ExportImport (void (*Func) (SymEntry*, unsigned char, unsigned),
                          unsigned char DefAddrSize, unsigned Flags)
/* Export or import symbols */
{
    SymEntry* Sym;
    unsigned char AddrSize;

    while (1) {

        /* We need an identifier here */
     	if (Tok != TOK_IDENT) {
       	    ErrorSkip ("Identifier expected");
     	    return;
     	}

        /* Find the symbol table entry, allocate a new one if necessary */
        Sym = SymFind (CurrentScope, &SVal, SYM_ALLOC_NEW);

        /* Skip the name */
        NextTok ();

        /* Get an optional address size */
        AddrSize = OptionalAddrSize ();
        if (AddrSize == ADDR_SIZE_DEFAULT) {
            AddrSize = DefAddrSize;
        }

        /* Call the actual import/export function */
        Func (Sym, AddrSize, Flags);

        /* More symbols? */
     	if (Tok == TOK_COMMA) {
     	    NextTok ();
     	} else {
     	    break;
     	}
    }
}



static long IntArg (long Min, long Max)
/* Read an integer argument and check a range. Accept the token "unlimited"
 * and return -1 in this case.
 */
{
    if (Tok == TOK_IDENT && SB_CompareStr (&SVal, "unlimited") == 0) {
	NextTok ();
	return -1;
    } else {
	long Val = ConstExpression ();
	if (Val < Min || Val > Max) {
	    Error ("Range error");
	    Val = Min;
	}
	return Val;
    }
}



static void ConDes (const StrBuf* Name, unsigned Type)
/* Parse remaining line for constructor/destructor of the remaining type */
{
    long Prio;


    /* Find the symbol table entry, allocate a new one if necessary */
    SymEntry* Sym = SymFind (CurrentScope, Name, SYM_ALLOC_NEW);

    /* Optional constructor priority */
    if (Tok == TOK_COMMA) {
    	/* Priority value follows */
    	NextTok ();
    	Prio = ConstExpression ();
    	if (Prio < CD_PRIO_MIN || Prio > CD_PRIO_MAX) {
    	    /* Value out of range */
    	    Error ("Range error");
    	    return;
    	}
    } else {
    	/* Use the default priority value */
    	Prio = CD_PRIO_DEF;
    }

    /* Define the symbol */
    SymConDes (Sym, ADDR_SIZE_DEFAULT, Type, (unsigned) Prio);
}



/*****************************************************************************/
/*	      	    	       Handler functions			     */
/*****************************************************************************/



static void DoA16 (void)
/* Switch the accu to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error ("Command is only valid in 65816 mode");
    } else {
       	/* Immidiate mode has two extension bytes */
	ExtBytes [AM65I_IMM_ACCU] = 2;
    }
}



static void DoA8 (void)
/* Switch the accu to 8 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error ("Command is only valid in 65816 mode");
    } else {
	/* Immidiate mode has one extension byte */
	ExtBytes [AM65I_IMM_ACCU] = 1;
    }
}



static void DoAddr (void)
/* Define addresses */
{
    while (1) {
	if (GetCPU() == CPU_65816) {
       	    EmitWord (GenWordExpr (Expression ()));
	} else {
	    /* Do a range check */
	    EmitWord (Expression ());
       	}
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoAlign (void)
/* Align the PC to some boundary */
{
    long Val;
    long Align;
    unsigned Bit;

    /* Read the alignment value */
    Align = ConstExpression ();
    if (Align <= 0 || Align > 0x10000) {
       	ErrorSkip ("Range error");
      	return;
    }

    /* Optional value follows */
    if (Tok == TOK_COMMA) {
	NextTok ();
	Val = ConstExpression ();
	/* We need a byte value here */
	if (!IsByteRange (Val)) {
       	    ErrorSkip ("Range error");
	    return;
	}
    } else {
	Val = -1;
    }

    /* Check if the alignment is a power of two */
    Bit = BitFind (Align);
    if (Align != (0x01L << Bit)) {
	Error ("Alignment value must be a power of 2");
    } else {
	SegAlign (Bit, (int) Val);
    }
}



static void DoASCIIZ (void)
/* Define text with a zero terminator */
{
    while (1) {
	/* Must have a string constant */
	if (Tok != TOK_STRCON) {
       	    ErrorSkip ("String constant expected");
	    return;
	}

	/* Translate into target charset and emit */
       	TgtTranslateStrBuf (&SVal);
       	EmitStrBuf (&SVal);
	NextTok ();
	if (Tok == TOK_COMMA) {
	    NextTok ();
	} else {
	    break;
	}
    }
    Emit0 (0);
}



static void DoAssert (void)
/* Add an assertion */
{
    static const char* ActionTab [] = {
	"WARN", "WARNING",
        "ERROR"
    };

    int      Action;
    unsigned Msg;

    /* First we have the expression that has to evaluated */
    ExprNode* Expr = Expression ();
    ConsumeComma ();

    /* Action follows */
    if (Tok != TOK_IDENT) {
        ErrorSkip ("Identifier expected");
        return;
    }
    Action = GetSubKey (ActionTab, sizeof (ActionTab) / sizeof (ActionTab[0]));
    switch (Action) {

        case 0:
        case 1:
            /* Warning */
            Action = ASSERT_ACT_WARN;
            break;

        case 2:
            /* Error */
            Action = ASSERT_ACT_ERROR;
            break;

        default:
            Error ("Illegal assert action specifier");
    }
    NextTok ();

    /* We can have an optional message. If no message is present, use
     * "Assertion failed".
     */
    if (Tok == TOK_COMMA) {

        /* Skip the comma */
        NextTok ();

        /* Read the message */
        if (Tok != TOK_STRCON) {
            ErrorSkip ("String constant expected");
            return;
        }

        /* Translate the message into a string id. We can then skip the input
         * string.
         */
        Msg = GetStrBufId (&SVal);
        NextTok ();

    } else {

        /* Use "Assertion failed" */
        Msg = GetStringId ("Assertion failed");

    }

    /* Remember the assertion */
    AddAssertion (Expr, Action, Msg);
}



static void DoAutoImport (void)
/* Mark unresolved symbols as imported */
{
    SetBoolOption (&AutoImport);
}


static void DoBankBytes (void)
/* Define bytes, extracting the bank byte from each expression in the list */
{
    while (1) {
        EmitByte (FuncBankByte ());
        if (Tok != TOK_COMMA) {
            break;
        } else {
            NextTok ();
        }
    }
}



static void DoBss (void)
/* Switch to the BSS segment */
{
    UseSeg (&BssSegDef);
}



static void DoByte (void)
/* Define bytes */
{
    while (1) {
	if (Tok == TOK_STRCON) {
	    /* A string, translate into target charset and emit */
       	    TgtTranslateStrBuf (&SVal);
	    EmitStrBuf (&SVal);
	    NextTok ();
	} else {
	    EmitByte (Expression ());
	}
    	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	    /* Do smart handling of dangling comma */
	    if (Tok == TOK_SEP) {
	     	Error ("Unexpected end of line");
	 	break;
	    }
	}
    }
}



static void DoCase (void)
/* Switch the IgnoreCase option */
{
    SetBoolOption (&IgnoreCase);
    IgnoreCase = !IgnoreCase;
}



static void DoCharMap (void)
/* Allow custome character mappings */
{
    long Index;
    long Code;

    /* Read the index as numerical value */
    Index = ConstExpression ();
    if (Index < 0 || Index > 255) {
      	/* Value out of range */
       	ErrorSkip ("Range error");
      	return;
    }

    /* Comma follows */
    ConsumeComma ();

    /* Read the character code */
    Code = ConstExpression ();
    if (Code < 0 || Code > 255) {
    	/* Value out of range */
       	ErrorSkip ("Range error");
    	return;
    }

    /* Set the character translation */
    TgtTranslateSet ((unsigned) Index, (unsigned char) Code);
}



static void DoCode (void)
/* Switch to the code segment */
{
    UseSeg (&CodeSegDef);
}



static void DoConDes (void)
/* Export a symbol as constructor/destructor */
{
    static const char* Keys[] = {
       	"CONSTRUCTOR",
	"DESTRUCTOR",
        "INTERRUPTOR",
    };
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    long Type;

    /* Symbol name follows */
    if (Tok != TOK_IDENT) {
    	ErrorSkip ("Identifier expected");
    	return;
    }
    SB_Copy (&Name, &SVal);
    NextTok ();

    /* Type follows. May be encoded as identifier or numerical */
    ConsumeComma ();
    if (Tok == TOK_IDENT) {

	/* Map the following keyword to a number, then skip it */
	Type = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
	NextTok ();

	/* Check if we got a valid keyword */
	if (Type < 0) {
       	    ErrorSkip ("Syntax error");
	    goto ExitPoint;
	}

    } else {

	/* Read the type as numerical value */
       	Type = ConstExpression ();
    	if (Type < CD_TYPE_MIN || Type > CD_TYPE_MAX) {
    	    /* Value out of range */
       	    ErrorSkip ("Range error");
    	    goto ExitPoint;
    	}

    }

    /* Parse the remainder of the line and export the symbol */
    ConDes (&Name, (unsigned) Type);

ExitPoint:
    /* Free string memory */
    SB_Done (&Name);
}



static void DoConstructor (void)
/* Export a symbol as constructor */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;

    /* Symbol name follows */
    if (Tok != TOK_IDENT) {
    	ErrorSkip ("Identifier expected");
    	return;
    }
    SB_Copy (&Name, &SVal);
    NextTok ();

    /* Parse the remainder of the line and export the symbol */
    ConDes (&Name, CD_TYPE_CON);

    /* Free string memory */
    SB_Done (&Name);
}



static void DoData (void)
/* Switch to the data segment */
{
    UseSeg (&DataSegDef);
}



static void DoDbg (void)
/* Add debug information from high level code */
{
    static const char* Keys[] = {
       	"FILE",
	"LINE",
    	"SYM",
    };
    int Key;


    /* We expect a subkey */
    if (Tok != TOK_IDENT) {
     	ErrorSkip ("Identifier expected");
     	return;
    }

    /* Map the following keyword to a number */
    Key = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));

    /* Skip the subkey */
    NextTok ();

    /* Check the key and dispatch to a handler */
    switch (Key) {
	case 0:     DbgInfoFile ();		break;
	case 1:	    DbgInfoLine ();		break;
	case 2:	    DbgInfoSym ();		break;
	default:    ErrorSkip ("Syntax error"); break;
    }
}



static void DoDByt (void)
/* Output double bytes */
{
    while (1) {
	EmitWord (GenSwapExpr (Expression ()));
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoDebugInfo (void)
/* Switch debug info on or off */
{
    SetBoolOption (&DbgSyms);
}



static void DoDefine (void)
/* Define a one line macro */
{
    MacDef (MAC_STYLE_DEFINE);
}



static void DoDestructor (void)
/* Export a symbol as destructor */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;

    /* Symbol name follows */
    if (Tok != TOK_IDENT) {
    	ErrorSkip ("Identifier expected");
    	return;
    }
    SB_Copy (&Name, &SVal);
    NextTok ();

    /* Parse the remainder of the line and export the symbol */
    ConDes (&Name, CD_TYPE_DES);

    /* Free string memory */
    SB_Done (&Name);
}



static void DoDWord (void)
/* Define dwords */
{
    while (1) {
       	EmitDWord (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoEnd (void)
/* End of assembly */
{
    ForcedEnd = 1;
    NextTok ();
}



static void DoEndProc (void)
/* Leave a lexical level */
{
    if (GetCurrentSymTabType () != ST_PROC) {
        /* No local scope */
        ErrorSkip ("No open .PROC");
    } else {
        SymLeaveLevel ();
    }
}



static void DoEndScope (void)
/* Leave a lexical level */
{
    if ( GetCurrentSymTabType () != ST_SCOPE) {
        /* No local scope */
        ErrorSkip ("No open .SCOPE");
    } else {
        SymLeaveLevel ();
    }
}



static void DoError (void)
/* User error */
{
    if (Tok != TOK_STRCON) {
 	ErrorSkip ("String constant expected");
    } else {
       	Error ("User error: %m%p", &SVal);
	SkipUntilSep ();
    }
}



static void DoExitMacro (void)
/* Exit a macro expansion */
{
    if (!InMacExpansion ()) {
	/* We aren't expanding a macro currently */
       	DoUnexpected ();
    } else {
	MacAbort ();
    }
}



static void DoExport (void)
/* Export a symbol */
{
    ExportImport (ExportWithAssign, ADDR_SIZE_DEFAULT, SF_NONE);
}



static void DoExportZP (void)
/* Export a zeropage symbol */
{
    ExportImport (ExportWithAssign, ADDR_SIZE_ZP, SF_NONE);
}



static void DoFarAddr (void)
/* Define far addresses (24 bit) */
{
    while (1) {
       	EmitFarAddr (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoFeature (void)
/* Switch the Feature option */
{
    /* Allow a list of comma separated keywords */
    while (1) {

     	/* We expect an identifier */
     	if (Tok != TOK_IDENT) {
     	    ErrorSkip ("Identifier expected");
     	    return;
     	}

	/* Make the string attribute lower case */
	LocaseSVal ();

     	/* Set the feature and check for errors */
	if (SetFeature (&SVal) == FEAT_UNKNOWN) {
     	    /* Not found */
     	    ErrorSkip ("Invalid feature: `%m%p'", &SVal);
     	    return;
     	} else {
	    /* Skip the keyword */
	    NextTok ();
	}

     	/* Allow more than one keyword */
     	if (Tok == TOK_COMMA) {
     	    NextTok ();
     	} else {
     	    break;
     	}
    }
}



static void DoFileOpt (void)
/* Insert a file option */
{
    long OptNum;

    /* The option type may be given as a keyword or as a number. */
    if (Tok == TOK_IDENT) {

	/* Option given as keyword */
	static const char* Keys [] = {
	    "AUTHOR", "COMMENT", "COMPILER"
      	};

	/* Map the option to a number */
	OptNum = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
	if (OptNum < 0) {
	    /* Not found */
	    ErrorSkip ("File option keyword expected");
	    return;
	}

	/* Skip the keyword */
	NextTok ();

	/* Must be followed by a comma */
	ConsumeComma ();

	/* We accept only string options for now */
	if (Tok != TOK_STRCON) {
	    ErrorSkip ("String constant expected");
	    return;
	}

       	/* Insert the option */
	switch (OptNum) {

	    case 0:
		/* Author */
		OptAuthor (&SVal);
		break;

	    case 1:
		/* Comment */
		OptComment (&SVal);
		break;

	    case 2:
		/* Compiler */
		OptCompiler (&SVal);
		break;

	    default:
	        Internal ("Invalid OptNum: %ld", OptNum);

	}

	/* Done */
	NextTok ();

    } else {

     	/* Option given as number */
       	OptNum = ConstExpression ();
     	if (!IsByteRange (OptNum)) {
     	    ErrorSkip ("Range error");
     	    return;
     	}

	/* Must be followed by a comma */
	ConsumeComma ();

	/* We accept only string options for now */
	if (Tok != TOK_STRCON) {
	    ErrorSkip ("String constant expected");
	    return;
	}

	/* Insert the option */
	OptStr ((unsigned char) OptNum, &SVal);

	/* Done */
	NextTok ();
    }
}



static void DoForceImport (void)
/* Do a forced import on a symbol */
{
    ExportImport (SymImport, ADDR_SIZE_DEFAULT, SF_FORCED);
}



static void DoGlobal (void)
/* Declare a global symbol */
{
    ExportImport (SymGlobal, ADDR_SIZE_DEFAULT, SF_NONE);
}



static void DoGlobalZP (void)
/* Declare a global zeropage symbol */
{
    ExportImport (SymGlobal, ADDR_SIZE_ZP, SF_NONE);
}


static void DoHiBytes (void)
/* Define bytes, extracting the hi byte from each expression in the list */
{
    while (1) {
        EmitByte (FuncHiByte ());
        if (Tok != TOK_COMMA) {
            break;
        } else {
            NextTok ();
        }
    }
}



static void DoI16 (void)
/* Switch the index registers to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
     	Error ("Command is only valid in 65816 mode");
    } else {
       	/* Immidiate mode has two extension bytes */
     	ExtBytes [AM65I_IMM_INDEX] = 2;
    }
}



static void DoI8 (void)
/* Switch the index registers to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error ("Command is only valid in 65816 mode");
    } else {
	/* Immidiate mode has one extension byte */
	ExtBytes [AM65I_IMM_INDEX] = 1;
    }
}



static void DoImport (void)
/* Import a symbol */
{
    ExportImport (SymImport, ADDR_SIZE_DEFAULT, SF_NONE);
}



static void DoImportZP (void)
/* Import a zero page symbol */
{
    ExportImport (SymImport, ADDR_SIZE_ZP, SF_NONE);
}



static void DoIncBin (void)
/* Include a binary file */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    long Start = 0L;
    long Count = -1L;
    long Size;
    FILE* F;

    /* Name must follow */
    if (Tok != TOK_STRCON) {
    	ErrorSkip ("String constant expected");
    	return;
    }
    SB_Copy (&Name, &SVal);
    SB_Terminate (&Name);
    NextTok ();

    /* A starting offset may follow */
    if (Tok == TOK_COMMA) {
    	NextTok ();
    	Start = ConstExpression ();

    	/* And a length may follow */
    	if (Tok == TOK_COMMA) {
    	    NextTok ();
    	    Count = ConstExpression ();
    	}

    }

    /* Try to open the file */
    F = fopen (SB_GetConstBuf (&Name), "rb");
    if (F == 0) {

       	/* Search for the file in the include directories. */
     	char* PathName = FindInclude (SB_GetConstBuf (&Name));
       	if (PathName == 0 || (F = fopen (PathName, "rb")) == 0) {
     	    /* Not found or cannot open, print an error and bail out */
       	    ErrorSkip ("Cannot open include file `%m%p': %s", &Name, strerror (errno));
     	}

     	/* Free the allocated memory */
     	xfree (PathName);

        /* If we had an error before, bail out now */
        if (F == 0) {
            goto ExitPoint;
        }
    }

    /* Get the size of the file */
    fseek (F, 0, SEEK_END);
    Size = ftell (F);

    /* If a count was not given, calculate it now */
    if (Count < 0) {
	Count = Size - Start;
	if (Count < 0) {
	    /* Nothing to read - flag this as a range error */
	    ErrorSkip ("Range error");
	    goto Done;
	}
    } else {
	/* Count was given, check if it is valid */
	if (Start + Count > Size) {
	    ErrorSkip ("Range error");
	    goto Done;
	}
    }

    /* Seek to the start position */
    fseek (F, Start, SEEK_SET);

    /* Read chunks and insert them into the output */
    while (Count > 0) {

	unsigned char Buf [1024];

	/* Calculate the number of bytes to read */
       	size_t BytesToRead = (Count > (long)sizeof(Buf))? sizeof(Buf) : (size_t) Count;

	/* Read chunk */
	size_t BytesRead = fread (Buf, 1, BytesToRead, F);
	if (BytesToRead != BytesRead) {
	    /* Some sort of error */
	    ErrorSkip ("Cannot read from include file `%m%p': %s",
                       &Name, strerror (errno));
	    break;
	}

	/* Insert it into the output */
	EmitData (Buf, BytesRead);

	/* Keep the counters current */
	Count -= BytesRead;
    }

Done:
    /* Close the file, ignore errors since it's r/o */
    (void) fclose (F);

ExitPoint:
    /* Free string memory */
    SB_Done (&Name);
}



static void DoInclude (void)
/* Include another file */
{
    /* Name must follow */
    if (Tok != TOK_STRCON) {
	ErrorSkip ("String constant expected");
    } else {
        SB_Terminate (&SVal);
    	if (NewInputFile (SB_GetConstBuf (&SVal)) == 0) {
            /* Error opening the file, skip remainder of line */
            SkipUntilSep ();
        }
    }
}



static void DoInterruptor (void)
/* Export a symbol as interruptor */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;

    /* Symbol name follows */
    if (Tok != TOK_IDENT) {
    	ErrorSkip ("Identifier expected");
    	return;
    }
    SB_Copy (&Name, &SVal);
    NextTok ();

    /* Parse the remainder of the line and export the symbol */
    ConDes (&Name, CD_TYPE_INT);

    /* Free string memory */
    SB_Done (&Name);
}



static void DoInvalid (void)
/* Handle a token that is invalid here, since it should have been handled on
 * a much lower level of the expression hierarchy. Getting this sort of token
 * means that the lower level code has bugs.
 * This function differs to DoUnexpected in that the latter may be triggered
 * by the user by using keywords in the wrong location. DoUnexpected is not
 * an error in the assembler itself, while DoInvalid is.
 */
{
    Internal ("Unexpected token: %m%p", &Keyword);
}



static void DoLineCont (void)
/* Switch the use of line continuations */
{
    SetBoolOption (&LineCont);
}



static void DoList (void)
/* Enable/disable the listing */
{
    /* Get the setting */
    unsigned char List;
    SetBoolOption (&List);

    /* Manage the counter */
    if (List) {
	EnableListing ();
    } else {
	DisableListing ();
    }
}



static void DoLoBytes (void)
/* Define bytes, extracting the lo byte from each expression in the list */
{
    while (1) {
        EmitByte (FuncLoByte ());
        if (Tok != TOK_COMMA) {
            break;
        } else {
            NextTok ();
        }
    }
}


static void DoListBytes (void)
/* Set maximum number of bytes to list for one line */
{
    SetListBytes (IntArg (MIN_LIST_BYTES, MAX_LIST_BYTES));
}



static void DoLocalChar (void)
/* Define the character that starts local labels */
{
    if (Tok != TOK_CHARCON) {
     	ErrorSkip ("Character constant expected");
    } else {
	if (IVal != '@' && IVal != '?') {
	    Error ("Invalid start character for locals");
	} else {
     	    LocalStart = (char) IVal;
       	}
     	NextTok ();
    }
}



static void DoMacPack (void)
/* Insert a macro package */
{
    int Package;

    /* We expect an identifier */
    if (Tok != TOK_IDENT) {
    	ErrorSkip ("Identifier expected");
    	return;
    }

    /* Search for the macro package name */
    LocaseSVal ();
    Package = MacPackFind (&SVal);
    if (Package < 0) {
    	/* Not found */
    	ErrorSkip ("Invalid macro package");
    	return;
    }

    /* Insert the package. If this fails, skip the remainder of the line to
     * avoid additional error messages.
     */
    if (MacPackInsert (Package) == 0) {
        SkipUntilSep ();
    }
}



static void DoMacro (void)
/* Start a macro definition */
{
    MacDef (MAC_STYLE_CLASSIC);
}



static void DoNull (void)
/* Switch to the NULL segment */
{
    UseSeg (&NullSegDef);
}



static void DoOrg (void)
/* Start absolute code */
{
    long PC = ConstExpression ();
    if (PC < 0 || PC > 0xFFFFFF) {
	Error ("Range error");
    	return;
    }
    EnterAbsoluteMode (PC);
}



static void DoOut (void)
/* Output a string */
{
    if (Tok != TOK_STRCON) {
	ErrorSkip ("String constant expected");
    } else {
	/* Output the string and be sure to flush the output to keep it in
	 * sync with any error messages if the output is redirected to a file.
	 */
	printf ("%.*s\n", (int) SB_GetLen (&SVal), SB_GetConstBuf (&SVal));
	fflush (stdout);
	NextTok ();
    }
}



static void DoP02 (void)
/* Switch to 6502 CPU */
{
    SetCPU (CPU_6502);
}



static void DoPC02 (void)
/* Switch to 65C02 CPU */
{
    SetCPU (CPU_65C02);
}



static void DoP816 (void)
/* Switch to 65816 CPU */
{
    SetCPU (CPU_65816);
}



static void DoPageLength (void)
/* Set the page length for the listing */
{
    PageLength = IntArg (MIN_PAGE_LEN, MAX_PAGE_LEN);
}



static void DoPopSeg (void)
/* Pop an old segment from the segment stack */
{
    SegDef* Def;

    /* Must have a segment on the stack */
    if (CollCount (&SegStack) == 0) {
        ErrorSkip ("Segment stack is empty");
        return;
    }

    /* Pop the last element */
    Def = CollPop (&SegStack);

    /* Restore this segment */
    UseSeg (Def);

    /* Delete the segment definition */
    FreeSegDef (Def);
}



static void DoProc (void)
/* Start a new lexical scope */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    unsigned char AddrSize;

    if (Tok == TOK_IDENT) {

        SymEntry* Sym;

	/* The new scope has a name. Remember it. */
        SB_Copy (&Name, &SVal);

        /* Search for the symbol, generate a new one if needed */
       	Sym = SymFind (CurrentScope, &Name, SYM_ALLOC_NEW);

        /* Skip the scope name */
        NextTok ();

        /* Read an optional address size specifier */
        AddrSize = OptionalAddrSize ();

        /* Mark the symbol as defined */
    	SymDef (Sym, GenCurrentPC (), AddrSize, SF_LABEL);

    } else {

        /* A .PROC statement without a name */
        Warning (1, "Unnamed .PROCs are deprecated, please use .SCOPE");
        AnonName (&Name, "PROC");
        AddrSize = ADDR_SIZE_DEFAULT;

    }

    /* Enter a new scope */
    SymEnterLevel (&Name, ST_PROC, AddrSize);

    /* Free memory for Name */
    SB_Done (&Name);
}



static void DoPSC02 (void)
/* Switch to 65SC02 CPU */
{
    SetCPU (CPU_65SC02);
}



static void DoPushSeg (void)
/* Push the current segment onto the segment stack */
{
    /* Can only push a limited size of segments */
    if (CollCount (&SegStack) >= MAX_PUSHED_SEGMENTS) {
        ErrorSkip ("Segment stack overflow");
        return;
    }

    /* Get the current segment and push it */
    CollAppend (&SegStack, DupSegDef (GetCurrentSegDef ()));
}



static void DoReloc (void)
/* Enter relocatable mode */
{
    EnterRelocMode ();
}



static void DoRepeat (void)
/* Repeat some instruction block */
{
    ParseRepeat ();
}



static void DoRes (void)
/* Reserve some number of storage bytes */
{
    long Count;
    long Val;

    Count = ConstExpression ();
    if (Count > 0xFFFF || Count < 0) {
	ErrorSkip ("Range error");
	return;
    }
    if (Tok == TOK_COMMA) {
	NextTok ();
	Val = ConstExpression ();
	/* We need a byte value here */
	if (!IsByteRange (Val)) {
       	    ErrorSkip ("Range error");
	    return;
	}

	/* Emit constant values */
	while (Count--) {
	    Emit0 ((unsigned char) Val);
	}

    } else {
	/* Emit fill fragments */
	EmitFill (Count);
    }
}



static void DoROData (void)
/* Switch to the r/o data segment */
{
    UseSeg (&RODataSegDef);
}



static void DoScope (void)
/* Start a local scope */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    unsigned char AddrSize;


    if (Tok == TOK_IDENT) {

    	/* The new scope has a name. Remember and skip it. */
        SB_Copy (&Name, &SVal);
        NextTok ();

    } else {

        /* An unnamed scope */
        AnonName (&Name, "SCOPE");

    }

    /* Read an optional address size specifier */
    AddrSize = OptionalAddrSize ();

    /* Enter the new scope */
    SymEnterLevel (&Name, ST_SCOPE, AddrSize);

    /* Free memory for Name */
    SB_Done (&Name);
}



static void DoSegment (void)
/* Switch to another segment */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    SegDef Def;

    if (Tok != TOK_STRCON) {
	ErrorSkip ("String constant expected");
    } else {

	/* Save the name of the segment and skip it */
	SB_Copy (&Name, &SVal);
	NextTok ();

        /* Use the name for the segment definition */
        SB_Terminate (&Name);
        Def.Name = SB_GetBuf (&Name);

    	/* Check for an optional address size modifier */
        Def.AddrSize = OptionalAddrSize ();

	/* Set the segment */
     	UseSeg (&Def);
    }

    /* Free memory for Name */
    SB_Done (&Name);
}



static void DoSetCPU (void)
/* Switch the CPU instruction set */
{
    /* We expect an identifier */
    if (Tok != TOK_STRCON) {
	ErrorSkip ("String constant expected");
    } else {
        cpu_t CPU;

        /* Try to find the CPU */
        SB_Terminate (&SVal);
        CPU = FindCPU (SB_GetConstBuf (&SVal));

        /* Switch to the new CPU */
        SetCPU (CPU);

        /* Skip the identifier. If the CPU switch was successful, the scanner
         * will treat the input now correctly for the new CPU.
         */
        NextTok ();
    }
}



static void DoSmart (void)
/* Smart mode on/off */
{
    SetBoolOption (&SmartMode);
}



static void DoSunPlus (void)
/* Switch to the SUNPLUS CPU */
{
    SetCPU (CPU_SUNPLUS);
}



static void DoTag (void)
/* Allocate space for a struct */
{
    SymEntry* SizeSym;
    long Size;

    /* Read the struct name */
    SymTable* Struct = ParseScopedSymTable ();

    /* Check the supposed struct */
    if (Struct == 0) {
        ErrorSkip ("Unknown struct");
        return;
    }
    if (GetSymTabType (Struct) != ST_STRUCT) {
        ErrorSkip ("Not a struct");
        return;
    }

    /* Get the symbol that defines the size of the struct */
    SizeSym = GetSizeOfScope (Struct);

    /* Check if it does exist and if its value is known */
    if (SizeSym == 0 || !SymIsConst (SizeSym, &Size)) {
        ErrorSkip ("Size of struct/union is unknown");
        return;
    }

    /* Optional multiplicator may follow */
    if (Tok == TOK_COMMA) {
        long Multiplicator;
        NextTok ();
        Multiplicator = ConstExpression ();
        /* Multiplicator must make sense */
        if (Multiplicator <= 0) {
            ErrorSkip ("Range error");
            return;
        }
        Size *= Multiplicator;
    }

    /* Emit fill fragments */
    EmitFill (Size);
}



static void DoUnexpected (void)
/* Got an unexpected keyword */
{
    Error ("Unexpected `%m%p'", &Keyword);
    SkipUntilSep ();
}



static void DoWarning (void)
/* User warning */
{
    if (Tok != TOK_STRCON) {
 	ErrorSkip ("String constant expected");
    } else {
       	Warning (0, "User warning: %m%p", &SVal);
	SkipUntilSep ();
    }
}



static void DoWord (void)
/* Define words */
{
    while (1) {
       	EmitWord (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoZeropage (void)
/* Switch to the zeropage segment */
{
    UseSeg (&ZeropageSegDef);
}



/*****************************************************************************/
/*	      		   	  Table data				     */
/*****************************************************************************/



/* Control commands flags */
enum {
    ccNone	= 0x0000,		/* No special flags */
    ccKeepToken	= 0x0001		/* Do not skip the current token */
};

/* Control command table */
typedef struct CtrlDesc CtrlDesc;
struct CtrlDesc {
    unsigned   	Flags; 			/* Flags for this directive */
    void       	(*Handler) (void);	/* Command handler */
};

#define PSEUDO_COUNT 	(sizeof (CtrlCmdTab) / sizeof (CtrlCmdTab [0]))
static CtrlDesc CtrlCmdTab [] = {
    { ccNone,		DoA16		},
    { ccNone,       	DoA8		},
    { ccNone,        	DoAddr 	       	},     	/* .ADDR */
    { ccNone,       	DoAlign		},
    { ccNone,       	DoASCIIZ	},
    { ccNone,           DoAssert        },
    { ccNone,       	DoAutoImport	},
    { ccNone,        	DoUnexpected	},	/* .BANKBYTE */
    { ccNone,           DoBankBytes     },
    { ccNone,        	DoUnexpected	},	/* .BLANK */
    { ccNone,       	DoBss		},
    { ccNone,       	DoByte		},
    { ccNone,       	DoCase		},
    { ccNone,  	       	DoCharMap       },
    { ccNone,		DoCode		},
    { ccNone,		DoUnexpected,	},	/* .CONCAT */
    { ccNone,		DoConDes	},
    { ccNone,		DoUnexpected	},	/* .CONST */
    { ccNone,		DoConstructor	},
    { ccNone,		DoUnexpected	},	/* .CPU */
    { ccNone,		DoData		},
    { ccNone,		DoDbg,		},
    { ccNone,		DoDByt		},
    { ccNone,        	DoDebugInfo	},
    { ccNone,		DoDefine	},
    { ccNone,		DoUnexpected	},	/* .DEFINED */
    { ccNone,		DoDestructor	},
    { ccNone,		DoDWord		},
    { ccKeepToken,	DoConditionals	},	/* .ELSE */
    { ccKeepToken,	DoConditionals	},	/* .ELSEIF */
    { ccKeepToken,     	DoEnd	  	},
    { ccNone,           DoUnexpected    },      /* .ENDENUM */
    { ccKeepToken,    	DoConditionals	},	/* .ENDIF */
    { ccNone,     	DoUnexpected	},	/* .ENDMACRO */
    { ccNone,		DoEndProc	},
    { ccNone,		DoUnexpected	},	/* .ENDREPEAT */
    { ccNone,           DoEndScope      },
    { ccNone,           DoUnexpected    },      /* .ENDSTRUCT */
    { ccNone,           DoUnexpected    },      /* .ENDUNION */
    { ccNone,           DoEnum          },
    { ccNone,		DoError	  	},
    { ccNone,		DoExitMacro	},
    { ccNone,		DoExport  	},
    { ccNone,       	DoExportZP	},
    { ccNone,        	DoFarAddr	},
    { ccNone,		DoFeature	},
    { ccNone,		DoFileOpt	},
    { ccNone,           DoForceImport   },
    { ccNone,		DoUnexpected	},	/* .FORCEWORD */
    { ccNone,		DoGlobal	},
    { ccNone,		DoGlobalZP	},
    { ccNone,        	DoUnexpected	},	/* .HIBYTE */
    { ccNone,           DoHiBytes       },
    { ccNone,        	DoUnexpected	},	/* .HIWORD */
    { ccNone,		DoI16	  	},
    { ccNone,		DoI8	  	},
    { ccNone,           DoUnexpected    },      /* .IDENT */
    { ccKeepToken,	DoConditionals	},	/* .IF */
    { ccKeepToken,	DoConditionals	},	/* .IFBLANK */
    { ccKeepToken,	DoConditionals	},	/* .IFCONST */
    { ccKeepToken,	DoConditionals	},	/* .IFDEF */
    { ccKeepToken,     	DoConditionals	},	/* .IFNBLANK */
    { ccKeepToken,	DoConditionals	},	/* .IFNCONST */
    { ccKeepToken,	DoConditionals	},	/* .IFNDEF */
    { ccKeepToken,	DoConditionals	},	/* .IFNREF */
    { ccKeepToken,	DoConditionals	},	/* .IFP02 */
    { ccKeepToken,	DoConditionals	},	/* .IFP816 */
    { ccKeepToken,	DoConditionals	},	/* .IFPC02 */
    { ccKeepToken,	DoConditionals	},	/* .IFPSC02 */
    { ccKeepToken,	DoConditionals	},	/* .IFREF */
    { ccNone,		DoImport  	},
    { ccNone,		DoImportZP	},
    { ccNone,		DoIncBin	},
    { ccNone,      	DoInclude	},
    { ccNone,           DoInterruptor   },
    { ccNone,		DoInvalid	},	/* .LEFT */
    { ccNone,		DoLineCont	},
    { ccNone,		DoList		},
    { ccNone,  	       	DoListBytes	},
    { ccNone,        	DoUnexpected	},	/* .LOBYTE */
    { ccNone,           DoLoBytes       },
    { ccNone,		DoUnexpected	},	/* .LOCAL */
    { ccNone,		DoLocalChar	},
    { ccNone,        	DoUnexpected	},	/* .LOWORD */
    { ccNone,		DoMacPack	},
    { ccNone,		DoMacro		},
    { ccNone,  	       	DoUnexpected	},	/* .MATCH */
    { ccNone,  	       	DoInvalid	},	/* .MID	*/
    { ccNone,		DoNull		},
    { ccNone,		DoOrg		},
    { ccNone,		DoOut		},
    { ccNone,		DoP02		},
    { ccNone,		DoP816		},
    { ccNone,  	       	DoPageLength	},
    { ccNone,       	DoUnexpected   	},   	/* .PARAMCOUNT */
    { ccNone,		DoPC02		},
    { ccNone,           DoPopSeg        },
    { ccNone,		DoProc		},
    { ccNone,  	       	DoPSC02		},
    { ccNone,           DoPushSeg       },
    { ccNone,    	DoUnexpected   	},   	/* .REFERENCED */
    { ccNone,		DoReloc		},
    { ccNone,		DoRepeat	},
    { ccNone,		DoRes		},
    { ccNone,		DoInvalid      	},     	/* .RIGHT */
    { ccNone,		DoROData	},
    { ccNone,           DoScope         },
    { ccNone,       	DoSegment	},
    { ccNone,           DoUnexpected    },      /* .SET */
    { ccNone,          	DoSetCPU        },
    { ccNone,           DoUnexpected    },      /* .SIZEOF */
    { ccNone,        	DoSmart		},
    { ccNone,           DoUnexpected    },      /* .SPRINTF */
    { ccNone,		DoUnexpected	},   	/* .STRAT */
    { ccNone,          	DoUnexpected	},   	/* .STRING */
    { ccNone,		DoUnexpected	},   	/* .STRLEN */
    { ccNone,           DoStruct        },
    { ccNone,		DoSunPlus	},
    { ccNone,           DoTag           },
    { ccNone,		DoUnexpected	},   	/* .TCOUNT */
    { ccNone,  	       	DoUnexpected	},   	/* .TIME */
    { ccNone,           DoUnion         },
    { ccNone,           DoUnexpected    },      /* .VERSION */
    { ccNone,		DoWarning	},
    { ccNone,       	DoWord		},
    { ccNone,  	       	DoUnexpected	},   	/* .XMATCH */
    { ccNone,       	DoZeropage	},
};



/*****************************************************************************/
/*     	       	    		     Code    				     */
/*****************************************************************************/



void HandlePseudo (void)
/* Handle a pseudo instruction */
{
    CtrlDesc* D;

    /* Calculate the index into the table */
    unsigned Index = Tok - TOK_FIRSTPSEUDO;

    /* Safety check */
    if (PSEUDO_COUNT != (TOK_LASTPSEUDO - TOK_FIRSTPSEUDO + 1)) {
	Internal ("Pseudo mismatch: PSEUDO_COUNT = %u, actual count = %u\n",
       	       	  (unsigned) PSEUDO_COUNT, TOK_LASTPSEUDO - TOK_FIRSTPSEUDO + 1);
    }
    CHECK (Index < PSEUDO_COUNT);

    /* Get the pseudo intruction descriptor */
    D = &CtrlCmdTab [Index];

    /* Remember the instruction, then skip it if needed */
    if ((D->Flags & ccKeepToken) == 0) {
    	SB_Copy (&Keyword, &SVal);
     	NextTok ();
    }

    /* Call the handler */
    D->Handler ();
}



void SegStackCheck (void)
/* Check if the segment stack is empty at end of assembly */
{
    if (CollCount (&SegStack) != 0) {
        Error ("Segment stack is not empty");
    }
}



