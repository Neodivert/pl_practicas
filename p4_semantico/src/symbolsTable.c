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

// Fast access to last defined method. 
struct Method* lastDefinedMethod = NULL;

// Symbols tree's root.
static Symbol* symTable = NULL;
static Symbol* mainMethodNext = NULL;
static Symbol* mainMethod = NULL;


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
	printf( "Insertando symbol\n");
	if( symTable == NULL ){
		// First symbol added to symbols table.
		//symTable = symb;
		//symTable->prev = NULL;	
		//lastDefinedMethod->lastSymbol = symb;
		printf( "Insertando symbol\n");
	}else{
		// Not the first symbol added to symbols table.
		if( nextSymIsFirstChild ){
			// This symbol is the first child of the symbols table top.
			printf( "Insertando symbol nex is first 0\n");
			printf( "Last defined apunta a %s\n", lastDefinedMethod->lastSymbol->name);			
			symb->firstChild = 1;
			printf( "Insertando symbol nex is first 1\n");
			((struct Method *)(lastDefinedMethod->lastSymbol->info))->localSymbols = symb;
			//symb->prev = symTable;
			printf( "Insertando symbol nex is first 2\n");
			symb->prev = lastDefinedMethod->lastSymbol;
			printf( "Insertando symbol nex is first 3\n");
			printf( "Insertando %s (primer hijo), nodo anterior: NULL\n", symb->name );			
		}else{
			// This symbols is the brother of the symbols table top.
			printf( "Insertando no next\n");
			printf( "Insertando %s, nodo anterior: %s\n", symb->name, lastDefinedMethod->lastSymbol->name );
			//symb->prev = symTable;
			//symTable->next = symb;
			symb->prev = lastDefinedMethod->lastSymbol;
			lastDefinedMethod->lastSymbol->next = symb;
			if( ((struct Method*)(mainMethod->info))->lastSymbol == lastDefinedMethod->lastSymbol) 
			{
				symTable = symb;
				((struct Method*)(mainMethod->info))->lastSymbol = symb;
			}			
		}
	
	}
	//Update the lastSymbol of the current method
	printf( "Cambiando last define por %s\n",symTable->name); 
	lastDefinedMethod->lastSymbol = symb;
	printf( "Last defined apunta a %s\n", lastDefinedMethod->lastSymbol->name);	
	nextSymIsFirstChild = 0;
}

struct Symbol* createVariable( int symType, const char* const name)
{
	struct Symbol* variableStruct = createSymbol( symType, name );
	variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
	((struct Variable *)(variableStruct->info))->type = NULL;
	return variableStruct;
}

struct Symbol* getCreateVariable( int symType, const char* const name)
{
	struct Symbol* variableStruct = searchVariable( symType, name );
	if( variableStruct == NULL)
	{
		variableStruct = createVariable(symType, name);
		//variableStruct = createSymbol( symType, name );
		//variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
		//((struct Variable *)(variableStruct->info))->type = NULL;
		//variableStruct->info = NULL;
	}
	return variableStruct;
}

/*                       2. Specific symbols insertion                        */

void insertMainPuts()
{
	printf( "Insertando main\n");
	// Create and fill the method's symbol.
	struct Symbol* mainSymbol = createSymbol( SYM_METHOD, "_main" );
	struct Symbol* putsSymbol = createSymbol( SYM_METHOD, "puts" );
	
	mainSymbol->info = (void *)malloc( sizeof( struct Method ) );
	putsSymbol->info = (void *)malloc( sizeof( struct Method ) );
	
	((struct Method *)(mainSymbol->info))->nArguments = 0;	
	((struct Method *)(putsSymbol->info))->nArguments = 1;	
	
	symTable = putsSymbol;
	mainMethodNext = putsSymbol;
	
	mainSymbol->prev = NULL;
	mainSymbol->next = NULL;
	((struct Method *)(mainSymbol->info))->localSymbols = putsSymbol;
	((struct Method *)(mainSymbol->info))->lastSymbol = putsSymbol;
	
	putsSymbol->prev = mainSymbol;
	putsSymbol->next = NULL;	
	lastDefinedMethod = ((struct Method *)(symbol->info));
	((struct Method *)(mainSymbol->info))->localSymbols = NULL;
	((struct Method *)(mainSymbol->info))->lastSymbol = putsSymbol;
	
	lastDefinedMethod = ((struct Method *)(mainSymbol->info));		
	mainMethod = mainSymbol;

}

