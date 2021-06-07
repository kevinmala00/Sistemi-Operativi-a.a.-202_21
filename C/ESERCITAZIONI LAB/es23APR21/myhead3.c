#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

void myhead(int fd, int numlines){
	char car;
	int count =1;
	char *str = malloc(sizeof(*str));
	int lung;
	while(count<=numlines){	int last;
		 for(int i=0; read(fd,&car,1); i++){
			if(car=='\n'){
				break;
			}
		 	lung = strlen(str);
		 	str = realloc(str, lung + 1);
		 	str[i]=car;
		 	last=i;
			
		 }
		 str[last+1]='\0';
		 puts(str);
		 str = calloc(1,sizeof(*str)); 
		 count++;
	}
	free(str);
}	


int main(int argc, char **argv){

	int fd, nl;
	if(argc -1 >3){
		printf("devi passare massimo 2 parametri\n");
		exit(1);
	}

	if(argc-1 == 0){
		nl=10;
	}

	if(argc-1== 1){
		nl = atoi(argv[1]);
		fd=0;
	}

	if(argc-1 == 2){
		fd=open(argv[2],O_RDONLY);
	 	if(fd<0){
			printf("errore: file non esistente o errore nell'apertura\n");
			exit(2);
		}
		nl = atoi(argv[1]);
	}
	myhead(fd, nl);
	return 0;
}

