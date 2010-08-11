/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*		   Main program for the ca65 macroassembler		     */
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
#include <time.h>

/* common */
#include "addrsize.h"
#include "chartype.h"
#include "cmdline.h"
#include "mmodel.h"
#include "print.h"
#include "strbuf.h"
#include "target.h"
#include "tgttrans.h"
#include "version.h"

/* ca65 */
#include "abend.h"
#include "asserts.h"
#include "error.h"
#include "expr.h"
#include "feature.h"
#include "filetab.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "istack.h"
#include "lineinfo.h"
#include "listing.h"
#include "macpack.h"
#include "macro.h"
#include "nexttok.h"
#include "objfile.h"
#include "options.h"
#include "pseudo.h"
#include "scanner.h"
#include "segment.h"
#include "sizeof.h"
#include "spool.h"
#include "symtab.h"
#include "ulabel.h"



/*****************************************************************************/
/*     	       	     	       	     Code				     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: %s [options] file\n"
            "Short options:\n"
            "  -D name[=value]\tDefine a symbol\n"
            "  -I dir\t\tSet an include directory search path\n"
            "  -U\t\t\tMark unresolved symbols as import\n"
            "  -V\t\t\tPrint the assembler version\n"
            "  -W n\t\t\tSet warning level n\n"
            "  -g\t\t\tAdd debug info to object file\n"
            "  -h\t\t\tHelp (this text)\n"
            "  -i\t\t\tIgnore case of symbols\n"
            "  -l\t\t\tCreate a listing if assembly was ok\n"
            "  -mm model\t\tSet the memory model\n"
            "  -o name\t\tName the output file\n"
            "  -s\t\t\tEnable smart mode\n"
            "  -t sys\t\tSet the target system\n"
            "  -v\t\t\tIncrease verbosity\n"
            "\n"
            "Long options:\n"
            "  --auto-import\t\tMark unresolved symbols as import\n"
            "  --cpu type\t\tSet cpu type\n"
            "  --debug-info\t\tAdd debug info to object file\n"
            "  --feature name\tSet an emulation feature\n"
            "  --forget-inc-paths\tForget include search paths\n"
            "  --help\t\tHelp (this text)\n"
            "  --ignore-case\t\tIgnore case of symbols\n"
            "  --include-dir dir\tSet an include directory search path\n"
            "  --listing\t\tCreate a listing if assembly was ok\n"
            "  --list-bytes n\tMaximum number of bytes per listing line\n"
            "  --macpack-dir dir\tSet a macro package directory\n"
            "  --memory-model model\tSet the memory model\n"
            "  --pagelength n\tSet the page length for the listing\n"
            "  --smart\t\tEnable smart mode\n"
            "  --target sys\t\tSet the target system\n"
            "  --verbose\t\tIncrease verbosity\n"
            "  --version\t\tPrint the assembler version\n",
            ProgName);
}



static void SetOptions (void)
/* Set the option for the translator */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Set the translator */
    SB_Printf (&Buf, "ca65 V%s", GetVersionAsString ());
    OptTranslator (&Buf);

    /* Set date and time */
    OptDateTime ((unsigned long) time(0));

    /* Release memory for the string */
    SB_Done (&Buf);
}



static void NewSymbol (const char* SymName, long Val)
/* Define a symbol with a fixed numeric value in the current scope */
{
    ExprNode* Expr;
    SymEntry* Sym;

    /* Convert the name to a string buffer */
    StrBuf SymBuf = STATIC_STRBUF_INITIALIZER;
    SB_CopyStr (&SymBuf, SymName);

    /* Search for the symbol, allocate a new one if it doesn't exist */
    Sym = SymFind (CurrentScope, &SymBuf, SYM_ALLOC_NEW);

    /* Check if have already a symbol with this name */
    if (SymIsDef (Sym)) {
    	AbEnd ("`%s' is already defined", SymName);
    }

    /* Generate an expression for the symbol */
    Expr = GenLiteralExpr (Val);

    /* Mark the symbol as defined */
    SymDef (Sym, Expr, ADDR_SIZE_DEFAULT, SF_NONE);

    /* Free string buffer memory */
    SB_Done (&SymBuf);
}



