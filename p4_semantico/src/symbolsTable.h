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


//TODO Que hacemos con los arrays? Incluirlos dentro de type
//entonces, habria que añadir dos campos mas, kizas un void*
//extra_info? con un struct k tenga tamaño y tipo del array
struct Type {
   int id;
   struct Symbol *fields;   // Si id = CLASS, esto apuntaria a los campos.
};

struct Variable {
   	struct Symbol* type;
};

struct Method {
	int nArguments;
	struct Symbol *localSymbols;
};

typedef struct Symbol Symbol; 

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty generic" symbol defining only its symType and name
struct Symbol* createSymbol ( int symType, const char* const name );

// Insert symbol in symbols table's top.
void insertSymbol( struct Symbol *symb );

struct Symbol* getCreateVariable( int symType, const char* const name);


/*                       2. Specific symbols insertion                        */

// Insert method "name" in symbols table. If we don't go out of scope (by 
// calling "goOutOfScope()"), next symbols will be added as method's local data.
void insertMethodDefinition( const char* const name  );

// Insert block with argument "argName" in symbols table. If we don't go out of 
// scope (by calling "goOutOfScope()"), next symbols will be added as block's 
// local data.
void insertBlockDefinition( const char* const argName );

// Insert type with name "name" and id "typeId" in symbols table.
void insertTypeDefinition( const char* const name, int typeId );

void insertVariable( struct Symbol *symbol, struct Symbol *type );


/*                             3. Symbol search                               */

struct Symbol* searchType( int id );

struct Symbol* searchVariable( int symType, const char* const name );

struct Symbol* searchMethod(const char* const name );

struct Symbol* searchNArgument(struct Symbol *method, int n);

/*                       4. Symbols table management                          */

// Insert a "main method" and the basic types (integer, float, etc).
void initializeSymTable();

// Show the entire symbol tree.
void showSymTable();

// Free the entire symbol tree.
void freeSymbTable();

// Free the allocate memory for a symbol.
void freeSymbol(struct Symbol*);


/*                                  5. Others                                 */

// Go backwards in the symbols table until reaching a method. Next symbols will 
// be this method's brothers.
void goOutOfScope();

// Set the last defined method's number of arguments to n. 
void setNArguments( int n );

void setMain();

#endif
// SYMBOLS_TABLE_H
