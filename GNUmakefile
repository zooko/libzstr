NAME=zstr

INCDIRS=-I../libzutil
LIBDIRS=-L../libzutil
LIBS=-lzutil

LIBPREFIX=lib
LIBSUFFIX=.a

RANLIB=ranlib
AR=ar

# CFLAGS=-DNDEBUG -Wall -O2 $(INCDIRS)
CFLAGS=-UNDEBUG -Wall -O0 $(INCDIRS) -g
LDFLAGS=$(LIBDIRS) $(LIBS) -g

# SRCS=$(wildcard *.c)
SRCS=zstr.c
TESTSRCS=test.c
OBJS=$(SRCS:%.c=%.o)
TESTOBJS=$(TESTSRCS:%.c=%.o)
TEST=test
LIB=$(LIBPREFIX)$(NAME)$(LIBSUFFIX)

all: $(LIB) $(TEST)

# .d auto-dependency files
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
ifneq (,$(SRCS:%.c=%.d))
-include $(SRCS:%.c=%.d)
endif
endif

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
	-rm $(LIB) $(OBJS) $(TEST) $(TESTOBJS) *.d 2>/dev/null

.PHONY: clean all
