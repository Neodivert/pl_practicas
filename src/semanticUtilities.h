#ifndef SEMANTIC_UTILITIES_H
#define SEMANTIC_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolsTable.h"
#include "sintax.tab.h"
extern int compilationState; 

struct MethodInfo
{
	struct Method *scope;
	int result;
};

/*                                  1. Expressions                            */

// Check if subexpressions' types s1 and s2 are both INTEGER, FLOAT or CHAR. 
// Return value:
//	s1 - if s1 and s2 are both INTEGER or FLOAT.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
Symbol* checkAritmeticExpression(Symbol* s1, Symbol* s2, char *op);

// Check if subexpressions types s1 and s2 are both INTEGER, FLOAT or CHAR.
// Return value:
// boolean type symbol - if s1 and s2 are both INTEGER, FLOAT or CHAR.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
Symbol* checkRelationalExpression(Symbol* s1, Symbol* s2, char *op);

// Check if subexpressions types s1 and s2 are both BOOLEAN.
// Return value:
//	s1 - if s1 and s2 are both BOOLEAN.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
Symbol* checkLogicalExpression(Symbol* s1, Symbol* s2, char *op);

// Check if expression's type s is TYPE_BOOLEAN (so its allowed in a NOT expr.)
// Return value:
//	s - if s is TYPE_BOOLEAN
//	NULL - otherwise (*)
// (*) If s is non-NULL, this function also generates an error message.
Symbol* checkNotExpression(Symbol* s);

// Check if type s is an boolean. 
// Return value:
//	s - if s is an boolean.
//	NULL - otherwise.
// (*) If s is non-NULL, this function also generates an error message.
Symbol* checkIsBoolean(Symbol* s);

// Check if both subexpressions types s1 and s2 are equal.
// Return value:
//	s1 - if s1 and s2 are the same type.
// 	NULL - otherwise. (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
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

// Return 0 if call argument with type "type" and position "argument" match the
// corresponding argument in method definition (*). Otherwise return 1.
// (*) If the argument does not have a known type we asume the method call is 
// right and assign the type of the value to the argument.
int checkMethodCallArguments(Symbol *method, Symbol *type, int argument);

//Check is method exist and was call with the right amount of arguments
struct Symbol* checkMethodCall(cstr name, int methodArguments, int currentNArguments, Symbol *method);

// Check if method name is already in symbols' table (if not, insert it).
// Return a MethodInfo struct, whose "scope" field points to the current 
// scope's symbol and "result" is an integer which indicates if method was
// already defined (1) or not (0).
struct MethodInfo *checkMethodDefinition(cstr name);

// Set method's return type to type of symbols "type"
void setMethodReturnType( struct Symbol *method, struct Symbol *type);

// Return 0 if variable name exists in symbols' table. If not, create and insert
// it, and then return 1.
int checkArgumentDefinition(cstr name);


/*                                 4. Blocks                                  */

// Search in symbols' table for block "name" with argument "argName". If it is
// not in the symbols' table, create and insert it. This functions returns and
// Method* that points to the current scope's (the block) symbol.
struct Method * checkBlockDefinition(cstr name, cstr argName);

/*                                 5. Classes                                  */

// Search class "className" in symbols' table. If not found, create and insert
// it.
// Return value: A pointer to the class' symbol.
struct Symbol* checkClassDefinitonPre(const char * const className, struct Symbol *currentClass);

// Search class "className" in symbols' table. If found, set its number of 
// arguments to "nVariables". If the class has no fields, call to yyerror.
struct Symbol* checkClassDefinitonPost(const char * const className, int nVariables);

// Create a instance "varName" from class' symbol "classSymbol".
// Return value: 1 if successful, 0 if error.
int checkClassNew(struct Symbol *classSymbol, const char* const varName);

// Make effective an assigment "varName = literal" in the definition of class 
// "classSymbol" 
// Other arguments:
// 	type - type of literal.
// 	pos - position of assignment "varName = literal" in class definition.
// Return value: position of assigment + 1.
int checkClassContentDefinition(struct Symbol *classSymbol, const char* const varName, struct Symbol *type, int pos);

// Create and return an auxiliar SymbolInfo struct for variable "name" with a
// null symbol pointer, the symbol's type (SYM_CLASS_VARIABLE) and the var 
// name. 
struct SymbolInfo* checkClassAtribute( const char* const name );

/*                                6. Others                                   */

// Search variable in left_ in symbol table:
// If found and without known type -> assing type
// If found and with known type -> check right is the same type
// If not found -> insert in symbol table with right's type    
// It returns left_ struct symbol, in other words var = exp
// it returns var struct symbol
Symbol* checkAssignement(struct SymbolInfo* left_, Symbol *right);

// Is s a variable?
// Return value: an integer with value:
//	0 - s is a variable with a known type.
//	1 - s is a variable with a uknown type.
// 	2 - s ins't a variable.
int isVariable(Symbol *s);

// Return a empty SymbolInfo struct (all its fields are NULL or 0).
struct SymbolInfo* nullSymbolInfo();

struct SymbolInfo* checkArrayContent(struct Symbol* type, struct SymbolInfo* arrayInfo );



// Check if a method call has the same number of arguments that the method
// definition.
//void checkMethodCall( cstr name );

#endif
// SEMANTIC_UTILITIES_H
