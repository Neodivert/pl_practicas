#ifndef SEMANTIC_UTILITIES_H
#define SEMANTIC_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolsTable.h"
#include "sintax.tab.h"

struct MethodInfo
{
	struct Method *scope;
	int result;
};

/*                                  1. Expressions                            */

// Check if subexpressions' types s1 and s2 are both INTEGER or FLOAT. If 
// previous condition is satisfied, return s1. Otherwise, generate an error 
// message (using op) and return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkAritmeticExpression(Symbol* s1, Symbol* s2, char *op);

// Check if subexpressions types s1 and s2 are both INTEGER or FLOAT. If 
// previous condition is satisfied, search for boolean type in symbols' table
// and return it. Otherwise, generate an error message (using op) and return 
// NULL. 
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkRelationalExpression(Symbol* s1, Symbol* s2, char *op);

// Check if subexpressions types s1 and s2 are both BOOLEAN. If previous 
// condition is satisfied, return s1. Otherwise, generate an error message 
// (using op) and return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkLogicalExpression(Symbol* s1, Symbol* s2, char *op);

// Check if expression's type s is TYPE_BOOLEAN (so its allowed in a NOT 
// expression).
// If s is of type TYPE_BOOLEAN, return NULL. Otherwise show an error message
// and return NULL.
// (*) If s is NULL, this function simply returns NULL.
Symbol* checkNotExpression(Symbol* s);

// Check if type s is an boolean. If previous  condition is satisfied, return s.
// Otherwise, generate an error message and return NULL (*).
// (*) If s is NULL, this function simply returns NULL.
Symbol* checkIsBoolean(Symbol* s);

// Return s1 if both subexpressions types s1 and s2 are of the same time.
// Otherwise return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkSameType(Symbol* s1, Symbol* s2);


/*                               2. Arrays                                    */

// Check if expression's type s is integer.
// Return a SymbolInfo struct, whose "info" field is set to TYPE_ARRAY, and 
// "symbol" field...
// - points to s if s is a TYPE_INTEGER.
// - points to null otherwise (in this case, an error message is shown too.
// This function is used to verify that an array index is actually a integer.
// (*) If s is NULL, this function simply returns NULL.
struct SymbolInfo* checkIsInteger(Symbol* s);

// Search in the symbols' table for an type array of size "n" and whose elements
// are of type "type".
// If n < 0 (invalid size), return NULL.
// Otherwise, return the array's type symbol (if array wasn't found, this 
// function also create it and insert it).
Symbol* checkArray(Symbol* type, int n);


/*                               3. Methods                                   */
int checkClassDefinition(struct Symbol *classSymbol, const char* const varName, struct Symbol *type, int pos);

void setMethodReturnType(struct Symbol *method, struct Symbol *type);

// Return 0 if call argument with type "type" and position "argument" match the
// corresponding argument in method definition (*). Otherwise return 1.
// (*) If the argument does not have a known type we asume the method call is 
// right and assign the type of the value to the argument.
int checkMethodCall(Symbol *method, Symbol *type, int argument);

// Check if method name is already in symbols' table (if not, insert it).
// Return a MethodInfo struct, whose "scope" field points to the current 
// scope's symbol and "result" is an integer which indicates if method was
// already defined (1) or not (0).
struct MethodInfo *checkMethodDefinition(cstr name);

// Set method's return type to type of symbols "type"
void setMethodReturnType(Symbol *method, Symbol *type);

// Return 0 if variable name exists in symbols' table. If not, create and insert
// it, and then return 1.
int checkArgumentDefinition(cstr name);


/*                                 4. Blocks                                  */

// Search in symbols' table for block "name" with argument "argName". If it is
// not in the symbols' table, create it and insert it. This functions returns
// and Method* that points to the current scope's (the block) symbol.
struct Method * checkBlockDefinition(cstr name, cstr argName);

// Generate a name for block whose name is "name" and argument is "argName".
char *createBlockName(cstr name, cstr argName);


/*                                5. Others                                   */

Symbol* checkAssignement(struct SymbolInfo* left_, Symbol *right);

int isVariable(Symbol *s);

struct SymbolInfo* nullSymbolInfo();

struct SymbolInfo* checkArrayContent(struct Symbol* type, struct SymbolInfo* arrayInfo );

// Check if a method call has the same number of arguments that the method
// definition.
//void checkMethodCall( cstr name );

#endif
// SEMANTIC_UTILITIES_H
