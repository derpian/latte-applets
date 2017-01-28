prefix = /usr

CX = g++
CC = gcc

CFLAGS = -Wall			 	\
	-DG_DISABLE_DEPRECATED 	 	\
	-DGDK_DISABLE_DEPRECATED 	\
	-DGDK_PIXBUF_DISABLE_DEPRECATED \
	-DGTK_DISABLE_DEPRECATED

GKT =  `pkg-config gtk+-3.0 --cflags --libs`

GKG =  `pkg-config glib-2.0 --cflags --libs`

all: menu

menu: main.c
	$(CC) main.c -o latte-menu $(CFLAGS) $(GKT)

clean:
	rm -f *.o latte-menu

distclean:
	rm -f *.o latte-menu

install:
	mkdir -p $(DESTDIR)/${prefix}/bin
	cp latte-menu $(DESTDIR)/${prefix}/bin
	mkdir -p $(DESTDIR)/${prefix}/share/icons
	cp icons/* $(DESTDIR)/${prefix}/share/icons
	mkdir -p $(DESTDIR)/${prefix}/share/latte
	cp latte/* $(DESTDIR)/${prefix}/share/latte
