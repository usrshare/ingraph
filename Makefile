OBJDIR=obj
SRCDIR=src

CC=gcc
LD=ld

CFLAGS=-std=c99 $(shell ncursesw5-config --cflags)
LIBS=$(shell ncursesw5-config --libs) -lm
DEFS=-D_POSIX_C_SOURCE=2

_OBJS = main.o

ifdef RELEASE
DEFS += -O3 -DNDEBUG
else
DEFS += -g
endif

OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

ingraph: $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFS)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR)/*.o ingraph
