PROJECT = libdiskimage.a
OBJECTS = diskimage.o
CFLAGS = -Wall
CC = gcc

all: $(PROJECT)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	ar rcs $(PROJECT) $(OBJECTS)

clean:
	rm -f *.o *.a

install:
	cp $(PROJECT) /usr/local/lib
	cp diskimage.h /usr/local/include
