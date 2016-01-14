#!/bin/bash

#SCRIPT DE COMPROBACIÓN DE LA PRÁCTICA 3

# Variable de permanencia en el bucle
stay=true

# Ejecuto el bucle mientras el fichero ejemplo no sea correcto
while $stay
do

# Se pregunta al usuario que ejemplo desea simular
	read -p "Introduzca el nombre del ejemplo que desea simular: " ejemplo

# Comprobamos si el fichero existe y es un fichero regular
	if [ -f schedsim/examples/$ejemplo ] ; then
		stay=false
	else
		echo "El fichero especificado no existe o no es un fichero regular"
	fi

done # fin del bucle

stay=true

# Ejecuto el bucle mientras el numero de CPUs no sea correcto
while $stay
do

# Se pregunta al usuario por el número de CPUs
	read -p "Introduzca el numero de CPUs que desea usar en la simulacion: " maxCPUs

	echo $maxCPUs
# Comprobamos si el número es > 0 y <= 8
	if [ $maxCPUs -gt 0 ] && [ $maxCPUs -le 8 ] ; then
		stay=false
		
	else
		echo "El número de CPUs no es correcto"
	fi

done # fin del bucle


# Si la carpeta ya existe, la borramos
if [ -d resultados ] ; then
	rm -r resultados
fi

# Creamos el directorio "resultados" de nuevo
mkdir ./resultados


# Declaramos una lista con todos los Schedulers
listaDeSchedulersDisponibles=("RR" "SJF" "FCFS" "PRIO")

for nameSched in ${listaDeSchedulersDisponibles[@]} 
do

	for ((cpus=1; cpus<=$maxCPUs; cpus++)) 
	do
	
	# Se ejecutan los schedulers para el ejemplo introducido por el usuario
	echo " "
	echo "##############################################################"
	echo "##############################################################"
	echo " "
	echo "Se va a ejecutar" $ejemplo "con el planificador" $nameSched "con" $cpus "CPUs"
	echo " "
	
		./schedsim/schedsim -n $cpus -i schedsim/examples/$ejemplo -s $nameSched
		
	# Añadimos una pausa para comprobar que lo ha ejecutado bien
	read -p "Presiona [Enter] para continuar..."
	
		for ((i=0; i<=$cpus-1; i++))
		do
		
		echo " "
		echo "Se va a mover el fichero CPU_"$i".log al directorio resultados/"
		echo " "
			
			mv CPU_$i.log resultados/$nameSched-CPU-$i.log 
			
		# Añadimos una pausa para comprobar que lo ha ejecutado bien
		read -p "Presiona [Enter] para continuar..."
	
		done
		
		clear

		# Nos movemos al directorio gantt-plot
		cd gantt-gplot
		
		for ((i=0; i<=$maxCPUs-1; i++)) # Para todas las CPUs
		do
		
		echo " "
		echo "##############################################################"
		echo "##############################################################"
		echo " "
		echo "Se van a generar las graficas para" $nameSched"-CPU-"$i".log"
		echo " "
		
			./generate_gantt_chart ../resultados/$nameSched-CPU-$i.log
			
		done
		
		# Añadimos una pausa para comprobar que lo ha ejecutado bien
		echo " "
		echo "Grafica para scheduler" $nameSched "completada"
		read -p "Presiona [Enter] para continuar..."

		cd ..
		
	done
done





























