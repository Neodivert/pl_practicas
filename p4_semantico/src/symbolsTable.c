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

// Fast access to set the method's number of arguments. 
struct Method* lastDefinedMethod = NULL;

// Symbols tree's root.
static Symbol* symTable = NULL;


/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

struct Symbol* createSymbol ( int symType, const char* const  name )
{
	// Allocate memory for new symbol.
	struct Symbol *symbol = (Symbol *)malloc( sizeof( struct Symbol ) );

	// Set symbol's type and name.
	symbol->symType = symType;
	symbol->name = (char *)malloc( strlen( name )+1 );
	strcpy( symbol->name, name );

	// Initialize symbol's "firstChild" field and pointers. 
	symbol->firstChild = 0;
	symbol->prev = NULL;
	symbol->next = NULL;

	return symbol;
}

void insertSymbol( struct Symbol *symb )
{
	assert( symb != NULL );

	if( symTable == NULL ){
		// First symbol added to symbols table.
		symTable = symb;
		symTable->prev = NULL;	
	}else{
		// Not the first symbol added to symbols table.
		if( nextSymIsFirstChild ){
			// This symbol is the first child of the symbols table top.
			symb->firstChild = 1;

			((struct Method *)(symTable->info))->localSymbols = symb;
			symb->prev = symTable;
			printf( "Insertando %s (primer hijo), nodo anterior: NULL\n", symb->name, symTable->name );
		}else{
			// This symbols is the brother of the symbols table top.
			printf( "Insertando %s, nodo anterior: %s\n", symb->name, symTable->name );
			symb->prev = symTable;
			symTable->next = symb;
		}
		symTable = symb;
	}
	nextSymIsFirstChild = 0;
}

struct Symbol* getCreateVariable( int symType, const char* const name)
{
	struct Symbol* variableStruct = searchVariable( symType, name );
	if( variableStruct == NULL)
	{
		variableStruct = createSymbol( symType, name );
		variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
		((struct Variable *)(variableStruct->info))->type = NULL;
		//variableStruct->info = NULL;
	}
	return variableStruct;
}

/*                       2. Specific symbols insertion                        */

void insertMethodBlockDefinition_( Symbol* symbol )
{
	symbol->info = (void *)malloc( sizeof( struct Method ) );

	if( symbol->symType == SYM_BLOCK ){
		// We only allow the block "each", which only has one mandatory 
		// argument.
		((struct Method *)(symbol->info))->nArguments = 1;
	}else{
		// This will be filled when method's argument is read.
		((struct Method *)(symbol->info))->nArguments = 0;

		// When the final argument is declared, we'll use this pointer to access
		// this method and fill its nArgument field.
		lastDefinedMethod = ((struct Method *)(symbol->info));
	}
	((struct Method *)(symbol->info))->localSymbols = NULL;

	// Insert method's symbol in table.
	insertSymbol( symbol );

	// If we don't go out of scope, next symbol will be a "child".
	nextSymIsFirstChild = 1;
}

void insertMethodDefinition( const char* const name  )
{
	// Create and fill the method's symbol.
	struct Symbol* symbol = createSymbol( SYM_METHOD, name );

	// Fill and insert the method's symbol.
	insertMethodBlockDefinition_( symbol );
}

void insertBlockDefinition( const char* const argName  )
{
	printf( "Insertando bloque con variable [%s]\n", argName );

	// Create and fill the block's symbol.
	struct Symbol* symbol = createSymbol( SYM_BLOCK, "_block" );

	// Fill and insert the block's symbol.
	insertMethodBlockDefinition_( symbol );

	// Insert the block's argument.
	insertVariable( getCreateVariable( SYM_VARIABLE, argName ), NULL );
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

// FALTA: Hay que hacer que si estamos buscando en un bocle, no se tiene acceso
// a las variables globales.y en el método exterior.
struct Symbol* searchVariable( int symType, const char* const name )
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
	insertMethodDefinition( "_main" );
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
}

// Auxiliar function. Show symbols from sym, and tabulate them according to its 
// their level.
void showSymTable_( struct Symbol* sym, int level )
{
	int i = 0;

	while( (sym != NULL) ){
		// Tabulate current symbol.
		for( i=0; i<level; i++ ) printf( "\t" );

		// Show current symbol's name.
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

		printf( " - [%s]", sym->name );

		// If DEBUG is defined, show previous and next symbol's name.
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
		#endif

		// Show extra info according to the current symbol's type.
		Symbol* aux;
		switch( sym->symType ){
			case SYM_VARIABLE:
				aux = ((struct Variable*)(sym->info))->type;
				printf( " - type: " );
				if( aux ){
					printf( "[%s]\n", aux->name );
				}else{
					printf( "NULL\n" );
				}
			break;
			case SYM_METHOD:
			case SYM_BLOCK:
				aux = ((struct Method*)(sym->info))->localSymbols;

				printf( " - nArguments: [%i]", ((struct Method*)(sym->info))->nArguments );
				if( aux ){
					printf( " - hijo: [%s]\n", aux->name );
					showSymTable_( aux, level+1 );
				}else{
					printf( " - hijo: [NULL]\n" );
				}
			break;
			default:
				printf( "\n" );
			break;
		}

		sym = sym->next;
	}
}


void showSymTable()
{
	Symbol* sym = symTable;

	printf( "Symbols table -------------------------------\n" );

	// Start showing from the beginning.
	while( sym->prev ){
		sym = sym->prev;
	}

	if( symTable ){
		showSymTable_( sym, 0 );
	}

	printf( "---------------------------------------------\n" );
}

void freeSymbol(struct Symbol* symbol)
{
	#ifdef DEBUG
	printf( "Eliminando simbolo: [%s]\n", symbol->name );
	#endif
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
	struct Symbol *sym = symTable;

	if( !sym ) return;

	// Go to the beginning before start freeing.
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

			if( symTable->symType == SYM_METHOD ){
				lastDefinedMethod = NULL;
			}
		}

	}
}

void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}


