#include "symbolsTable.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

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

static char change = 0;

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
			printf( "Insertando symbol nex is first 2\n");
			symb->prev = lastDefinedMethod->lastSymbol;
			printf( "Insertando symbol nex is first 3\n");
			printf( "Insertando %s (primer hijo), nodo anterior: NULL\n", symb->name );			
		}else{
			// This symbols is the brother of the symbols table top.
			printf( "Insertando no next\n");
			printf( "Insertando %s, nodo anterior: %s\n", symb->name, lastDefinedMethod->lastSymbol->name );
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
	setChanged();
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
	
	mainSymbol->firstChild = 1;
	putsSymbol->firstChild = 0;
	
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
	((struct Method *)(putsSymbol->info))->localSymbols = NULL;
	((struct Method *)(putsSymbol->info))->lastSymbol = putsSymbol;
	
	lastDefinedMethod = ((struct Method *)(mainSymbol->info));		
	mainMethod = mainSymbol;
	printf( "Main y puts insertados\n");
	nextSymIsFirstChild = 0;
	setChanged();
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
	}

	((struct Method *)(symbol->info))->localSymbols = NULL;
	((struct Method *)(symbol->info))->lastSymbol = symbol;
	
	//lastDefinedMethod->lastSymbol = symbol;
	// Insert method's symbol in table.
	insertSymbol( symbol );
	
	// When the final argument is declared, we'll use this pointer to access
	// this method and fill its nArgument field.
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

void insertBlockDefinition( const char* const name, const char* const argName  )
{
	printf( "Insertando bloque con variable [%s]\n", argName );

	// Create and fill the block's symbol.
	struct Symbol* symbol = createSymbol( SYM_BLOCK, name );

	// Fill and insert the block's symbol.
	insertMethodBlockDefinition_( symbol );

	// Insert the block's argument.
	//TODO La variable no es de tipo desconocido, es del tipo del array	
	insertVariable( createVariable(SYM_VARIABLE, argName), NULL );
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


Symbol* createArraySymbol( Symbol* type, unsigned int n )
{
	printf( "CREANDO ARRAY DE TAM %d de tipo %s\n", n, type->name );

	struct Symbol* symbol = createSymbol( SYM_ARRAY_TYPE, "_array" );

	symbol->info = (void *)malloc( sizeof( struct ArrayType ) );

	((struct ArrayType *)(symbol->info))->type = type;
	((struct ArrayType *)(symbol->info))->nElements = n;

	printf( "Creado simbolo array - tipo [%s], nElements [%d]\n", ((struct ArrayType *)(symbol->info))->type->name, ((struct ArrayType *)(symbol->info))->nElements );

	return symbol;
}

void insertArray( Symbol* array, Symbol* type )
{
	int i, n = ((struct ArrayType *)(type->info))->nElements;
	char elementName[100];
	char index[5];

	Symbol* element = NULL;

	printf( "Creando array\n" );

	for( i=0; i<n; i++ ){
		strcpy( elementName, array->name );
		sprintf( index, "%d", i );
		strcat( elementName, "[" );
		strcat( elementName, index );
		strcat( elementName, "]" );

		element = createSymbol( SYM_VARIABLE, elementName );

		element->info = (void *)malloc( sizeof( struct Variable ) );

		//((struct Variable*)(element->info))->type = ((struct ArrayType*)(type->info))->type;
	/*
	variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
	((struct Variable *)(variableStruct->info))->type = NULL;
	return variableStruct;
	*/

		insertVariable( element, ((struct ArrayType*)(type->info))->type );

		printf( "\tIntroduciendo elemento [%s]\n", elementName );
		
	}

	showSymTable();
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
	//struct Symbol* previousSymbol = s;
	
	while( s != NULL){
		if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
			printf( "Variable %s Encontrada \n", name );
			return s;
		}
		//If prev symbol is a method it could be the parent or the brother.
		//If it is the parent stop the search 	
		if(s->symType == SYM_METHOD && ((struct Method *)(s->info))->localSymbols == lastDefinedMethod->localSymbols)
		{
			printf( "Variable %s no encontrada caso yupi\n", name );
			return NULL;
		}	
		//previousSymbol = s;
		s = s->prev;
		
		/*if(s != NULL && s->symType == SYM_METHOD && ((struct Method *)(s->info))->localSymbols == previousSymbol->prev)
		{
			printf( "Variable %s no encontrada \n", name );
			return NULL;
		}	*/			
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
	struct Method* scope = getCurrentScope();
	//printf("----->Scope apunta a %s\n", scope->lastSymbol->name);
	insertMethodDefinition( "getc" );
	//TODO Insertar codigo para que funcione el getc
	goInScope(scope);		
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
			case SYM_ARRAY_TYPE:
				printf( "SYM_ARRAY_TYPE" );
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
		struct ArrayType* arrayInfo;
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
			case SYM_ARRAY_TYPE:
				arrayInfo = ((struct ArrayType *)(sym->info));
				printf( " - nElements: [%i], elements type: [%s]\n", arrayInfo->nElements, arrayInfo->type->name );
			break;
			default:
				printf( "\n" );
			break;
		}

		sym = sym->next;
	}
}

void showGlobals_()
{
	if( nextSymIsFirstChild ){
		printf( "nextSymIsFirstChild = 1\n" );
	}else{
		printf( "nextSymIsFirstChild = 0\n" );
	}

	if( symTable ){
		printf( "symTable: [%s]\n", symTable->name );
	}else{
		printf( "symTable: [NULL]\n" );
	}

	if( mainMethod ){
		printf( "mainMethod: [%s]\n", mainMethod->name );
	}else{
		printf( "mainMethod: [NULL]\n" );
	}

	if( mainMethodNext ){
		printf( "mainMethodNext: [%s]\n", mainMethodNext->name );
	}else{
		printf( "mainMethodNext: [NULL]\n" );
	}
}

void showSymTable()
{
	Symbol* sym = symTable;

	printf( "Symbols table -------------------------------\n" );

	showGlobals_();
	
	printf( "**********\n" );

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
/*
void goOutOfScope(){
	if( nextSymIsFirstChild ){
		nextSymIsFirstChild = 0;
	}else{
		struct Symbol*scope = lastDefinedMethod->lastSymbol;
		while( scope != NULL && !scope->firstChild ){
			scope = scope->prev;
		}

		if( scope ){
			scope = scope->prev;
			struct Symbol *s = scope;
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
				lastDefinedMethod = (struct Method *)(mainMethod->info);	
			}	
		}

	}
	showSymTable();
	printf("<<<<<<<<<<<<get out scope symtable = %s\n", symTable->name); 
}*/

struct Method* getCurrentScope()
{
	return lastDefinedMethod;
}

void goInScope(struct Method *method)
{	
	nextSymIsFirstChild = 0;
	lastDefinedMethod = method;	
}
	
void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}

void setChanged()
{
	change = 1;
}

void resetChange()
{
	change = 0;
}

const char getChange()
{
	return change;
}

/*void setMain()
{
	mainMethodNext = symTable;	
	((struct Method*)(mainMethodNext->info))->lastSymbol = symTable;	 
}*/

