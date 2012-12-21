/*This file intends to keep the definitions of the procedures and functions that will used in
the code generation. Also the global variables and data structures*/
#include "Q.h"
#include "symbolsTable.h"
#include <stdio.h>
/*gc creates the code line that the code generation part passes*/
//gc(char *str);

/*Registers a new label and returns the identifier*/
int newLabel();

/*Returns an available register*/
int assignRegisters(int type);

/*Frees all the registers*/
int freeRegisters();

/*Frees the i-th register*/
int freeRegister(int i, int type);

/*Code generation for variables*/
int variableCodeGeneration;

/*Generates code for variable storage*/
int getAllGlobals(FILE* yyout);

/*Returns the actual address for storage in memory*/
/*Also asigns it to the corresponding field in the symbols table*/
unsigned int returnAddress(int symbolType, cstr id);

// Generate the code for a method "begin"
//	- Method label.
//	- Local data allocation.
void genMethodBegin( FILE* yyout, cstr methodName );

// Generate the code for a method "end"
// 


/*                               Methods                                      */

// Generate the code for a method "begin" (method label and local space 
// allocation).
void genMethodBegin( FILE* yyout, cstr methodName );

// Generate the code for a method "end" (local data free and return).
void genMethodEnd( FILE* yyout, cstr methodName );

// Generate the code for a method call "begin" (parameters memory allocation).
void genMethodCallBegin( FILE* yyout, cstr methodName );

// Generate the code for a parameter pass. Arguments:
// - vRegister - index of register with the parameter's value.
// - offset - parameter position in stack.
void genParameterPass( FILE* yyout, int vRegister, int offset );
