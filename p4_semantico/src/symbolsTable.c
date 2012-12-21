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

//Pointer for method tracking in the initial registration of global variables
struct Symbol* currentGlobalVariable = NULL;

static char change = 0;

extern int compilationState;

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty and generic" symbol defining only its symType and name.
Symbol* createSymbol ( int symType, cstr  name )
{
	// Allocate memory for new symbol.
	Symbol *symbol = (Symbol *)malloc( sizeof( Symbol ) );

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

// Insert symbol in symbols table.
void insertSymbol( Symbol *symb )
{
	assert( symb != NULL );
	// Not the first symbol added to symbols table.
	if( nextSymIsFirstChild ){
		if(symb->symType == SYM_GLOBAL)
		{	
			symb->prev = ((struct Method*)(mainMethod->info))->lastSymbol;
			((struct Method*)(mainMethod->info))->lastSymbol->next = symb;
			((struct Method*)(mainMethod->info))->lastSymbol = symb;		
		}else
		{
			// This symbol is the first child of the symbols table top.			
			symb->firstChild = 1;
			((struct Method *)(lastDefinedMethod->lastSymbol->info))->localSymbols = symb;
			symb->prev = lastDefinedMethod->lastSymbol;	
			lastDefinedMethod->lastSymbol = symb;	
			nextSymIsFirstChild = 0;				
		}		
	}else{
		if(symb->symType == SYM_GLOBAL)
		{	
			symb->prev = ((struct Method*)(mainMethod->info))->lastSymbol;
			((struct Method*)(mainMethod->info))->lastSymbol->next = symb;
			((struct Method*)(mainMethod->info))->lastSymbol = symb;		
		}else
		{		
			// This symbols is the brother of the symbols table top.
			symb->prev = lastDefinedMethod->lastSymbol;
			lastDefinedMethod->lastSymbol->next = symb;
			if( ((struct Method*)(mainMethod->info))->lastSymbol == lastDefinedMethod->lastSymbol) 
			{
				symTable = symb;
				((struct Method*)(mainMethod->info))->lastSymbol = symb;
			}
			lastDefinedMethod->lastSymbol = symb;									
		}			
		nextSymIsFirstChild = 0;
	}
	
	//Simbol table changed
	setChanged();
}

// Create a variable's symbol defining its symType (SYM_VARIABLE, SYM_CONSTANT
// or SYM_GLOBAL) and its name.
Symbol* createVariable( int symType, cstr name)
{
	Symbol* variableStruct = createSymbol( symType, name );
	variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
	((struct Variable *)(variableStruct->info))->type = NULL;
	((struct Variable *)(variableStruct->info))->address = 0;
	return variableStruct;
}


// Search in symbols' table for variable with symType "symType" and name "name"
// and return it. If not found, create and return it.
// The "attribute" argument indicates if the variable is simple, or an array or
// class element.
Symbol* getCreateVariable( int symType, cstr name, struct SymbolInfo* atribute)
{
	Symbol* variableStruct = searchVariable( symType, name );
	if(atribute == NULL )
	{
		yyerror("Internal error: Atribute should never be null");
		return NULL;
	}
	
	if( atribute->info == 0)//No atribute info 
	{	//Variable with a simple type
		if( variableStruct == NULL){
			variableStruct = createVariable(symType, name);
		}
	}else //Array or class variable
	{
		if(variableStruct != NULL )
		{	//The variable already exists
			if(atribute->info == TYPE_ARRAY){
				//Atribute is [] so this variable must be an array
				struct Variable* variable = ((struct Variable*)(variableStruct->info));
				if( ((struct Type*)(variable->type)) != NULL ){
					if(((struct Type*)(variable->type->info))->id != TYPE_ARRAY)
					{
						yyerror("Type error: [] operator can not be applied on variable %s", name);
					}
				}	
			}	
		}else{
			variableStruct = createVariable(symType, name);
		}
	}	
	return variableStruct;
}

// Create a array type symbol for a array of size n whose elements are of type
// "type".
Symbol* createArraySymbol( Symbol* type, unsigned int n )
{
	char arrayName[50] = "";
	char index[5];
	sprintf( index, "%d", n );
	strcat(arrayName, "array_");
	strcat(arrayName, type->name);
	strcat(arrayName, "_");
	strcat(arrayName, index);
	Symbol* symbol = createSymbol( SYM_TYPE, arrayName );

	symbol->info = (void *)malloc( sizeof( struct Type ) );

	struct Type* arrayType = ((struct Type*)(symbol->info));
	arrayType->id = TYPE_ARRAY;
	arrayType->size = n*((struct Type*)(type->info))->size;
	arrayType->arrayInfo = malloc( sizeof( struct ArrayType ) );
	arrayType->arrayInfo->type = type;
	arrayType->arrayInfo->nElements = n;

	return symbol;
}

Symbol* createClassSymbol( const char* const name )
{
	struct Symbol* symbol = createSymbol(SYM_TYPE, name);
	symbol->info = (void *)malloc(sizeof(struct Type));
	struct Type* type = (struct Type*)symbol->info;
	type->id = TYPE_CLASS;
	type->size = 0;
	type->classInfo = malloc(sizeof(struct ClassType));
	type->classInfo->nElements = 0;
	type->classInfo->elements = NULL;
	return symbol;	
}

/*                       2. Specific symbols insertion                        */

// Auxiliar function for inserting the two first symbols in first table:
// main and puts methods.
void insertMainPuts()
{
	// Create and fill the method's symbol.
	Symbol* mainSymbol = createSymbol( SYM_METHOD, "_main" );
	Symbol* putsSymbol = createSymbol( SYM_METHOD, "puts" );
	
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
	((struct Method *)(putsSymbol->info))->label = newLabel();
	
	lastDefinedMethod = ((struct Method *)(mainSymbol->info));		
	mainMethod = mainSymbol;
	nextSymIsFirstChild = 0;
}

// Insert a block or method definition symbol in symbols' table.
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
	}

	((struct Method *)(symbol->info))->localSymbols = NULL;
	((struct Method *)(symbol->info))->lastSymbol = symbol;
	((struct Method *)(symbol->info))->returnType = NULL;
	((struct Method *)(symbol->info))->label = newLabel();
	
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

