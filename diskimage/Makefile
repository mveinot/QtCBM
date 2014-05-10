PROJECT = libdiskimage.a
OBJECTS = diskimage.o
CFLAGS = -Wall -pedantic
CC = gcc

all: $(PROJECT)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	libtool -o $(PROJECT) -static $(OBJECTS)

clean:
	rm -f *.o *.a

install:
	cp $(PROJECT) /usr/local/lib
	cp diskimage.h /usr/local/include
