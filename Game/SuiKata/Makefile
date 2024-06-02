#	makefile for SUIKA.x
PROGNAME	:= SUIKA.x
SRCDIR		:= src
INCDIR		:= inc
LIBDIR		:= lib
OBJDIR		:= build
TARGET		:= $(PROGNAME)
SOURCES_C	:= $(wildcard $(SRCDIR)/*.c)
SOURCES_S	:= $(wildcard $(SRCDIR)/*.s)
SOURCES_EX	:= $(notdir $(SOURCES_C:.c=.o)) $(notdir $(SOURCES_S:.s=.o))
OBJS		:= $(addprefix $(OBJDIR)/,$(SOURCES_EX)) 
DPNDS		:= $(OBJ:.o=.d)
#$(warning $(OBJS))

CC			= gcc
CFLAGS		= -Wall -O2 -I $(INCDIR)
#CFLAGS		= -O -fomit-frame-pointer -fstrength-reduce -Wall
#CFLAGS		= -g -O0 -Wall
AS			= has060
LD			= hlk
LKFLAGS		= -z-stack=131072
LDFLAGS 	= -s
LIBS		= libc.a libgnu.a libdos.a libiocs.a lib/APICGLIB.a lib/ZMUSIC.L lib/JOYLIB3.L lib/XSP2lib.o Lib/PCM8Afnc.o
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/ZMUSIC.L
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/ZMSC3LIB.L
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/mcclib.a

#LZH		= OverKata001

.PHONY: all clean dist
all: $(TARGET)

clean:
	del $(OBJDIR)\*.o

dist:
	LHA a -t $(LZH)

$(TARGET): $(OBJS)
	$(LD) -o $@ $^ -l $(LIBS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
#	-mkdir -p ./$(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	$(AS) -o $@ -c $<
