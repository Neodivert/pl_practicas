/*This file intends to keep the definitions of the procedures and functions that will used in
the code generation. Also the global variables and data structures*/
#include "Q.h"
#include "symbolsTable.h"
#include <stdio.h>
/*gc creates the code line that the code generation part passes*/
//gc(char *str);

typedef struct RegisterInfo RegisterInfo; 

/*Registers a new label and returns the identifier*/
int newLabel();

/*Returns an available register*/
/*0 -> entero, 1-> Flotante*/
int assignRegisters(int type);

/*Frees all the registers*/
int freeRegisters();

/*Frees the i-th register*/
int freeRegister(int i, int type);

/*Code generation for variables*/
int variableCodeGeneration;

/*Generates code for variable storage*/
int getAllGlobals(FILE* yyout);

/*Generates code for local variables of hidden main method*/
int getAllLocalsMain(FILE* yyout);

/*Returns the actual address for storage in memory*/
/*Also asigns it to the corresponding field in the symbols table*/
unsigned int returnAddress(int symbolType, cstr id);

/* Save current used registers to stack 
void storeRegisters(FILE* yyout);

/* Load registers form stack 
void loadRegisters(FILE* yyout, int reg); */

/*                            Assignement                              */

Symbol* genAssignement(FILE* yyout, SymbolInfo* leftSide, Symbol* rightSide, int insideIfLoop);

Symbol* genAccessVariable(FILE* yyout,cstr name, int symType, SymbolInfo* atribute, ExtraInfo** extraInfoPerRegister, int* nextRegisterOverflow);

// Generate the code for a method "begin"
//	- Method label.
//	- Local data allocation.
void genMethodBegin( FILE* yyout, cstr methodName );

SymbolInfo* genArrayContent( FILE* yyout, SymbolInfo* leftSide, Symbol* literalInfo,
	SymbolInfo* arrayInfo ); 


/*                            Method definition                              */

// Generate the code for a method "begin" (set method label and new base, and
// allocate local space).
void genMethodBegin( FILE* yyout, cstr methodName );

// Generate the code for a method "end" (free local data and return).
void genMethodEnd( FILE* yyout, cstr methodName );


/*                               Method call                                 */

// Generate the code for a method call "begin" (arguments memory allocation).
void genMethodCallBegin( FILE* yyout, cstr methodName );

// Generate the code for a method call (save base and return label and call
// method).
void genMethodCall( FILE* yyout, Method* method, int reg );

// Generate the code for a parameter pass. Arguments:
// - iRegister - index of register with the argument's value.
// - method - called method symbol.
// - iArgument - argument index.
void genArgumentPass( FILE* yyout, Symbol* argumentSymbol, Symbol* method, int iArgument );

/*Returns the code that correspond to the storage of the type passed*/
/*as parameter*/
char pointerType(Symbol* symbol);

void genOperation(FILE* yyout, Symbol* leftSide, Symbol* rightSide, char* op );

void genPuts( FILE* yyout, cstr str );

cstr genNumericString( Symbol* symbol );

cstr genVariableInterpolation( FILE* yyout, Symbol* symbol );

void genGetCall( FILE* yyout, char inputType, int reg );

void genPutsHead( FILE* yyout );

/*				Overflow				*/
int checkOverflow(FILE* yyout, int reg, ExtraInfo** extraInfoPerRegister, int* nextRegisterOverflow, int type);


int getType( Symbol* symbol );

int isFloat( Symbol* symbol );