static void CBMSystem (const char* Sys)
/* Define a CBM system */
{
    NewSymbol ("__CBM__", 1);
    NewSymbol (Sys, 1);
}



static void SetSys (const char* Sys)
/* Define a target system */
{
    switch (Target = FindTarget (Sys)) {

	case TGT_NONE:
	    break;

        case TGT_MODULE:
            AbEnd ("Cannot use `module' as a target for the assembler");
            break;

	case TGT_ATARI:
    	    NewSymbol ("__ATARI__", 1);
	    break;

	case TGT_C16:
	    CBMSystem ("__C16__");
	    break;

	case TGT_C64:
	    CBMSystem ("__C64__");
	    break;

	case TGT_VIC20:
	    CBMSystem ("__VIC20__");
	    break;

	case TGT_C128:
	    CBMSystem ("__C128__");
	    break;

	case TGT_PLUS4:
	    CBMSystem ("__PLUS4__");
	    break;

	case TGT_CBM510:
	    CBMSystem ("__CBM510__");
	    break;

	case TGT_CBM610:
	    CBMSystem ("__CBM610__");
	    break;

	case TGT_PET:
     	    CBMSystem ("__PET__");
     	    break;

     	case TGT_BBC:
     	    NewSymbol ("__BBC__", 1);
     	    break;

     	case TGT_APPLE2:
     	    NewSymbol ("__APPLE2__", 1);
     	    break;

     	case TGT_APPLE2ENH:
            NewSymbol ("__APPLE2ENH__", 1);
     	    break;

     	case TGT_GEOS:
     	    /* Do not handle as a CBM system */
     	    NewSymbol ("__GEOS__", 1);
     	    break;

	case TGT_LUNIX:
	    NewSymbol ("__LUNIX__", 1);
	    break;

        case TGT_ATMOS:
            NewSymbol ("__ATMOS__", 1);
            break;

        case TGT_NES:
            NewSymbol ("__NES__", 1);
            break;

        case TGT_SUPERVISION:
            NewSymbol ("__SUPERVISION__", 1);
            break;

        case TGT_LYNX:
            NewSymbol ("__LYNX__", 1);
            break;

     	default:
            AbEnd ("Invalid target name: `%s'", Sys);

    }

    /* Initialize the translation tables for the target system */
    TgtTranslateInit ();
}



static void DefineSymbol (const char* Def)
/* Define a symbol from the command line */
{
    const char* P;
    unsigned I;
    long Val;
    StrBuf SymName = AUTO_STRBUF_INITIALIZER;


    /* The symbol must start with a character or underline */
    if (!IsIdStart (Def [0])) {
	InvDef (Def);
    }
    P = Def;

    /* Copy the symbol, checking the rest */
    I = 0;
    while (IsIdChar (*P)) {
        SB_AppendChar (&SymName, *P++);
    }
    SB_Terminate (&SymName);

    /* Do we have a value given? */
    if (*P != '=') {
	if (*P != '\0') {
	    InvDef (Def);
	}
	Val = 0;
    } else {
    	/* We have a value */
    	++P;
    	if (*P == '$') {
    	    ++P;
    	    if (sscanf (P, "%lx", &Val) != 1) {
    	       	InvDef (Def);
    	    }
    	} else {
    	    if (sscanf (P, "%li", &Val) != 1) {
     	       	InvDef (Def);
    	    }
       	}
    }

    /* Define the new symbol */
    NewSymbol (SB_GetConstBuf (&SymName), Val);

    /* Release string memory */
    SB_Done (&SymName);
}



static void OptAutoImport (const char* Opt attribute ((unused)),
    	    	     	   const char* Arg attribute ((unused)))
