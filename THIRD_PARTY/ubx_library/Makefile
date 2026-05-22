CC := gcc

UNIX_SOURCES := serial_port_unix.c
WIN_SOURCES := serial_port_win.c ubxutil_win.c

LIBRARY_UNIX := libubx.a
LIBRARY_WIN := ubx.lib

SOURCES := $(wildcard *.c)


ifeq ($(OS), Windows_NT)
$(info Building Windows sources)
CC := mingw32-gcc
SOURCES := $(filter-out $(UNIX_SOURCES),$(SOURCES))
LIBRARY := $(LIBRARY_WIN)
else
$(info	Building Unix sources)
SOURCES := $(filter-out $(WIN_SOURCES),$(SOURCES))
LIBRARY := $(LIBRARY_UNIX)
endif

OBJECTS := $(SOURCES:.c=.o)

all : $(LIBRARY)

$(OBJECTS) : %.o : %.c
	$(CC) -c $^ -o $@

$(LIBRARY) : $(OBJECTS)
	ar -cr $@ $^

clean:
	rm -rf $(OBJECTS) $(LIBRARY_UNIX) $(LIBRARY_WIN)

