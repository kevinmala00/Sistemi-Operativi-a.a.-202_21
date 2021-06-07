#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char **argv){
if(argc - 1 != 3){
    printf("errore: devi passare esattamente 3 parametri\n");
    exit(1);
}
int fd;
if((fd = open(argv[1], O_RDONLY) < 0)){
    printf("errore: il primo parametro deìve essere un file\n");
    exit(2);
}
if(argv[1]-'0' <= 0){
    printf("errore: il secondo parametro deve essere maggiore di 0\n");
    exit(3);
}
if(argv[2][1] != '\0'){
    printf("errore: devi passare un unico carattere\n");
    exit(3);
}
printf("file: %s\n", argv[0]);
printf("numero di parateri: %i\n", argc);

for(int i=1; argv[i]!=NULL; i++){
    printf("parametro %i: %s\n", i, argv[i]);
}
close(fd);
return EXIT_SUCCESS;
}
