NAME=zstr

INCDIRS=-I../libzutil
LIBDIRS=-L../libzutil
LIBS=-lzutil

LIBPREFIX=lib
LIBSUFFIX=.a

RANLIB=ranlib
AR=ar

CFLAGS=-DNDEBUG -Wall -O0 $(INCDIRS)
LDFLAGS=$(LIBDIRS) $(LIBS)

# SRCS=$(wildcard *.c)
SRCS=zstr.c
TESTSRCS=test.c
OBJS=$(SRCS:%.c=%.o)
TESTOBJS=$(TESTSRCS:%.c=%.o)
TEST=test
LIB=$(LIBPREFIX)$(NAME)$(LIBSUFFIX)

all: $(LIB) $(TEST)

include $(SRCS:%.c=%.d)

%.d: %.c
	@echo remaking $@
	@set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

$(LIB): $(OBJS)
	$(AR) -r $@ $+
	$(RANLIB) $@

$(TEST): $(TESTOBJS) $(LIB)
	$(CC) $+ -o $@ $(LDFLAGS)

clean:
	-rm $(LIB) $(OBJS) $(TEST) $(TESTOBJS) *.d

.PHONY: clean all
