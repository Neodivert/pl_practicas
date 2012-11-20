#include "symbolsTable.h"
#include <stdio.h>

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


void insertMethodDefinition( const char* const name  )
{
	struct Symbol* symbol = createSymbol( SYM_METHOD, name );

	insertSymbol( symbol );

	//symbol->info = (void *)malloc( sizeof( struct Method ) );
}

void insertTypeDefinition( const char* const name, int typeId )
{	
	printf( "Insertando tipo %i\n", typeId );
	struct Symbol* symbol = createSymbol( SYM_TYPE, name );

	symbol->info = (void *)malloc( sizeof( struct Type ) );
	
	((struct Type *)(symbol->info))->id = typeId;

	insertSymbol( symbol );
}

struct Symbol* searchType( int typeId )
{
	struct Symbol* s = symTable;
	while( s != NULL ){
		if( s->symType == SYM_TYPE ){
			if( ((struct Type *)(s->info))->id == typeId ){
				return s;
			}
		}

		s = s->prev;
	}

	showSymTable();

	printf( "\n\n\n\n\n NULLAZO AL CANTO (%d)\n\n\n\n\n", typeId );
	return NULL;
}



void showSymTable()
{
	printf( "Symbols table -------------------------------\n" );
	struct Symbol* s = symTable;
	while( s != NULL ){
		switch( s->symType ){
			case SYM_TYPE:
				printf( "TYPE" );
			break;
			case SYM_VARIABLE:
				printf( "VARIABLE" );
			break;
			case SYM_CONSTANT:
				printf( "CONSTANT" );
			break;
			case SYM_METHOD:
				printf( "METHOD" );
			break;
			case SYM_BLOCK:
				printf( "BLOCK" );
			break;
			default:
				printf( "UKNOWN TYPE" );
			break;
		}
		printf( " - name: [%s]\n", s->name );
	
		s = s->prev;
	}
	printf( "---------------------------------------------\n" );
}

struct Symbol* createSymbol ( int symType, const char* const  name )
{
	struct Symbol *symbol = (Symbol *)malloc( sizeof( struct Symbol ) );
	int n = strlen( name );
	
	symbol->symType = symType;
	symbol->name = (char *)malloc( n+1 );
	strcpy( symbol->name, name );

	return symbol;
}

void insertSymbol(struct Symbol *symb){
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


/*
int main( int argc, char *argv[] )
{
	
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
	//insertTypeDefinition( "class" );

	struct Symbol* s = searchType( TYPE_INTEGER );
	printf( "%s\n", s->name );

	insertMethodDefinition( "abc" );
	insertMethodDefinition( "abc2" );
	insertMethodDefinition( "xyz" );
	showSymTable();
	return 0;
	
}
*/