/* Mark unresolved symbols as imported */
{
    AutoImport = 1;
}



static void OptCPU (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --cpu option */
{
    cpu_t CPU = FindCPU (Arg);
    if (CPU == CPU_UNKNOWN) {
	AbEnd ("Invalid CPU: `%s'", Arg);
    } else {
	SetCPU (CPU);
    }
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
		     	  const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DbgSyms = 1;
}



static void OptFeature (const char* Opt attribute ((unused)), const char* Arg)
/* Set an emulation feature */
{
    /* Make a string buffer from Arg */
    StrBuf Feature;

    /* Set the feature, check for errors */
    if (SetFeature (SB_InitFromString (&Feature, Arg)) == FEAT_UNKNOWN) {
      	AbEnd ("Illegal emulation feature: `%s'", Arg);
    }
}



static void OptForgetIncPaths (const char* Opt attribute ((unused)),
                               const char* Arg attribute ((unused)))
/* Forget all currently defined include paths */
{
    ForgetAllIncludePaths ();
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIgnoreCase (const char* Opt attribute ((unused)),
			   const char* Arg attribute ((unused)))
/* Ignore case on symbols */
{
    IgnoreCase = 1;
}



static void OptIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Add an include search path */
{
    AddIncludePath (Arg);
}



static void OptListBytes (const char* Opt, const char* Arg)
/* Set the maximum number of bytes per listing line */
{
    unsigned Num;
    char     Check;

    /* Convert the argument to a number */
    if (sscanf (Arg, "%u%c", &Num, &Check) != 1) {
        AbEnd ("Invalid argument for option `%s'", Opt);
    }

    /* Check the bounds */
    if (Num != 0 && (Num < MIN_LIST_BYTES || Num > MAX_LIST_BYTES)) {
        AbEnd ("Argument for option `%s' is out of range", Opt);
    }

    /* Use the value */
    SetListBytes (Num);
}



static void OptListing (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Create a listing file */
{
    Listing = 1;
}



static void OptMacPackDir (const char* Opt attribute ((unused)), const char* Arg)
/* Set a macro package directory */
{
    /* Make a string buffer from Arg */
    StrBuf Dir;

    /* Use the directory */
    MacPackSetDir (SB_InitFromString (&Dir, Arg));
}



static void OptMemoryModel (const char* Opt, const char* Arg)
/* Set the memory model */
{
    mmodel_t M;

    /* Check the current memory model */
    if (MemoryModel != MMODEL_UNKNOWN) {
        AbEnd ("Cannot use option `%s' twice", Opt);
    }

    /* Translate the memory model name and check it */
    M = FindMemoryModel (Arg);
    if (M == MMODEL_UNKNOWN) {
        AbEnd ("Unknown memory model: %s", Arg);
    } else if (M == MMODEL_HUGE) {
        AbEnd ("Unsupported memory model: %s", Arg);
    }

    /* Set the memory model */
    SetMemoryModel (M);
}



static void OptPageLength (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --pagelength option */
{
    int Len = atoi (Arg);
    if (Len != -1 && (Len < MIN_PAGE_LEN || Len > MAX_PAGE_LEN)) {
	AbEnd ("Invalid page length: %d", Len);
    }
    PageLength = Len;
}



static void OptSmart (const char* Opt attribute ((unused)),
		      const char* Arg attribute ((unused)))
/* Handle the -s/--smart options */
{
    SmartMode = 1;
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    SetSys (Arg);
}



static void OptVerbose (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr, "ca65 V%s - %s\n", GetVersionAsString (), Copyright);
}



static void DoPCAssign (void)
/* Start absolute code */
{
    long PC = ConstExpression ();
    if (PC < 0 || PC > 0xFFFFFF) {
	Error ("Range error");
    } else {
	EnterAbsoluteMode (PC);
    }
}



static void OneLine (void)
/* Assemble one line */
{
    Segment*      Seg   = 0;
    unsigned long PC    = 0;
    SymEntry*     Sym   = 0;
    int           Macro = 0;
    int           Instr = -1;

    /* Initialize the new listing line if we are actually reading from file
     * and not from internally pushed input.
     */
    if (!HavePushedInput ()) {
       	InitListingLine ();
    }

    if (Tok == TOK_COLON) {
       	/* An unnamed label */
       	ULabDef ();
       	NextTok ();
    }

    /* If the first token on the line is an identifier, check for a macro or
     * an instruction.
     */
    if (Tok == TOK_IDENT) {
        if (!UbiquitousIdents) {
            /* Macros and symbols cannot use instruction names */
            Instr = FindInstruction (&SVal);
            if (Instr < 0) {
                Macro = IsMacro (&SVal);
            }
        } else {
            /* Macros and symbols may use the names of instructions */
            Macro = IsMacro (&SVal);
        }
    }

    /* Handle an identifier */
    if (Tok == TOK_LOCAL_IDENT || (Tok == TOK_IDENT && Instr < 0 && !Macro)) {

        /* Did we have whitespace before the ident? */
        int HadWS = WS;

        /* Generate the symbol table entry, then skip the name */
        if (Tok == TOK_IDENT) {
            Sym = SymFind (CurrentScope, &SVal, SYM_ALLOC_NEW);
        } else {
            Sym = SymFindLocal (SymLast, &SVal, SYM_ALLOC_NEW);
        }
        NextTok ();

        /* If a colon follows, this is a label definition. If there
         * is no colon, it's an assignment.
         */
        if (Tok == TOK_EQ || Tok == TOK_ASSIGN) {

            /* Determine the symbol flags from the assignment token */
            unsigned Flags = (Tok == TOK_ASSIGN)? SF_LABEL : SF_NONE;

            /* Skip the '=' */
            NextTok ();

            /* Define the symbol with the expression following the '=' */
            SymDef (Sym, Expression(), ADDR_SIZE_DEFAULT, Flags);

            /* Don't allow anything after a symbol definition */
            ConsumeSep ();
            return;

        } else if (Tok == TOK_SET) {

            ExprNode* Expr;

            /* .SET defines variables (= redefinable symbols) */
            NextTok ();

            /* Read the assignment expression, which must be constant */
            Expr = GenLiteralExpr (ConstExpression ());

            /* Define the symbol with the constant expression following
             * the '='
             */
            SymDef (Sym, Expr, ADDR_SIZE_DEFAULT, SF_VAR);

            /* Don't allow anything after a symbol definition */
            ConsumeSep ();
            return;

        } else {

            /* A label. Remember the current segment, so we can later
             * determine the size of the data stored under the label.
             */
            Seg = ActiveSeg;
            PC  = GetPC ();

            /* Define the label */
            SymDef (Sym, GenCurrentPC (), ADDR_SIZE_DEFAULT, SF_LABEL);

            /* Skip the colon. If NoColonLabels is enabled, allow labels
             * without a colon if there is no whitespace before the
             * identifier.
             */
            if (Tok != TOK_COLON) {
                if (HadWS || !NoColonLabels) {
                    Error ("`:' expected");
                    /* Try some smart error recovery */
                    if (Tok == TOK_NAMESPACE) {
                        NextTok ();
                    }
                }
            } else {
                /* Skip the colon */
                NextTok ();
            }

            /* If we come here, a new identifier may be waiting, which may
             * be a macro or instruction.
             */
            if (Tok == TOK_IDENT) {
                if (!UbiquitousIdents) {
                    /* Macros and symbols cannot use instruction names */
                    Instr = FindInstruction (&SVal);
                    if (Instr < 0) {
                        Macro = IsMacro (&SVal);
                    }
                } else {
                    /* Macros and symbols may use the names of instructions */
                    Macro = IsMacro (&SVal);
                }
            }
        }
    }

    /* We've handled a possible label, now handle the remainder of the line */
    if (Tok >= TOK_FIRSTPSEUDO && Tok <= TOK_LASTPSEUDO) {
        /* A control command */
        HandlePseudo ();
    } else if (Macro) {
        /* A macro expansion */
        MacExpandStart ();
    } else if (Instr >= 0 ||
               (UbiquitousIdents && ((Instr = FindInstruction (&SVal)) >= 0))) {
        /* A mnemonic - assemble one instruction */
        HandleInstruction (Instr);
    } else if (PCAssignment && (Tok == TOK_STAR || Tok == TOK_PC)) {
        NextTok ();
        if (Tok != TOK_EQ) {
            Error ("`=' expected");
            SkipUntilSep ();
        } else {
            /* Skip the equal sign */
            NextTok ();
            /* Enter absolute mode */
            DoPCAssign ();
        }
    }

    /* If we have defined a label, remember its size. Sym is also set by
     * a symbol assignment, but in this case Done is false, so we don't
     * come here.
     */
    if (Sym) {
        unsigned long Size;
        if (Seg == ActiveSeg) {
            /* Same segment */
            Size = GetPC () - PC;
        } else {
            /* The line has switched the segment */
            Size = 0;
        }
        DefSizeOfSymbol (Sym, Size);
    }

    /* Line separator must come here */
    ConsumeSep ();
}



static void Assemble (void)
/* Start the ball rolling ... */
{
    /* Prime the pump */
    NextTok ();

    /* Assemble lines until end of file */
    while (Tok != TOK_EOF) {
     	OneLine ();
    }
}



static void CreateObjFile (void)
/* Create the object file */
{
    /* Open the object, write the header */
    ObjOpen ();

    /* Write the object file options */
    WriteOptions ();

    /* Write the list of input files */
    WriteFiles ();

    /* Write the segment data to the file */
    WriteSegments ();

    /* Write the import list */
    WriteImports ();

    /* Write the export list */
    WriteExports ();

    /* Write debug symbols if requested */
    WriteDbgSyms ();

    /* Write line infos if requested */
    WriteLineInfo ();

    /* Write the string pool */
    WriteStrPool ();

    /* Write the assertions */
    WriteAssertions ();

    /* Write an updated header and close the file */
    ObjClose ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--auto-import",     	0,	OptAutoImport		},
        { "--cpu",     	       	1,	OptCPU 			},
	{ "--debug-info",      	0,	OptDebugInfo		},
	{ "--feature",		1,	OptFeature		},
       	{ "--forget-inc-paths",	0,     	OptForgetIncPaths       },
	{ "--help",    		0,	OptHelp			},
	{ "--ignore-case",     	0,	OptIgnoreCase 		},
	{ "--include-dir",     	1,	OptIncludeDir		},
        { "--list-bytes",       1,      OptListBytes            },
	{ "--listing", 	       	0,	OptListing		},
        { "--macpack-dir",      1,      OptMacPackDir           },
        { "--memory-model",     1,      OptMemoryModel          },
	{ "--pagelength",      	1,	OptPageLength		},
    	{ "--smart",   	       	0,	OptSmart		},
	{ "--target",  		1,	OptTarget		},
	{ "--verbose", 	       	0,	OptVerbose		},
	{ "--version", 	       	0,	OptVersion		},
    };

    /* Name of the global name space */
    static const StrBuf GlobalNameSpace = STATIC_STRBUF_INITIALIZER;

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "ca65");

    /* Initialize the include search paths */
    InitIncludePaths ();

    /* Enter the base lexical level. We must do that here, since we may
     * define symbols using -D.
     */
    SymEnterLevel (&GlobalNameSpace, ST_GLOBAL, ADDR_SIZE_DEFAULT);

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

       	/* Get the argument */
       	const char* Arg = ArgVec [I];

       	/* Check for an option */
       	if (Arg[0] == '-') {
       	    switch (Arg[1]) {

	       	case '-':
	       	    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
	       	    break;

       	       	case 'g':
       	       	    OptDebugInfo (Arg, 0);
       	       	    break;

	       	case 'h':
	       	    OptHelp (Arg, 0);
	       	    break;

       	        case 'i':
       	       	    OptIgnoreCase (Arg, 0);
       	       	    break;

       	       	case 'l':
	       	    OptListing (Arg, 0);
       	       	    break;

                case 'm':
                    if (Arg[2] == 'm') {
                        OptMemoryModel (Arg, GetArg (&I, 3));
                    } else {
                        UnknownOption (Arg);
                    }
                    break;

       	        case 'o':
       		    OutFile = GetArg (&I, 2);
       	     	    break;

       		case 's':
       		    OptSmart (Arg, 0);
       		    break;

		case 't':
		    OptTarget (Arg, GetArg (&I, 2));
		    break;

       	       	case 'v':
		    OptVerbose (Arg, 0);
       	       	    break;

    	        case 'D':
    		    DefineSymbol (GetArg (&I, 2));
    		    break;

    		case 'I':
    		    OptIncludeDir (Arg, GetArg (&I, 2));
    		    break;

       	        case 'U':
		    OptAutoImport (Arg, 0);
       		    break;

       	        case 'V':
    	    	    OptVersion (Arg, 0);
       		    break;

       	        case 'W':
       		    WarnLevel = atoi (GetArg (&I, 2));
       		    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* Filename. Check if we already had one */
    	    if (InFile) {
    	       	fprintf (stderr, "%s: Don't know what to do with `%s'\n",
	     		 ProgName, Arg);
	     	exit (EXIT_FAILURE);
    	    } else {
	     	InFile = Arg;
	    }
     	}

	/* Next argument */
	++I;
    }

    /* Do we have an input file? */
    if (InFile == 0) {
	fprintf (stderr, "%s: No input files\n", ProgName);
	exit (EXIT_FAILURE);
    }

    /* If no CPU given, use the default CPU for the target */
    if (GetCPU () == CPU_UNKNOWN) {
        if (Target != TGT_UNKNOWN) {
            SetCPU (DefaultCPU[Target]);
        } else {
            SetCPU (CPU_6502);
        }
    }

    /* If no memory model was given, use the default */
    if (MemoryModel == MMODEL_UNKNOWN) {
        SetMemoryModel (MMODEL_NEAR);
    }

    /* Initialize the segments */
    InitSegments ();

    /* Initialize the scanner, open the input file */
    InitScanner (InFile);

    /* Define the default options */
    SetOptions ();

    /* Assemble the input */
    Assemble ();

    /* If we didn't have any errors, check the segment stack */
    if (ErrorCount == 0) {
        SegStackCheck ();
    }

    /* If we didn't have any errors, check the unnamed labels */
    if (ErrorCount == 0) {
        ULabCheck ();
    }

    /* If we didn't have any errors, check the symbol table */
    if (ErrorCount == 0) {
        SymCheck ();
    }

    /* If we didn't have any errors, check and resolve the segment data */
    if (ErrorCount == 0) {
        SegCheck ();
    }

    /* If we didn't have any errors, check the assertions */
    if (ErrorCount == 0) {
        CheckAssertions ();
    }

    /* If we didn't have an errors, index the line infos */
    MakeLineInfoIndex ();

    /* Dump the data */
    if (Verbosity >= 2) {
        SymDump (stdout);
        SegDump ();
    }

    /* If we didn't have any errors, create the object and listing files */
    if (ErrorCount == 0) {
	CreateObjFile ();
	if (Listing) {
	    CreateListing ();
	}
    }

    /* Close the input file */
    DoneScanner ();

    /* Return an apropriate exit code */
    return (ErrorCount == 0)? EXIT_SUCCESS : EXIT_FAILURE;
}



