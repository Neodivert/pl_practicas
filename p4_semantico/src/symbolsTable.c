#include "symbolsTable.h"

// Definiciones de funciones para tratar la tabla de simbolos.

//Método para insertar un nodo que contiene un símbolo
/*Symbol* insert( struct Symbol* actual, struct Symbol *Symb, int type ) // He cambiado el & por * en el segundo argumento por que daba error (en C no habian referencias).
{
	//Si es nulo lo metemos y ya
	if(actual == NULL){
		symTable = actual;
		actual->prev = NULL;
	}else{
		//Si no es nulo tenemos que desplazar la lista e insertar el nuevo nodo
		
		Symbol* aux;
		aux = actual;
		Symb.prev = aux;
		return &Symb;
	}
		
}*/

void insertSimbol(struct Symbol *symb){
	if(symTable == NULL){ 
		symTable = symb;
		symTable->prev = NULL;	
	}else{
		symb->prev = symTable;
		symTable = symb;	
	}		
}

void freeSymbTable(){
	struct Symbol *aux;
	aux = symTable;
	while(aux != NULL){
		symTable = aux->prev;
		free((struct Type*)aux->info);
		free(aux);
		aux = symTable;
	} 
}
