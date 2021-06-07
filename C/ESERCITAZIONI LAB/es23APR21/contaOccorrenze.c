#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

long contaoccorrenze(int fd, char c){
    char buffer;
    long occ=0;
    int nread;
    while(read(fd, &buffer, 1)){
        if(buffer == c){
        occ++;
        }
    }
    return occ;
}


int main(int argc, char **argv){

    if(argc-1!=2){
        printf("errore: devi passare esattametente 2 parametri\n");
        exit(1);
    }

    int fd;
    if ((fd=open(argv[1], O_RDONLY)<0)){
        printf("errore: il primo paramtro deve essere un file\n");
        exit(2);
    }

    if(argv[2][1]!='\0'){
        printf("errore il secondo paraetro deve esseere un singolo carattere\n");
    }
    printf("eseguibile: %s\n", argv[0]);
    printf("numero di parametri passati: %i\n", argc);
    for(int i=1; argv[i]!=NULL; i++){
        printf("parametro %i: %s\n", i, argv[i]);
    }    
    char c=argv[2][0];
    long occ = contaoccorrenze(fd,c);

    printf("trovate %ld occorrenze\n", occ);

    close(fd);

    return EXIT_SUCCESS;

}
