#
# Makefile for cross-compiler version of CC65.
#




# ------------------------------------------------------------------------------

# The executable to build
EXE  	= cc65

# Library directories
COMMON	= ../common

# The compiler include search path. Default is "/usr/lib/cc65/include/" if
# nothing is defined. You may use CC65_INC=foo on the command line to override it.
CC65_INC = \"/usr/lib/cc65/include/\"

#
CC      = gcc
CFLAGS  = -O2 -g -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
override CFLAGS += -DCC65_INC=$(CC65_INC)
EBIND   = emxbind
LDFLAGS = -lm

# Determine the svn version number if possible
ifneq "$(shell which svnversion 2>/dev/null)" ""
ifneq "$(wildcard .svn)" ""
SVNVERSION=$(shell svnversion)
else
SVNVERSION=unknown
endif
else
SVNVERSION=unknown
endif

# ------------------------------------------------------------------------------
# Object files and libraries to link

OBJS =	anonname.o	\
	asmcode.o 	\
	asmlabel.o	\
	asmstmt.o	\
	assignment.o    \
	casenode.o	\
	codeent.o	\
	codegen.o 	\
	codelab.o	\
	codeinfo.o	\
	codeopt.o	\
	codeseg.o	\
	compile.o 	\
	coptadd.o	\
	coptc02.o	\
	coptcmp.o	\
	coptind.o	\
	coptneg.o	\
	coptptrload.o   \
	coptpush.o	\
	coptsize.o      \
	coptstop.o	\
	coptstore.o     \
	coptsub.o	\
	copttest.o	\
	dataseg.o	\
	datatype.o	\
	declare.o      	\
	declattr.o	\
	error.o	  	\
	expr.o	 	\
	exprdesc.o      \
	funcdesc.o	\
	function.o     	\
	global.o 	\
	goto.o	   	\
	hexval.o        \
	ident.o		\
	incpath.o      	\
	input.o		\
	lineinfo.o	\
	litpool.o      	\
	loadexpr.o      \
	locals.o	\
	loop.o		\
	macrotab.o	\
	main.o		\
	opcodes.o	\
	output.o        \
	preproc.o      	\
	pragma.o       	\
	reginfo.o	\
	scanner.o      	\
	scanstrbuf.o    \
	segments.o	\
	shiftexpr.o     \
	stackptr.o      \
	standard.o      \
	stdfunc.o	\
	stdnames.o      \
	stmt.o 	 	\
	svnversion.o    \
	swstmt.o	\
	symentry.o	\
	symtab.o       	\
	testexpr.o      \
	textseg.o	\
	typecmp.o	\
	typeconv.o      \
	util.o

LIBS =	$(COMMON)/common.a


# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	svnversion $(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $(OBJS) $(LIBS) $(LDFLAGS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

.PHONY:	svnversion
svnversion:
	@$(RM) svnversion.c
	@echo "/* This file is auto-generated - do not modify! */" >> svnversion.c
	@echo "" >> svnversion.c
	@echo "const char SVNVersion[] = \"$(SVNVERSION)\";" >> svnversion.c

svnversion.c:	svnversion

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) $(OBJS) $(EXE) .depend svnversion.c

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM $^ > .depend



