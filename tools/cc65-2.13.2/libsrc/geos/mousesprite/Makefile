#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= startmousemode.o clearmousemode.o mouseup.o mouseoff.o\
	  drawsprite.o possprite.o enablsprite.o disablsprite.o\
	  ismseinregion.o inittextprompt.o promptoff.o prompton.o\
	  getnextchar.o\
	  mouse.o

all: $(S_OBJS)

clean:
	@$(RM) *.~ $(S_OBJS) core
