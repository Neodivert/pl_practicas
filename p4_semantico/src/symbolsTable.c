#include "symbolsTable.h"
#include <stdio.h>
#include <assert.h>

//#define DEBUG 1

/* 
If nextSymIsFirstChild is 1, next time insertSymbol is invoked, the new symbol 
is inserted as the child of top symbol.
This variable is set to 1 after inserting a Method in symTable, and it's set to 
0 when a new symbol is inserted (local data) or "GoOutOfScope()" is invoked 
(we go out of method scope).
*/
static char nextSymIsFirstChild = 0;

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

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

struct Symbol* createSymbol ( int symType, const char* const  name )
{
	struct Symbol *symbol = (Symbol *)malloc( sizeof( struct Symbol ) );
	int n = strlen( name );

	symbol->symType = symType;
	symbol->name = (char *)malloc( n+1 );
	strcpy( symbol->name, name );

	symbol->firstChild = 0;
	symbol->prev = NULL;
	symbol->next = NULL;

	return symbol;
}

void insertSymbol(struct Symbol *symb)
{
	if(symTable == NULL){ 
		symTable = symb;
		symTable->prev = NULL;	
	}else{
		if( nextSymIsFirstChild ){
			printf( "\t\t\tPRIMER HIJO\n" );
			symb->firstChild = 1;
			// CAMBIAR ASSERT CUANDO SE INCLUYAN CLASES Y BLOQUES.
			assert( symTable->symType == SYM_METHOD );
			((struct Method *)(symTable->info))->localSymbols = symb;
			symb->prev = symTable;
		}else{
			printf( "Insertando %s, nodo anterior: %s\n", symb->name, symTable->name );
			symb->prev = symTable;
			symTable->next = symb;
		}
		symTable = symb;
	}
	nextSymIsFirstChild = 0;
}


/*                       2. Specific symbols insertion                        */

void insertMethodDefinition( const char* const name  )
{
	struct Symbol* symbol = createSymbol( SYM_METHOD, name );

	symbol->info = (void *)malloc( sizeof( struct Method ) );

	((struct Method *)(symbol->info))->nArguments = 0;
	((struct Method *)(symbol->info))->localSymbols = NULL;
	
	insertSymbol( symbol );

	// If we don't go out of scope, next symbol will be a "child".
	nextSymIsFirstChild = 1;

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
	((struct Variable *)(symbol->info))->type = (void *)type;

	insertSymbol( symbol );
}


/*                             3. Symbol search                               */

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


struct Symbol* searchVariable(  int symType, const char* const name )
{
	struct Symbol* s = symTable;
	while( s != NULL ){
		if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
			return s;
		}

		s = s->prev;
	}

	showSymTable( symTable, 0 );

	printf( "Variable %s no encontrada \n", name );
	return NULL;
}


/*                       4. Symbols table management                          */

void initializeSymTable()
{
	insertMethodDefinition( "_MAIN" );
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );

	// A partir de aqui es para hacer pruebas. BORRAR mas adelante.
	insertMethodDefinition( "_METHOD2" );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
	insertMethodDefinition( "_METHOD3" );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	goOutOfScope();
	goOutOfScope();
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
	insertMethodDefinition( "_METHOD3" );
	insertMethodDefinition( "_METHOD4" );
	goOutOfScope();
	insertMethodDefinition( "_METHOD5" );
	insertMethodDefinition( "_METHOD6" );
	goOutOfScope();
	goOutOfScope();
	insertMethodDefinition( "_METHOD7" );
}

void showSymTable( struct Symbol* sym, int level )
{
	if( sym == NULL ) return;
	int i = 0;
	if( level == 0 ){
		printf( "Symbols table -------------------------------\n" );

		// Start showing from the beginning.
		while( sym->prev ){
			sym = sym->prev;
		}
	}

	while( (sym != NULL) ){
		for( i=0; i<level; i++ ) printf( "\t" );
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

		
		printf( " - name: [%s]", sym->name );

		#ifdef DEBUG

		if( sym->prev ){
			printf( " - prev: [%s]", sym->prev->name );
		}else{
			printf( " - prev: [NULL]" );
		}

		if( sym->next ){
			printf( " - next: [%s]", sym->next->name );
		}else{
			printf( " - next: [NULL]" );
		}

		#else
			printf( "\n" );
		#endif

		if( ( sym->symType == SYM_METHOD ) && ((struct Method*)(sym->info))->localSymbols ){
			#ifdef DEBUG
			printf( " - hijo: [%s]\n",((struct Method*)(sym->info))->localSymbols->name );
			#endif 
			//printf( "\n\t%s\n", ((struct Method*)(sym->info))->localSymbols->name );
			showSymTable( ((struct Method*)(sym->info))->localSymbols, level+1 );
			
		}
		#ifdef DEBUG
		else{
			printf( " - hijo: [NULL]\n" );
		}
		#endif
	
		sym = sym->next;
	}
	
	if( level == 0 ){
		printf( "---------------------------------------------\n" );
	}
}

void freeSymbol(struct Symbol* symbol)
{
	printf( "Eliminando simbolo: [%s]\n", symbol->name );
	free(symbol->name);
	free(symbol->info);
	free(symbol);
}


void freeSymbTable_( struct Symbol* symTable_ ){
	struct Symbol *aux = symTable_;
	while( aux ){
		symTable_ = aux->next;
		if( aux->symType == SYM_METHOD ){
			freeSymbTable_( ((struct Method *)(aux->info))->localSymbols );
		}
		freeSymbol( aux );
		aux = symTable_;
	}
}

void freeSymbTable(){
	struct Symbol *aux, *sym = symTable;

	if( !sym ) return;

	while( sym->prev ){
		sym = sym->prev;
	}

	freeSymbTable_( sym );
}


/*                                  5. Others                                 */

void goOutOfScope(){
	if( nextSymIsFirstChild ){
		nextSymIsFirstChild = 0;
	}else{
		while( symTable != NULL && !symTable->firstChild ){
			symTable = symTable->prev;
		}

		if( symTable ){
			symTable = symTable->prev;
		}
	}
}


/*
int main( int argc, char *argv[] )
{
	initializeSymTable();
	//insertTypeDefinition( "class" );

	struct Symbol* s = searchType( TYPE_INTEGER );
	printf( "%s\n", s->name );

	//insertMethodDefinition( "abc" );
	//insertMethodDefinition( "abc2" );
	//insertMethodDefinition( "xyz" );
	showSymTable( symTable, 0 );

	freeSymbTable();



	return 0;
	
}
*/

