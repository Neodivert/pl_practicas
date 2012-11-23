#ifndef SYMBOLS_TABLE_H
#define SYMBOLS_TABLE_H

#include <stdlib.h>
#include <string.h>

static const int MAX_SIZE = 30;


/*                                   Constants                                */
/******************************************************************************/

// Valores posibles para el campo "symType" de Symbol.
#define SYM_TYPE 1
#define SYM_VARIABLE 2
#define SYM_CONSTANT 3
#define SYM_GLOBAL 4
#define SYM_METHOD 5
#define SYM_BLOCK 6


// Valores posibles para el campo "id" de Type.
#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3
#define TYPE_CHAR 4
#define TYPE_BOOLEAN 5
#define TYPE_CLASS 6
#define TYPE_ARRAY 7

/*                                Data structs                                */
/******************************************************************************/

struct Symbol
{
   	int symType; // symType = TYPE, VARIABLE, etc.
	char *name;
	
	void *info;

	// If firstChild = 0, prev points to the previous Symbol.
	// If firstChild = 1, prev points to the parent.
	char firstChild;
	struct Symbol *prev, *next;
};	

typedef struct Symbol Symbol; 


//TODO Que hacemos con los arrays? Incluirlos dentro de type
//entonces, habria que añadir dos campos mas, kizas un void*
//extra_info? con un struct k tenga tamaño y tipo del array
struct Type {
	int id;
	union 
	{
		struct ArrayType* arrayInfo; // Used if id == TYPE_ARRAY;
		struct ClassType* classInfo;   // If id == TYPE_ARRAY, this points to first element.
   	};
};

struct ArrayType {
	Symbol* type;
	unsigned int nElements;
};

struct ClassType{
	unsigned int nElements;
	Symbol* firstField;
};

struct Variable {
   	Symbol* type;
};

struct Method {
	int nArguments;
	Symbol *lastSymbol;
	Symbol *localSymbols;
	Symbol *returnType;
};



typedef const char* const cstr;

struct SymbolInfo
{
	Symbol *symbol;
	int info;
};
/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty generic" symbol defining only its symType and name
Symbol* createSymbol ( int symType, cstr name );

// Insert symbol in symbols table's top.
void insertSymbol( Symbol *symb );

Symbol* createVariable( int symType, cstr name);

Symbol* getCreateVariable( int symType, cstr name, struct SymbolInfo* atribute);

Symbol* createArraySymbol( Symbol* type, unsigned int n );

/*                       2. Specific symbols insertion                        */

// Insert method "name" in symbols table. If we don't go out of scope (by 
// calling "goOutOfScope()"), next symbols will be added as method's local data.
void insertMethodDefinition( cstr name  );

// Insert block with argument "argName" in symbols table. If we don't go out of 
// scope (by calling "goOutOfScope()"), next symbols will be added as block's 
// local data.
void insertBlockDefinition( cstr name, cstr argName );

// Insert type with name "name" and id "typeId" in symbols table.
void insertTypeDefinition( cstr name, int typeId );

void insertVariable( Symbol *symbol, Symbol *type );

// Insert n variables with name[i] where i is in range 0..n-1.
void insertArray( Symbol* type, unsigned int n );


/*                             3. Symbol search                               */

Symbol* searchType( int id );

Symbol* searchVariable( int symType, cstr name );

Symbol* searchMethod(cstr name );

Symbol* searchNArgument(Symbol *method, int n);


/*                       4. Symbols table management                          */

// Insert a "main method" and the basic types (integer, float, etc).
void initializeSymTable();

// Show the entire symbol tree.
void showSymTable();

// Free the entire symbol tree.
void freeSymbTable();

// Free the allocate memory for a symbol.
void freeSymbol(Symbol*);


/*                                  5. Others                                 */

// Go backwards in the symbols table until reaching a method. Next symbols will 
// be this method's brothers.
struct Method* getCurrentScope();

//void goOutOfScope();

void goInScope(struct Method *method);

// Set the last defined method's number of arguments to n. 
void setNArguments( int n );

void setChanged();

void resetChange();

const char getChange();

Symbol* getArrayType(Symbol* variable);  

Symbol* getVariableType(int symType, cstr name, struct SymbolInfo* symbolInfo);

//void setMain();

#endif
// SYMBOLS_TABLE_H