void insertMethodBlockDefinition_( Symbol* symbol )
{
	printf( "Insertando block metod\n");
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
		
	}
	((struct Method *)(symbol->info))->localSymbols = NULL;
	((struct Method *)(symbol->info))->lastSymbol = symbol;
	
	//lastDefinedMethod->lastSymbol = symbol;
	// Insert method's symbol in table.
	insertSymbol( symbol );
	
	lastDefinedMethod = ((struct Method *)(symbol->info));
	//lastDefinedMethod->lastSymbol = symbol;	

	//((struct Method *)(symbol->info))->lastSymbol = symbol;
	// If we don't go out of scope, next symbol will be a "child".
	nextSymIsFirstChild = 1;
}

void insertMethodDefinition( const char* const name  )
{
	printf( "Insertando metod\n");
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
	printf("Buscando type %d, symbtable apunta a %s\n", typeId, symTable->name);
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
	struct Symbol* s = lastDefinedMethod->lastSymbol;
	while( s != NULL && s->symType != SYM_METHOD){
		if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
			return s;
		}

		s = s->prev;
	}

	showSymTable( symTable, 0 );

	printf( "Variable %s no encontrada \n", name );
	return NULL;
}

struct Symbol* searchMethod(const char* const name )
{
	struct Symbol* s = mainMethodNext;
	printf("mainMethodNext apunta a %s\n", s->name);
	while( s != NULL ){
		printf("symtype = %d, name = %s\n", s->symType, s->name);
		if( s->symType == SYM_METHOD && (strcmp(s->name, name) == 0)  ){
			return s;
		}

		s = s->next;
	}

	showSymTable( symTable, 0 );
	printf( "Metodo %s no encontrado \n", name );
	return NULL;
}

struct Symbol* searchNArgument(struct Symbol *method, int n)
{
	if(method == NULL || n > ((struct Method *)(method->info))->nArguments 
		|| n <= 0)
	 return NULL; 
	int i;
	struct Symbol* argument = ((struct Method *)(method->info))->localSymbols;
	for (i = 1; i < n; i++)
	{
		argument = argument->next;
	}
	return argument;
}

/*                       4. Symbols table management                          */

void initializeSymTable()
{
	printf("Antes de main\n");
	//insertMethodDefinition( "_main" );
	insertMainPuts();
	printf("Despues de main\n");
	showSymTable();
	//lastDefinedMethod = (struct Method *)(symTable->info);
	printf("El last method apunta a %s \n",symTable->name); 
	//insertMethodDefinition( "puts" );
	//printf("Despues de puts\n");
	//En set main establecemos el primer hijo de main.
	//setMain();
	//TODO Insertar codigo para que funcione el puts
	//y ademas ponerle un argumento string
	insertMethodDefinition( "getc" );
	//TODO Insertar codigo para que funcione el getc
	goOutOfScope();		
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
		struct Symbol*scope = lastDefinedMethod->lastSymbol;
		while( symTable != NULL && !symTable->firstChild ){
			symTable = symTable->prev;
		}

		if( symTable ){
			symTable = symTable->prev;
			struct Symbol *s = symTable;
			while( s != NULL && (s->symType != SYM_METHOD && s->symType != SYM_BLOCK)){
				s = s->prev;
			}
			if(s != NULL)
			{
				printf("<<<<<<<<<<<<get out scope vale %s\n", s->name); 
				lastDefinedMethod = ((struct Method*)(s->info));
			}	
			else
			{
				//If no previous method was found, assume main is last
				//this should never happend
				printf("<<<<<<<<<<<get out scope xungo vale %s\n", s->name); 
				lastDefinedMethod = (struct Method *)(mainMethodNext->prev->info);	
			}	
		}

	}
	showSymTable();
	printf("<<<<<<<<<<<<get out scope symtable = %s\n", symTable->name); 
}

void goInScope(struct Symbol *method)
{	
	lastDefinedMethod = ((struct Method *)(method->info));	
}
	
void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}

void setMain()
{
	mainMethodNext = symTable;	
	((struct Method*)(mainMethodNext->info))->lastSymbol = symbTable;	 
}

