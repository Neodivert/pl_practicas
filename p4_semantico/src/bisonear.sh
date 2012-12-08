#!/bin/bash
# Script que se usa de la forma
#                  ./bisonear.sh <fichero_sintactico> <fichero_lexico>
# Trata y compila los ficheros sintactico y lexico

# Comprueba que se pasan los dos parámetros necesarios.
if (( $# != 2 )); then
   echo "ERROR: Se requiere dos argumento (fichero sintáctico (.y) y fichero lexico (.l)" >&2
   exit -1
fi

# A partir del nombre del fichero sintáctico se genera:
codigo_analizador=${1}
analizador=$( echo ${codigo_analizador%.*} )	# El nombre del ejecutable.
tablas=$analizador.tab.c			# El nombre del .c de las tablas.
include_tablas=$analizador.tab.h		# El nombre del .h de las tablas.

# En el fichero léxico se hace un include del .h de tablas. Se sustituye 
# el fichero del include por el del analizador sintáctico actual y se trata 
# con flex.
sed -r -i "s/#include.*/#include \"$include_tablas\"/" ${2}
flex ${2}

# Se trata el fichero sintáctico con bison y se genera el analizador final.
bison -vdt $codigo_analizador
gcc -o $analizador $tablas lex.yy.c symbolsTable.c semanticUtilities.c codegenUtils.c -lfl
