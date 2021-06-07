CC=gcc
CFLAGS=-Wall -std=c11 
file=parametri1 parametri2 contaOccorrenze 22sett99-1 selezionaMultipli myhead3 selezionaLinea selezionaLunghezzaLinea

all: $(file)

parametri1: parametri1.c
	$(CC) $(CFLAGS) -o $@ $<
	
parametri2: parametri2.c
	$(CC) $(CFLAGS) -o $@ $<

contaOccorrenze: contaOccorrenze.c
	$(CC) $(CFLAGS) -o $@ $<

22set99-1:22set99-1.c
	$(CC) $(CFLAGS) -o $@ $<

selezionaMultipli: selezionaMultipli.c
	$(CC) $(FLAGS) -o $@ $<

myhead3: myhead3.c
	$(CC) $(CFLAGS) -o $@ $<

selezionaLinea: selezionaLinea.c
	$(CC) $(CFLAGS) -o $@ $<

selezionaLunghezzaLinea: selezionaLunghezzaLinea.c 
	$(CC) $(CFLAGS) -o $@ $<
	
clean:
	rm -rf *.o
