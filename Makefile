CC         = /usr/bin/gcc
RM         = /bin/rm
INCLUDE    = -I.
LIBRARY    = -L.
LDFLAGS    = -lm -lgd

#############################################################################

all: colorMod.o
	$(CC) -o colorMod colorMod.o $(LIBRARY) $(LDFLAGS)


colorMod.o: colorMod.c
	$(CC) -c $(INCLUDE) -o colorMod.o colorMod.c

clean:
	$(RM) colorMod *.o core.*
