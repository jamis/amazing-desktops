CC=gcc
CFLAGS=
LDFLAGS=-lpng

SOURCES=src/maze.c
OBJECTS=$(SOURCES:.c=.o)

DELIVERABLE=maze

all: $(SOURCES) $(DELIVERABLE)

$(DELIVERABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(DELIVERABLE) *.png