// Insert method "name" in symbols table. If we don't go out of scope (by 
// calling "goOutOfScope()"), next symbols will be added as method's local data.
void insertMethodDefinition( cstr name  )
{
	// Create and fill the method's symbol.
	Symbol* symbol = createSymbol( SYM_METHOD, name );

	// Fill and insert the method's symbol.
	insertMethodBlockDefinition_( symbol );
}

// Insert block with argument "argName" in symbols table. If we don't go out of 
// scope (by calling "goOutOfScope()"), next symbols will be added as block's 
// local data.
void insertBlockDefinition( cstr name, cstr argName  )
{

	// Create and fill the block's symbol.
	Symbol* symbol = createSymbol( SYM_BLOCK, name );

	// Fill and insert the block's symbol.
	insertMethodBlockDefinition_( symbol );

	// Insert the block's argument, it's type will be set later
	insertVariable( createVariable(SYM_VARIABLE, argName), NULL );
}

// Insert type with name "name" and id "typeId" in symbols table.
void insertTypeDefinition( cstr name, int typeId, int size )
{	
	
	Symbol* symbol = createSymbol( SYM_TYPE, name );

	symbol->info = (void *)malloc( sizeof( struct Type ) );
	
	((struct Type *)(symbol->info))->id = typeId;
	((struct Type *)(symbol->info))->size = size;

	insertSymbol( symbol );
}

// Insert variable "symbol" of type "type" in symbols table.
void insertVariable( Symbol *symbol, Symbol *type )
{	
	((struct Variable *)(symbol->info))->type = (void *)type;

	insertSymbol( symbol );
}

// Insert symbol of a array type with n elements of type "type".
void insertArray( Symbol* type, unsigned int n )
{
	Symbol* symbol;
	symbol = createArraySymbol(type, n);
	insertSymbol(symbol);
}

/*                             3. Symbol search                               */

// Search in symbols table for a type with id "id".
Symbol* searchType( int typeId )
{
	Symbol* s = symTable;
	while( s != NULL ){
		if( s->symType == SYM_TYPE ){
			if( ((struct Type *)(s->info))->id == typeId ){
				return s;
			}
		}
		s = s->prev;
	}
	return NULL;
}

