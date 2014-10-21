CC=gcc
ifeq ($(CC), gcc)
	CFLAGS += --short-enums
endif

BINARY=p2p
INCLUDEDIRS=includes
LIBS=-pthread -luuid
CFLAGS=$(addprefix -I, $(INCLUDEDIRS)) -Wall -Wextra $(LIBS)

all: CFLAGS += -Ofast
all: LDFLAGS += -s
all: depend $(BINARY)

debug: CFLAGS += -g -DDEBUG
debug: depend $(BINARY)

$(BINARY): p2p.o utils.o networkingV2.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

depend:
	$(CC) $(addprefix -I, $(INCLUDEDIRS)) -MM *.c > .depend

-include .depend

compress: $(BINARY)
	gzexe $(BINARY) && $(RM) $(BINARY)~

decompress:
	test -f $(BINARY) && gzexe -d $(BINARY) && $(RM) $(BINARY)~ || $(MAKE)

.PHONY: clean distclean depend

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(BINARY)
