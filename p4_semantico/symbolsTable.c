// Esto iría en el campo type de Symbol
const int VARIABLE = 1;
const int CONSTANT = 2;
const int FUNCTION = 3;
const int BLOCK = 4;

// Esto iría en el campo variableType.
const int INTEGER = 1;
const int FLOAT = 2;
const int STRING = 3;
const int CHAR = 4;
const int BOOLEAN = 5;

const int MAX_NAME_SIZE = 30; 

struct Class {
	Symbol *fields;
} class;

struct Variable {
	int variableType;
	void *value; 				// int, float, char*, bool
} variable;

struct Method {
	int nArguments;
	Symbol *localSymbols;
	//En caso de ser una definicion de método o de anónimo, tiene que tener un enlace al nodo
	//que contiene toda su información en el árbol.
	//asumo que esto es un puntero al nodo
	
	struct Method* block;
} function;


struct Symbol
{
	char name [MAX_NAME_SIZE];
	int type;
	void *info;
	struct Symbol* prev, pos;	// Posición en el padre
}; 								// *symbols;  

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
Symbol* insert(Symbol* actual, Symbol &Symb, int type)
{
	//Si es nulo lo metemos y ya
	
	//Si no es nulo tenemos que desplazar la lista e insertar el nuevo nodo
	Symbol* aux;
	aux = actual;
	Symb.prev = aux;
	return &Symb;
}

// Cuando te encuentras una variable global en una definicion no haces nada, simplemente la almacenas
// en la tabla local al método. Una vez encuentras la llamada a dicho método coges y haces un recorrido
// del código en busca de las variables globales. Una vez hecho esto se cogen las variables globales
// y se suben al ámbito superior (raiz) nada mas encontrarse. 
//
//
//
//