// FALTA: Hay que hacer que si estamos buscando en un bucle, no se tiene acceso
// a las variables globales.y en el método exterior.
// Search in symbols table for a variable with type "symType" and name "name".
Symbol* searchVariable( int symType, cstr name )
{
	Symbol* s;
	if(symType == SYM_GLOBAL){	
		s = ((struct Method*)(mainMethod->info))->lastSymbol;
		while( s != NULL){
			if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
				return s;
			}
			s = s->prev;		
		}				
	}else{	
		s = lastDefinedMethod->lastSymbol;
		while( s != NULL){
			if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
				return s;
			}
			//If prev symbol is a method it could be the parent or the brother.
			//If it is the parent stop the search 	
			if(s->symType == SYM_METHOD && ((struct Method *)(s->info))->localSymbols == lastDefinedMethod->localSymbols){
				return NULL;
			}	
			s = s->prev;		
		}			
	}
	return NULL;
}

// TODO: Hay que cambiar el comentario.
// Search in symbols table for a method "name".
struct Symbol* searchTopLevel(int symType, const char* const name )
{
	Symbol* s = mainMethodNext;
	while( s != NULL ){
		if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
			return s;
		}
		s = s->next;
	}
	return NULL;
}

// Search in symbols table for the n-th argument of method "method".
Symbol* searchNArgument(Symbol *method, int n)
{
	int i;
	
	if(method == NULL || n > ((struct Method *)(method->info))->nArguments || n <= 0){
		return NULL; 
	} 

	Symbol* argument = ((struct Method *)(method->info))->localSymbols;
	for (i = 1; i < n; i++){
		argument = argument->next;
	}
	return argument;
}

//Searchs for the next Global Variable record in the current global variable registration
//without knowing the name
struct Variable* nextGlobalVariablePointer(int flag)
{
	struct Symbol* aux = NULL;
	while (currentGlobalVariable != NULL)
	{
		if(currentGlobalVariable->symType == SYM_GLOBAL)
		{
			aux = currentGlobalVariable;
			currentGlobalVariable = currentGlobalVariable->next;
			return ((struct Variable*)(aux->info));
		}				
		currentGlobalVariable = currentGlobalVariable->next;	
	}
	return NULL;
}

/*                       4. Symbols table management                          */

// Insert a "main method" and the basic methods (I/O) and types (integer, 
// float, etc).
void initializeSymTable()
{
	insertMainPuts();
			
	struct Method* scope = getCurrentScope();
		insertMethodDefinition( "getc" );			
	goInScope(scope);

	scope = getCurrentScope();
		insertMethodDefinition( "exit" );			
	goInScope(scope);
			
	insertTypeDefinition( "integer", TYPE_INTEGER, 4 );
	insertTypeDefinition( "float", TYPE_FLOAT, 4 );
	insertTypeDefinition( "string", TYPE_STRING, 1 );
	insertTypeDefinition( "char", TYPE_CHAR, 1 );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN, 1 );
	
	struct Symbol* method = searchTopLevel(SYM_METHOD, "puts");
	scope = getCurrentScope();
	
	goInScope( ((struct Method*)(method->info)) );	
		struct Symbol* var = createVariable(SYM_VARIABLE, "input_str");
		insertVariable(var, searchType(TYPE_STRING));	
	goInScope(scope);
		
	method = searchTopLevel(SYM_METHOD, "getc");
	((struct Method*)(method->info))->returnType = searchType(TYPE_CHAR);		

	currentGlobalVariable = mainMethodNext;	

}

