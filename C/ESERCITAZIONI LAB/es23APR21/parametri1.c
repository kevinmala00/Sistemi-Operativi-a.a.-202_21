#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){

if(argc -1 < 1){
    printf("errore: devi passare almeno 1 parametro\n");
    exit(1);
}
printf("file: %s, \nnumero di parametri: %i\n", argv[0], argc);

for(int i=1; i <= argc; i++){
    printf("parametro %i: %s\n", i, argv[i]);
}

return EXIT_SUCCESS;

}
