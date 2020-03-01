#	makefile for OverKata.x

CC		= gcc
CFLAGS	= -O -fomit-frame-pointer -fstrength-reduce
AS		= has
LD		= hlk
INC		= 
LIBS	= libc.a libgnu.a
LZH		= OverKata100

vpath	%.c	;

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

all:	OverKata.x 

clean:
	rm -f *.ind *.bak *.o *.x \
	inc/*.bak

execonly:
	rm -f *.ind *.bak *.o *.c *.h *.s makefile \
	rmdir inc

OverKata.x:	OverKata.o Draw.o MFP.o
	$(LD) $^ -o $@ -l $(LIBS)

OverKata.o:	OverKata.c OverKata.h
Draw.o:	Draw.c	 Draw.h
MFP.o:	MFP.c	 MFP.h

dist:
	LHA a -t ../$(LZH)

