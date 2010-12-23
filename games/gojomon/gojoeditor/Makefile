OBJS=main.o window.o draw.o toolbar.o fileops.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -Wall
LIBS=`pkg-config --libs gtk+-2.0`
PROG=gjmedit

all:$(OBJS)
	gcc -o $(PROG) $(LIBS) $(OBJS)

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(PROG) $(OBJS)
