#!/bin/bash
if [[ "$1" = *".l" ]]
then
	echo "File argument is without extension" >&2
	exit
fi
extension=".l" 
flex "$1"$extension
gcc -o "$1" lex.yy.c -lfl

