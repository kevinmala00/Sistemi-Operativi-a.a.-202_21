CC=gcc
CFLAGS=-Wall -std=c11
FILE=padreFiglioConStatus myGrepConFork-ridStError myGrepConFork-ridStErrorEInput padreFigliMultipli padreFigliConSalvataggioPID padreFigliConConteggioOccorrenze

all: $(FILE)

padreFiglioConStatus: $@.c
	$(CC) $(CFLAGS) -o $@ $<

myGrepConFork-ridStError: $@.c
	$(CC) $(CFLAGS) -o $@ $<

myGrepConFork-ridStErrorEInput: $@.c
	$(CC) $(CFLAGS) -o $@ $<

padreFigliMultipli: $@.c
	$(CC) $(CFLAGS) -o $@ $<

padreFigliConSalvataggioPID: $@.c
	$(CC) $(CFLAGS) -o $@ $<

padreFigliConConteggioOccorrenze: $@.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -rf *.o
