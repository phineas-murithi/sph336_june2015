CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

test:all
	@echo running tests...
#	@echo $(PATH)
	./main 2
	./main r

all:clean $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
	@echo linking...
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS):$(SOURCES)
	@echo compiling...
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o main
