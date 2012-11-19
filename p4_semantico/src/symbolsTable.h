
#ifndef SYMBOLS_TABLE_H
#define SYMBOLS_TABLE_H

const int MAX_SIZE = 30;

/*                          Super Estructura "Symbol"                         */
/******************************************************************************/
// Valores posibles para el campo "symType" de Symbol.
const int TYPE = 1;
const int VARIABLE = 2;
const int CONSTANT = 3;
const int FUNCTION = 4;
const int BLOCK = 5;

struct Symbol
{
   int symType; // symType = TYPE, VARIABLE, etc.
	char name[30];
	
	void *info;
	struct Symbol* prev;//, pos;	// Posición en el padre
}; 								// *symbols;  


/*                Estructura para los diferentes tipos de simbolos            */
/******************************************************************************/

// Valores posibles para el campo "id" de Type.
const int TYPE_INTEGER = 1;
const int TYPE_FLOAT = 2;
const int TYPE_STRING = 3;
const int TYPE_CHAR = 4;
const int TYPE_BOOLEAN = 5;
const int TYPE_CLASS = 6;

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
	
	struct Method* block;
} function;


typedef struct Symbol Symbol; 
extern Symbol* symTable;

//Asumo que ahora hay que crear el árbol

//Nodos del arbol
//Nodo raiz VS Nodo def nodo anónimo(diferencia en el tipo especificado)

	//Enlace a la tabla en el propio nodo
	//Los enlaces a los hijos son contenidos en la definición del
	//hijo dentro de la tabla de símbolos



//Al crear métodos de insercion de cada tipo de elemento en Symbol
//Métodos de inicialización?


//Método para insertar un nodo que contiene un símbolo
struct Symbol* insert( struct Symbol* actual, struct Symbol *Symb, int type ); // He cambiado el & por * en el segundo argumento por que daba error (en C no habian referencias).

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
