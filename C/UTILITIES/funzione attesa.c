/*funzione di attesa
    @param
        const char *str: contiene il livello del processo che si sta chiudendo (es: "figlio", "nipote"...)
        int indice: contiene l'indice del processo che si sta aspettando, se è negativo allora vi è un solo processo
    @return 
        int : che corrisponde al valore ritornato dal processo in chiusura
    */
int waiting(const char *str, int index){
        /*contiene i 2 bytes ritornati dal processo atteso con la wait*/
        int status;

        /*contiene il valore di ritorno del processo appena terminato*/
        int ritorno;
    	int pid = wait(&status);
		if (pid < 0){
		    printf("Errore: problemi nella wait di attesa dei processi con gerarchia %s\n", str);
            if(!strcmp(str,"figlio")){
                exit(5);
            }
            if(!strcmp(str,"nipote")){
                exit(-1);
            }
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    	else{ 
            ritorno=(int)((status >> 8) &	0xFF);
            if(index<0){
                if(ritorno==255){
                printf("attenzione ci sono stati dei problemi durante l'esecuzione del processo %s\n");
                }
                printf("Il %s con pid=%d ha ritornato %d\n", str, pid, ritorno);
            }else{
                if(ritorno==255){
                printf("attenzione ci sono stati dei problemi durante l'esecuzione del processo %s con indice %d\n", str, index);
                }
                printf("Il %s di indice %d con pid=%d ha ritornato %d\n", str, index, pid, ritorno);
            }
        }
        return ritorno;
}
