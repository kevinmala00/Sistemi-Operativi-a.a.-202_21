CC=gcc
CFLAGS=-Wall -std=c11
file=provaPipe pipe-newSenzaClose pipe-newGenerico1 pipe-newGenerico2 partec191201-b

all: $(file)

provaPipe:provaPipe.c
	$(CC) $(CFLAGS) -o $@ $<

pipe-newSenzaClose:pipe-newSenzaClose.c
	$(CC) $(CFLAGS) -o $@ $<

pipe-newGenerico1:pipe-newGenerico1.c
	$(CC) $(CFLAGS) -o $@ $<

pipe-newGenerico2:pipe-newGenerico2.c
	$(CC) $(CFLAGS) -o $@ $<

partec191201-b:partec191201-b.c
	$(CC) $(CFLAGS) -o $@ $<

clear:
	rm -rf *.o
