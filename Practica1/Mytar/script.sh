#!/bin/bash

make;
make clean;

#if !(locate mytar.c);
#	then 
#		echo "El fichero no existe en el directorio";
#fi

if test -d tmp; 
	then rm -r tmp;
fi

mkdir tmp;

cd tmp;

echo "Hola Mundo" > file1.txt;
head -n 10 /etc/passwd  > file2.txt;
head -c 1024 /dev/urandom > file3.dat;

./../Ejemplo -cf m.mtar file1.txt file2.txt file3.dat;

mkdir out;

cp m.mtar out/m.mtar;

cd out;

./../../Ejemplo -xf m.mtar;
./../../Ejemplo -tf m.mtar;

cd ..;

if ! test -e file1.txt || ! diff file1.txt out/file1.txt;
	then
		echo "Error en la extraccion de file1.txt";
fi

if ! test -e file2.txt || ! diff file2.txt out/file2.txt;
	then
		echo "Error en la extraccion de file2.txt";
fi

if ! test -e file3.dat || ! diff file3.dat out/file3.dat;
	then
		echo "Error en la extraccion de file3.dat";
fi

cd ..;