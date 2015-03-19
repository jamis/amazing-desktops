CC=gcc
CFLAGS=-Isrc
LDFLAGS=-lpng

SOURCES=src/maze.c src/colors.c src/gradient.c src/image.c \
  src/grid.c src/algorithm.c src/distances.c \
  src/growing_tree.c src/binary_tree.c
OBJECTS=$(SOURCES:.c=.o)

DELIVERABLE=maze

all: $(SOURCES) $(DELIVERABLE)

$(DELIVERABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(DELIVERABLE) *.png