// Auxiliar function. Show symbols from sym, and tabulate them according to 
// their level.
void showSymTable_( Symbol* sym, int level )
{
	int i = 0;
	int j = 0;
	while( (sym != NULL) ){
		// Tabulate current symbol.
		for( i=0; i<level; i++ ) printf( "\t" );

		// Show current symbol's name.
		switch( sym->symType ){
			case SYM_TYPE:
				printf( "TYPE" );
			break;
			case SYM_GLOBAL:
				printf( "GLOBAL" );	
			break;			
			case SYM_VARIABLE:
				printf( "VARIABLE" );
			break;
			case SYM_CLASS_VARIABLE:
				printf( "CLASS_VAR" );
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
		struct ArrayType* arrayInfo;
		switch( sym->symType ){
			case SYM_TYPE:
				printf(" - size:[%i]", ((struct Type*)(sym->info))->size);
				if( ((struct Type*)(sym->info))->id == TYPE_ARRAY )
				{
					struct ArrayType *arrayInfo = ((struct Type*)(sym->info))->arrayInfo;
					printf(" - type:[%s] - nElements:[%d]\n", arrayInfo->type->name,arrayInfo->nElements);
				}else
				{
					if( ((struct Type*)(sym->info))->id == TYPE_CLASS )
					{
						struct ClassType *classInfo = ((struct Type*)(sym->info))->classInfo;
						printf(" - elements:[%d]", classInfo->nElements);
						for( j = 0; j < classInfo->nElements; j++){
							if(classInfo->elements && classInfo->elements[j]){
							 printf(" - element: [%d] [%s] ",j, classInfo->elements[j]->name);
							} 
						}						
					}	
					printf("\n");
				}
				break;
			case SYM_VARIABLE:
			case SYM_GLOBAL:
			case SYM_CONSTANT:
			case SYM_CLASS_VARIABLE:
				aux = ((struct Variable*)(sym->info))->type;
				printf( " - type: " );
				if( aux ){
					printf( "[%s]", aux->name );
				}else{
					printf( "NULL" );
				}
				printf( " - address [%i]\n", ((struct Variable*)(sym->info))->address);
			break;
			case SYM_METHOD:
				printf("- arguments' size [%i]", ((struct Method*)(sym->info))->argumentsSize);
				printf("- locals' size [%i]", ((struct Method*)(sym->info))->localsSize);
				printf("- label [%i]", ((struct Method*)(sym->info))->label);
				aux = ((struct Method*)(sym->info))->returnType;
				if(aux == NULL){
					printf( " - return type: [NULL]]");
				}else{
					printf( " - return type: [%s]", aux->name);
				}	
			case SYM_BLOCK:
				printf("- label [%i]", ((struct Method*)(sym->info))->label);
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

// Show the value of globals variables rilated to symbols' table.
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

// Show the entire symbol tree.
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

// Free the memory allocated for a symbol.
void freeSymbol(Symbol* symbol)
{
	#ifdef DEBUG
	printf( "Eliminando simbolo: [%s]\n", symbol->name );
	#endif
	if(symbol->symType == SYM_TYPE)
	{
		struct Type *type = ((struct Type*)(symbol->info));
		if ( type->id == TYPE_ARRAY )
		{
			free(type->arrayInfo);
		}
		else
		{
			if ( type->id == TYPE_CLASS )
			{
				free(type->classInfo->elements);
				free(type->classInfo);
			}
		}	
	}	
	free(symbol->name);
	free(symbol->info);
	free(symbol);
}

// Auxiliar recursive function for freeing the symbols' tree.
void freeSymbTable_( Symbol* symTable_ ){
	Symbol *aux = symTable_;
	while( aux ){
		symTable_ = aux->next;
		if( aux->symType == SYM_METHOD ){
			freeSymbTable_( ((struct Method *)(aux->info))->localSymbols );
		}		
		freeSymbol( aux );
		aux = symTable_;
	}
}

// Free the entire symbol tree.
void freeSymbTable(){
	Symbol *sym = symTable;

	if( !sym ) return;

	// Go to the beginning before start freeing.
	while( sym->prev ){
		sym = sym->prev;
	}

	freeSymbTable_( sym );
}

void freeSymbolInfo(struct SymbolInfo* symbol)
{
	free(symbol->name);
	free(symbol);	
}

/*                          5. Global "change" management                     */

// Set "change" global to 1.
void setChanged()
{
	change = 1;
}

// Set "change" global to 0.
void resetChange()
{
	change = 0;
}

// Get the "change" global value.
const char getChange()
{
	return change;
}


/*                        6. Last defined method management                   */

// Return a pointer to the last defined method / block.
struct Method* getCurrentScope()
{
	return lastDefinedMethod;
}

// Set method as the last defined method.
void goInScope(struct Method *method)
{	
	
	if(method->localSymbols){
		nextSymIsFirstChild = 0;
	}else{
		nextSymIsFirstChild = 1;
	}		
	lastDefinedMethod = method;	
}

struct Method *getParentScope()
{
	//If we are at main's scope there is nothing over it, so
	//return mainScope
	if( lastDefinedMethod == ((struct Method *)(mainMethod->info)) ){
		return lastDefinedMethod;
	}
	
	struct Symbol *s = NULL;
	//If localSymbol is defined, in prev is current method
	if(lastDefinedMethod->localSymbols){
		s = lastDefinedMethod->localSymbols->prev;	
	}else{ //Otherwise current lastSymbol points current method
		s = lastDefinedMethod->lastSymbol;
	}	
	
	while( s != NULL && ! s->firstChild){
		s = s->prev;		
	}
	
	
	
	if(s == NULL || s->prev == NULL){
		lastDefinedMethod = (struct Method *)(mainMethod->info);
	}else{
		lastDefinedMethod = (struct Method *)(s->prev->info);
	}
	return lastDefinedMethod;
}
	
// Set the last defined method's number of arguments to n. 
void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}


/*                                  7. Others                                 */

// Return the type of the elements of array type "variable". If unknown, return
// NULL.
Symbol* getArrayType(Symbol* variable)
{
	if( variable != NULL && variable->info != NULL)
	{
		Symbol* array = ((struct Variable*)(variable->info))->type;
		if(array != NULL && array->info != NULL ){	
			if( ((struct Type*)(array->info))->id  == TYPE_ARRAY ){			
				return ((struct Type*)(array->info))->arrayInfo->type;	
			}else{
				return NULL;
			}		
		}else{
			return NULL;
		}	
	}else{
		return NULL;
	}		
}

Symbol* getVariableType(int symType, cstr name, struct SymbolInfo* symbolInfo)
{
	Symbol* variable = getCreateVariable( symType, name, symbolInfo ); 			
	Symbol* type = NULL;
	if(variable == NULL){
		type = NULL;
	}else{
		if( symbolInfo->info == TYPE_ARRAY ){
			type = getArrayType(variable);			
		}else{
			if( symbolInfo->info == SYM_CLASS_VARIABLE ){
				type = getClassVar(variable, symbolInfo->name);
				if(type){
					if( type->info ){
						type = ((struct Variable*)(type->info))->type;
					}else{
						type = NULL;
					}	
				}else{
					type = NULL;
				}					
			}else{
				if(variable->info != NULL){
					type = ((struct Variable*)(variable->info))->type;
				}else{
					type = NULL;
				}		
			}	
		}	
	}	
	freeSymbolInfo(symbolInfo);
	//This is only call on right side of expression so insert variable here
	//if it does not exits on symbol table
	if(searchVariable(symType, name) == NULL){
		insertVariable(variable, NULL);
	}
	
	return type;
}

struct Symbol* getClassVar( struct Symbol* variable, const char* const atributeName)
{
	char varName[100] = "";
	strcat(varName, variable->name);
	if(variable && variable->info){
		if( ((struct Variable*)(variable->info))->type ){
			strcat(varName, ((struct Variable*)(variable->info))->type->name);
			strcat(varName, "@");
			strcat(varName, atributeName);
			struct Symbol *classVar = searchVariable( SYM_VARIABLE, varName);
			if(classVar){
				return classVar;
			}else{
				return NULL;
			}	
		}else{
			return NULL;
		}		
	}else{
		return NULL;
	}		
}

struct Symbol* getReturnType( struct Symbol* method)
{
	if(method && method->info){
		return ((struct Method *)(method->info))->returnType;
	}else{
		return NULL;
	}
}

void createPutsGetcExitCode()
{
}

// Auxiliar - Fill method's fields for arguments size and local data size.
void getMethodDataSize( struct Method* method )
{
   int i = 0;
   
	// Initialization
	method->argumentsSize = 0;
	method->localsSize = 0;
   Symbol* argument = method->localSymbols;
   
	// Calculate arguments size.
   for( i=0; i<method->nArguments; i++ ){
      method->argumentsSize += ( (struct Type* )( ( ( ( struct Variable* )( argument->info ) )->type )->info ) )->size;
      argument = argument->next;
   }

	// Calculate local data size.
	while( argument ){
      if( argument->symType == SYM_VARIABLE ){ 
         method->localsSize += ( (struct Type* )( ( ( ( struct Variable* )( argument->info ) )->type )->info ) )->size;
      }
      argument = argument->next;
   }
}

// Iterate over methods in symbol's table and fill its fields for arguments and
// local data sizes.
void fillMethodDataSizes()
{
	Symbol* symbol = mainMethodNext;
	struct Method* method = NULL;

	// FIXME: falta que descienda en el arbol y que contemple los bloques.
	// Searh for methods and call to "genMethodDataSize" on them.
	while( symbol ){
		if( ( symbol->symType == SYM_METHOD ) ){
			method = ( struct Method* )( symbol->info );
			getMethodDataSize( method, &method->argumentsSize, &method->localsSize );
		}
		symbol = symbol->next;
	}
}
