#!/bin/sh
#file:FCR.sh relativo alla prova in iteinre del 22/04/2021
#sintassi: $0 <ger> <string> <filetemp>


#mi sposto nella directory da analizzare contenuta in $1
cd $1


#variabile su cui salvo se trovato un file
trovato=0

#inizio i controllo richiesti, controllo il nome di ogni file deve terminare con .$2
for K in *
do 
	if test -f $K
	then #è un file controllo il no
		case $K in
			*.$2) trovato=1;; #dico che ho trovato un fiele corrispondente
			*);; #non faccio nulla
		esac
	fi
done

if test $trovato -eq 1
then
	echo "`pwd`" >>$3
fi


#chiamata ricorsiva su ogni sottodirectory
for dir in *
do 
	if test -d $dir -a -x $dir
	then
		$0 `pwd`/$dir $2 $3
	fi
done

