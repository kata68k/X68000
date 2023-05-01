#	makefile for BattleKata.x
PROGNAME	:= BattleKata.x
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

CC		= gcc
CFLAGS		= -Wall -O2 -I $(INCDIR)
#CFLAGS		= -O -fomit-frame-pointer -fstrength-reduce -Wall
#CFLAGS		= -g -O0 -Wall
AS			= has
LD			= hlk
LKFLAGS		= -z-stack=131072
LDFLAGS 	= -s
LIBS		= libc.a libgnu.a libdos.a libiocs.a lib/APICGLIB.a lib/ZMUSIC.L lib/JOYLIB3.L lib/XSP2lib.o


.PHONY: all clean dist
all: $(TARGET)

clean:
	-rm $(OBJDIR)

dist:
	LHA a -t $(LZH)

$(TARGET): $(OBJS)
	$(LD) -o $@ $^ -l $(LIBS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	$(AS) -o $@ -c $<
