CC ?= gcc
LD ?= gcc
RM ?= rm -f

CFLAGS ?= -g -O2
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

CFLAGS += -Wall -Wextra -Wformat -std=gnu17 -fPIE -fno-omit-frame-pointer -fstack-protector-strong
CPPFLAGS += -I/usr/local/include -D_DEFAULT_SOURCE
LDFLAGS += -pie
LIBS += -L/usr/local/lib -llua-5.4

PROGRAM = lua_example
SOURCES = main.c repl.c lua2c.c c2lua.c yield.c 
OBJECTS = $(SOURCES:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES: .o
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)


.PHONY: all clean

clean:
	$(RM) $(PROGRAM) $(OBJECTS)
