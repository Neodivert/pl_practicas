#!/bin/bash
# Script que "bisonea" (compila) el compilador, compila la máquina Q y ejecuta
# un fichero de prueba.

# Comprueba que se pasan los parámetros necesarios.
if (( $# != 1 )); then
   echo "ERROR: Se requiere un argumento (programa .em)" >&2
   exit 1
fi

# "Bisonea" (compilador) el compilador.
./bisonear.sh sintax.y lexicon.l
if (( $? != 0 )) ; then
	echo "ERROR bisoneando" >&2
	exit 1
fi

# Compila el fichero de prueba.
./sintax $1 foo &> out
if (( $? != 0 )) ; then
	echo "ERROR sintacticando" >&2
	exit 1
fi

# Compila la maquina Q.
gcc IQ.o Qlib.c -o IQ
if (( $? != 0 )) ; then
	echo "ERROR IQuando" >&2
	exit 1
fi

# Ejecuta el programa de prueba en la maquina Q.
./IQ -g ${1%.*}.q.c
