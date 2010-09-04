#
# gcc Makefile for co65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= co65

# Library dir
COMMON	= ../common

#
CC	= gcc
CFLAGS 	= -g -O2 -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
EBIND	= emxbind
LDFLAGS	=

# -----------------------------------------------------------------------------
# List of all object files

OBJS =	convert.o       \
	error.o         \
	fileio.o        \
	global.o        \
	main.o          \
	model.o         \
	o65.o

LIBS = $(COMMON)/common.a

# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $^ $(LDFLAGS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $^ > .depend


