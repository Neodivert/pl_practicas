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

struct Method* lastDefinedMethod = NULL;
static Symbol* symTable = NULL;
static Symbol* mainMethod = NULL;
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
	assert( symb != NULL );

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

void insertMethodDefinition( const char* const name  )
{
	// Create and fill the method's symbol.
	struct Symbol* symbol = createSymbol( SYM_METHOD, name );

	symbol->info = (void *)malloc( sizeof( struct Method ) );

	((struct Method *)(symbol->info))->nArguments = 0;
	((struct Method *)(symbol->info))->localSymbols = NULL;

	// When the final argument is declared, we'll use this pointer to access
	// this method and fill its nArgument field.
	lastDefinedMethod = ((struct Method *)(symbol->info));
	
	// Insert method's symbol in table.
	insertSymbol( symbol );

	// If we don't go out of scope, next symbol will be a "child".
	nextSymIsFirstChild = 1;
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

struct Symbol* searchMethod(const char* const name )
{
	struct Symbol* s = mainMethod;
	printf("Mainmethod apunta a %s\n", s->name);
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


void showSymTable_( struct Symbol* sym, int level )
{
	int i = 0;

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

		printf( " - [%s]", sym->name );

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
		/*
		if( ( sym->symType == SYM_METHOD ) && ((struct Method*)(sym->info))->localSymbols ){
			#ifdef DEBUG
			printf( " - hijo: [%s]\n",((struct Method*)(sym->info))->localSymbols->name );
			#endif 
			//printf( "\n\t%s\n", ((struct Method*)(sym->info))->localSymbols->name );
			showSymTable_( ((struct Method*)(sym->info))->localSymbols, level+1 );
			
		}
		#ifdef DEBUG
		else{
			printf( " - hijo: [NULL]\n" );
		}
		#endif
		*/
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

void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}

void setMain()
{
	mainMethod = symTable;	 
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

