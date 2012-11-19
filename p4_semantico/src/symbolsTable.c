#include "symbolsTable.h"

// Definiciones de funciones para tratar la tabla de simbolos.

//Método para insertar un nodo que contiene un símbolo
Symbol* insert( struct Symbol* actual, struct Symbol *Symb, int type ) // He cambiado el & por * en el segundo argumento por que daba error (en C no habian referencias).
{
	//Si es nulo lo metemos y ya
	
	//Si no es nulo tenemos que desplazar la lista e insertar el nuevo nodo
	Symbol* aux;
	aux = actual;
	Symb.prev = aux;
	return &Symb;
}
