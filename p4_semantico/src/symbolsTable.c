#include "symbolsTable.h"
#include <stdio.h>
#include <assert.h>

// If firstChild is 1, next time insertSymbol is invoked, the new symbol is
// inserted as the child of top symbol.
// This variable is set to 1 after inserting a Method in symTable, and it's
// set to 0 when a new symbol is inserted (local data) or "GoOutOfScope()" is 
// invoked (we go out of method scope).
static char firstChild = 0;

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

	symbol->info = (void *)malloc( sizeof( struct Method ) );

	((struct Method *)(symbol->info))->nArguments = 0;
	((struct Method *)(symbol->info))->localSymbols = NULL;
	
	insertSymbol( symbol );

	// If we don't go out of scope, next symbol will be a "child".
	firstChild = 1;

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

void insertVariable( struct Symbol *symbol, struct Symbol *type )
{	
	symbol->info = (void *)malloc( sizeof( struct Variable ) );
	((struct Variable *)(symbol->info))->type = (void *)type;

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

	showSymTable( symTable, 0 );

	printf( "\n\n\n\n\n NULLAZO AL CANTO (%d)\n\n\n\n\n", typeId );
	return NULL;
}


struct Symbol* searchVariable( const char* const name )
{
	struct Symbol* s = symTable;
	while( s != NULL ){
		if( s->symType == SYM_VARIABLE && (strcmp(s->name, name) == 0)  ){
			return s;
		}

		s = s->prev;
	}

	showSymTable( symTable, 0 );

	printf( "Variable %s no encontrada \n", name );
	return NULL;
}

void showSymTable( struct Symbol* sym, int level )
{
	int i = 0;
	if( level == 0 ){
		printf( "Symbols table -------------------------------\n" );
	}

	for( ; i<level*3; i++ ){
		printf( "\t" );
	}

	while( sym != NULL && !sym->firstChild ){
		switch( sym->symType ){
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
		printf( " - name: [%s]\n", sym->name );

		if( ( sym->symType == SYM_METHOD ) && ((struct Method*)(sym->info))->localSymbols ){
			showSymTable( ((struct Method*)(sym->info))->localSymbols, level+1 );
		}
	
		sym = sym->prev;
	}
	
	if( level == 0 ){
		printf( "---------------------------------------------\n" );
	}
}

struct Symbol* createSymbol ( int symType, const char* const  name )
{
	struct Symbol *symbol = (Symbol *)malloc( sizeof( struct Symbol ) );
	int n = strlen( name );

	symbol->symType = symType;
	symbol->name = (char *)malloc( n+1 );
	strcpy( symbol->name, name );

	symbol->firstChild = 0;

	return symbol;
}

void insertSymbol(struct Symbol *symb){
	if( firstChild ){
		symb->firstChild = 1;
		// CAMBIAR ASSERT CUANDO SE INCLUYAN CLASES Y BLOQUES.
		assert( symTable->symType == SYM_METHOD );
		((struct Method *)(symTable->info))->localSymbols = symb;
	}
	if(symTable == NULL){ 
		symTable = symb;
		symTable->prev = NULL;	
	}else{
		symb->prev = symTable;
		symTable = symb;	
	}
	firstChild = 0;
}

void freeSymbol(struct Symbol* symbol)
{
	free(symbol->info);
	free(symbol);
}

void freeSymbTable(){
	struct Symbol *aux;
	aux = symTable;
	while(aux != NULL){
		symTable = aux->prev;
		free(aux->info);
		free(aux);
		aux = symTable;
	}
}

void GoOutOfScope(){
	while( symTable != NULL && !symTable->firstChild ){
		symTable = symTable->prev;
	}

	if( symTable ){
		symTable = symTable->prev;
	}
}

void initializeSymTable()
{
	insertMethodDefinition( "_MAIN" );
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
}

int main( int argc, char *argv[] )
{
	//insertMethodDefinition( "_main" );
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
	//insertTypeDefinition( "class" );

	struct Symbol* s = searchType( TYPE_INTEGER );
	printf( "%s\n", s->name );

	//insertMethodDefinition( "abc" );
	//insertMethodDefinition( "abc2" );
	//insertMethodDefinition( "xyz" );
	showSymTable( symTable, 0 );
	return 0;
	
}

