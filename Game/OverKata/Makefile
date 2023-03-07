#	makefile for OverKata.x
PROGNAME	:= OverKata.x
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
AS			= has
LD			= hlk
LKFLAGS		= -z-stack=131072
LDFLAGS 	= -s
LIBS		= libc.a libgnu.a libdos.a libiocs.a LIB/APICGLIB.a LIB/ZMUSIC.L LIB/JOYLIB3.L
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/ZMUSIC.L
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/ZMSC3LIB.L
#LIBS		= libc.a libgnu.a LIB/APICGLIB.a LIB/mcclib.a

#LZH		= OverKata001

.PHONY: all clean dist
all: $(TARGET)

clean:
	-rm $(OBJDIR)

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