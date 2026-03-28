CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
TARGET  = salc

SRCDIR  = src
OBJDIR  = obj

SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/lex.c \
          $(SRCDIR)/parser.c \
          $(SRCDIR)/symtab.c \
          $(SRCDIR)/diag.c \
          $(SRCDIR)/log.c \
          $(SRCDIR)/opt.c \
          $(SRCDIR)/token.c

OBJS    = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(TARGET): $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o:    $(SRCDIR)/main.c    include/diag.h include/lex.h include/log.h include/opt.h include/parser.h include/symtab.h
$(OBJDIR)/lex.o:     $(SRCDIR)/lex.c     include/lex.h include/token.h
$(OBJDIR)/parser.o:  $(SRCDIR)/parser.c  include/parser.h include/diag.h include/lex.h include/log.h include/symtab.h include/token.h
$(OBJDIR)/symtab.o:  $(SRCDIR)/symtab.c  include/symtab.h
$(OBJDIR)/diag.o:    $(SRCDIR)/diag.c    include/diag.h include/token.h include/log.h
$(OBJDIR)/log.o:     $(SRCDIR)/log.c     include/log.h include/opt.h include/symtab.h include/token.h
$(OBJDIR)/opt.o:     $(SRCDIR)/opt.c     include/opt.h
$(OBJDIR)/token.o:   $(SRCDIR)/token.c   include/token.h

clean:
	rm -f $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: clean