CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)

all: dcontrol

dcontrol: desktop_control.c
	$(CC) $(CFLAGS) -o dcontrol desktop_control.c $(LDFLAGS)

clean:
	rm -f dcontrol
