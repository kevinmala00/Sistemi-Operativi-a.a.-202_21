#!/bin/sh
#file: FCP.sh della prova in itinerem del 22/04/2021
#sintassi: FCP.sh 

#la riga successiva è stata utilizzata per il debug 
#in caso si vuole testare lo script eliminare il carattere # dalla linea successiva
#set 2 txt /home/n269790/ger22APR21_ITINERE /home/n269790/ger22APR21_ITINERE

#controllo numero dei parametri
case $# in
	0|1|2|3) echo "errore devi passare almeno 4 parametri"; exit 1;;
	*) echo "numero di parametri passati" ;;
esac



#controllo tipo dei parametri
#controllo il primo parametro
case $1 in 
	*[!0-9]*) echo "errore il parametro deve essere un caratttere numerico"
		exit 2;;
	*) #deve essere strettamente positivo
		if test $1 -eq 0
		then 
			echo "errore il parametro deve essere maggiore di 0"
			exit 3
		fi;;
esac

#salvo il parametro 
Y=$1

shift
#controllo il secondo parametro
if test $1
then 
	w=$1
fi

shift
#controllo i restanti parametri 
for ger in $*
do
	case $ger in
		/*) #nomeassoluto controllo se traversabile e se esiste
			if test ! -d $ger -o ! -x $ger
			then
				echo "errore :directory non esistente o non traversabile"
				exit 5
			fi;;
		*) echo "eroore devi passare le gerarchia con nome assoluto"
		       exit 4;;
       esac
done       

#creo il file temporaneo il cui nome assoluto è salvato nella variabile file
> /tmp/Temporaneo


#esporto il path
PATH=`pwd`:$PATH
export PATH

#chiamata ricorsiva
for dir in $*
do 
    FCR.sh $dir $w "/tmp/Temporaneo"
done

#terminate le ricerche ricorsive
n=`wc -l </tmp/Temporaneo`
echo "trovati $n elementi corrispondenti alle specifiche"

if test $n -gt $Y
then
	echo -n "kevin inserisci il numero maggiore di 1 e minore di  $Y della directory di cui vuoi visualizzare il nome assoluto\t"
	read J
	case $J in 
		*[!0-9]*) echo "errore il parametro deve essere un caratttere numerico"
			exit 5;;
		*) #deve essere strettamente positivo
			if test $1 -eq 0
			then 
				echo "errore il parametro deve essere maggiore di 0"
				exit 6
			fi;;
	esac
	if test $J -ge 1 -a $J -le $Y
	then
		echo "nome: `head -$J </tmp/Temporaneo|tail -1`"
	fi
fi

#rimuovo il file temporaneo
rm /tmp/Temporaneo
