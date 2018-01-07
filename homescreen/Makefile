CC=gcc
LIBS=clutter-0.9 gtk+-2.0 clutter-gtk-0.9
CFLAGS=`pkg-config --cflags $(LIBS)` -I./ -I../include
LDFLAGS=`pkg-config --libs $(LIBS)`
SRCS=main.c dock.c webblk.c calblk.c todoblk.c mailblk.c alphafuncs.c clutter-behaviour-scale-gravity.c dvm-clutter-actor-data-snapshot.c clutter-behaviour-function.c
OBJS=${SRCS:.c=.o}

all: $(OBJS)
	$(CC) -o a.out $(OBJS) $(LDFLAGS)

.c:.o
	$(CC) -c -g -Wall $(CFLAGS) $?

clean:
	rm -f *.o
	rm -f a.out

.PHONY: all clean
