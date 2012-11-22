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
	struct Symbol *prev, *next;//, pos;	// Posición en el padre
}; 								// *symbols;  


//TODO Que hacemos con los arrays? Incluirlos dentro de type
//entonces, habria que añadir dos campos mas, kizas un void*
//extra_info? con un struct k tenga tamaño y tipo del array
struct Type {
   int id;
   struct Symbol *fields;   // Si id = CLASS, esto apuntaria a los campos.
};

struct Variable {
   	struct Symbol* type;
	//void *value; 				// int, float, char*, bool
	// La linea anterior esta comentada porque en principio no nos interesa saber
	// el valor de una variable (como compilador).
};

struct Method {
	int nArguments;
	struct Symbol *localSymbols;
	//En caso de ser una definicion de método o de anónimo, tiene que tener un enlace al nodo
	//que contiene toda su información en el árbol.
	//asumo que esto es un puntero al nodo
	
	//struct Method* block;
}; //function;


typedef struct Symbol Symbol; 

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty generic" symbol defining only its symType and name
struct Symbol* createSymbol ( int symType, const char* const name );

void insertSymbol( struct Symbol *symb );

struct Symbol* getCreateVariable( int symType, const char* const name);

/*                       2. Specific symbols insertion                        */

void insertMethodDefinition( const char* const name  );

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

void goOutOfScope();

void setNArguments( int n );

void setMain();
//Asumo que ahora hay que crear el árbol

//Nodos del arbol
//Nodo raiz VS Nodo def nodo anónimo(diferencia en el tipo especificado)

	//Enlace a la tabla en el propio nodo
	//Los enlaces a los hijos son contenidos en la definición del
	//hijo dentro de la tabla de símbolos



//Al crear métodos de insercion de cada tipo de elemento en Symbol
//Métodos de inicialización?


//Método para insertar un nodo que contiene un símbolo
//struct Symbol* insert( struct Symbol* actual, struct Symbol *Symb, int type ); // He cambiado el & por * en el segundo argumento por que daba error (en C no habian referencias).




// Cuando te encuentras una variable global en una definicion no haces nada, simplemente la almacenas
// en la tabla local al método. Una vez encuentras la llamada a dicho método coges y haces un recorrido
// del código en busca de las variables globales. Una vez hecho esto se cogen las variables globales
// y se suben al ámbito superior (raiz) nada mas encontrarse. 
//
//
//
//

#endif
// SYMBOLS_TABLE_H
