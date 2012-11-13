%{
#include <stdio.h>
extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */

void yyerror(char* mens);
%}

%union { float real; int integer; }

%token <integer> INTEGER
%token <real> FLOAT 
%token CHAR
%token ID_GLOBAL_VARIABLE 
%token ID_INSTANCE_VARIABLE 
%token ID_CONSTANT 
%token IDENTIF 
%token DEF 
%token END 
%token IF 
%token THEN 
%token ELSE 
%token WHILE 
%token DO 
%token CLASS 
%token BEGIN_COMPLEX_STRING 
%token END_COMPLEX_STRING 
%token START_STRUCT 
%token END_STRUCT 
%token SUBSTRING 
%token END_LINE 
%token NOT_EQUAL 
%token EQUAL_EQUAL 
%token LESS_EQUAL
%token GREATER_EQUAL 
%token NIL 
%token BOOL 
%token SEC_SCAPE 
%token NOT
%token EACH
%token NEW
%token ARRAY

%type <real> aritmetic_expression
%type <real> numeric_literal
%type <real> term
%type <real> factor

%left '+'
%left '-'
%left '*'
%left '/'
%left AND
%left OR

%%

/* 
Idea del tío Moi - Guardar un puntero del tipo:

Symbol *currentScope;

Donde guardamos la direccion del registro para la clase / método o bloque
en la que nos encontramos actualmente. Esto nos servirá para:
  - Saber que las variables que nos encontramos son locales a ese 
    método/bloque/clase.
  - Lanzar error cuando el programador intente definir un método dentro de 
    otro. O una clase dentro de un método, etc. -> Esto ahora que lo pienso
    es cosa del sintactico, asi que nada. xD
*/

program : 
	code
	| code program
	| error program {yyerror( "Unknown sintax error" ); yyerrok;}
	| error {yyerror( "Unknown sintax error" ); yyerrok;}
	;

code : 
	method_definition
	| class_definition
	| method_call
   	| loop
   	| if_construction
	| separator
	| assignment
	;

/*
Despues de cada IDENTIF: incluir método en la tabla de símbolos. Si ya existía,
dar error (¿o permitimos sobrecarga de funciones?).
Actualizar el puntero "currentScope" que apunte al registro del método.

Al final de cada regla: poner puntero currentScope a NULL.
*/
method_definition : 
	DEF IDENTIF arguments_definition separator method_code END separator
	| DEF IDENTIF separator method_code END separator
	| DEF error END separator {yyerror( "Sintax error on method definition" ); yyerrok;}
	;

method_code : 
	separator
	| assignment
	| assignment method_code
	| method_call
	| method_call method_code
	| separator method_code
	| loop
	| loop method_code
	| if_construction	
	| if_construction method_code 
	;

/*
Despues de cada IDENTIF:
Añadir argumento de nombre IDENTIF en el metodo apuntado por "currentScope".
*/
arguments_definition : 
	'(' IDENTIF more_arguments_definition ')' 
	| IDENTIF more_arguments_definition
	;

/*
Despues de cada IDENTIF:
Añadir argumento de nombre IDENTIF en el método apuntado por "currentScope".
*/
more_arguments_definition : 
	',' IDENTIF more_arguments_definition
	|
	;

separator : 
	END_LINE 
	| ';'
	; 


/*
Después de ID_CONSTANT: incluir registro de clase con nombre ID_CONSTANT.
Actualizar "currentScope".

Después del END: poner currentScope a NULL.
*/ 
class_definition : 
	CLASS ID_CONSTANT separator
		class_content
	END separator
	|	CLASS error	END separator {yyerror( "Sintax error on class definition" ); yyerrok;}
	;

/*
Después de cada ID_INSTANCE_VARIABLE: añadir campo de nombre 
ID_INSTANCE_VARIABLE en la clase apuntada por "currentScope".
*/
class_content : 
	ID_INSTANCE_VARIABLE '=' literal 
	| ID_INSTANCE_VARIABLE '=' literal separator class_content	
	| separator class_content	
	|		
	;
			
/*
Buscar método llamado IDENTIF en el árbol.
- Si existe: "comprobar" el numero de argumentos con el de la definición (no 
  sé como se haría).
- Si no existe: crear un nodo para el método IDENTIF y que esté parcialmente
  definido.
*/
method_call : 
	IDENTIF  arguments separator
	| IDENTIF separator
	| block_call  
	| IDENTIF  error separator {yyerror( "Sintax error on method call" ); yyerrok;}
	;		

/* 1
¿Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado?
*/
arguments : 
	 method_call_argument more_arguments 
	| method_call_argument
	;

/* 2
¿Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado?
*/
method_call_argument : 	
	expression
	| string
	;
	
/* 3
¿Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado?
*/
more_arguments : 
	',' method_call_argument
	| ',' method_call_argument more_arguments	             
	;

/*
Después del primer IDENTIF: incluir en árbol y actualizar currentScope.
Después del segundo IDENTIF: incluir como argumento en currentScope.
*/
block_call : 
	IDENTIF EACH DO '|' IDENTIF '|' separator
		method_code
	END separator
	| IDENTIF EACH error END separator {yyerror( "Sintax error on each definition" ); yyerrok;}
	;			 

loop : 
	WHILE expression DO separator
		method_code 
	END separator
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
	;

after_if : 
	THEN
	| separator
	;
	
else_part : 
	ELSE separator method_code
	| ELSE separator error {yyerror( "Sintax error on else" ); yyerrok;}
	|
	;
	
if_construction : 
	IF expression after_if
		method_code
		else_part
	END separator
	| IF expression after_if
		error
		else_part
	END separator {yyerror( "Sintax error on if code" ); yyerrok;}
	| IF error after_if
		method_code
		else_part
	END separator {yyerror( "Sintax error on if condition" ); yyerrok;}
	| IF 
		error 
	END separator {yyerror( "Sintax error on if" ); yyerrok;}	
	;

/*
Buscar texto left_side en tabla de símbolos. Aqui me pierdo porque no se como
se procede con las estructuras y los arrays. ¿Se hace algo cuando se lee el 
identificador de la estructura y luego esperas a leer el identificador del
campo? ¿Se busca la variable al final cuando ya se tiene todo el nombre?
*/
assignment : 
	left_side right_side separator
	| left_side error separator {yyerror( "Sintax error on local variable assignment" ); yyerrok;}
	;

left_side :
	ID_GLOBAL_VARIABLE atribute '='
	| IDENTIF atribute '='
	| ID_CONSTANT atribute '='
	;

atribute :
	'.' IDENTIF
	| '[' expression ']'
	|
	;	

/*
¿Algo con ID_CONSTANT?
*/		
right_side :
	expression
	| string
	| ARRAY NEW INTEGER 	
	| ID_CONSTANT NEW 
	| '[' content_vector ']'
	;
		
content_vector :   
	literal
	| literal ',' content_vector
	;		      

relational_operator :
	EQUAL_EQUAL
	| LESS_EQUAL
	| GREATER_EQUAL
	| '<'
	| '>'
	| NOT_EQUAL
	;

expression :
	logical_expression
	| relational_expression OR expression
	;
	
logical_expression :
	relational_expression
	| relational_expression AND logical_expression
	;

/* Me dio por implementar la resolucion de expresiones aritmeticas y que
mostrara el resultado :D. Aunque esto no se si hacia falta en esta entrega xD
Bueno, para indexar un vector si haria falta, pero solo expresiones enteras,
y aqui las trato todas como reales */
relational_expression :
	aritmetic_expression { printf( "\n\n\n\nAritmetic expression result: %f\n", $1 ); }
	| aritmetic_expression relational_operator relational_expression
	;

aritmetic_expression :
	term { $$ = $1; }
	| term '+' aritmetic_expression { $$ = $1 + $3; }
	| term '-' aritmetic_expression { $$ = $1 - $3; }
	;

term :
	factor { $$ = $1; }
	| factor '*' term { $$ = $1 * $3; }
	| factor '/' term { $$ = $1 / $3; }
	;

factor :
	IDENTIF atribute
    | ID_CONSTANT atribute
    | ID_GLOBAL_VARIABLE atribute
	| literal
	| NOT factor
	| '(' expression ')'
	| '(' error ')' {yyerror( "Sintax error on expression" ); yyerrok;}
	;

// Separé literales numéricos y el resto para hacer pruebas.
numeric_literal :
	INTEGER { $$ = $1; printf( "\n\n\n\nValor entero %i\n", $1 ); } 
	| FLOAT { $$ = $1; }
	;

literal : 
	numeric_literal
	| CHAR
	| BOOL 
	;
	
string :
	BEGIN_COMPLEX_STRING END_COMPLEX_STRING
	| BEGIN_COMPLEX_STRING substring END_COMPLEX_STRING 
	| BEGIN_COMPLEX_STRING error END_COMPLEX_STRING {yyerror( "Sintax error on string" ); yyerrok;}
	;
	
substring :
		SUBSTRING
		| string_struct
		| SUBSTRING substring
		| string_struct substring 
		;
		
// En vez de esto mejor poner expression entre los structs. 	
// Pero hay que modificar el lexico, porque ahora solo permite una variable	
string_struct :
		START_STRUCT ID_GLOBAL_VARIABLE END_STRUCT
		| START_STRUCT ID_CONSTANT END_STRUCT
		| START_STRUCT IDENTIF END_STRUCT
		| START_STRUCT IDENTIF '.' IDENTIF END_STRUCT
		| START_STRUCT error END_STRUCT {yyerror( "Sintax error on string interpolation" ); yyerrok;}
		;
%%
//Lo que tenemos que hacer es dos bucles, en donde llamamos a yyparse
//En el primero seria recorrer arbol e ir rellenandolo, hasta que no
//haya cambios. El arbol esta compuesto de nodos con tablas de simbolos
//y desde un punto se puede acceder a las variables de de su tabla, del
//padre, el abuelo, etc. Se hacen un numero indeterminado de lecturas
//del fichero.
//En la segunda parte es la generacion de codigo, que seria volver a llamar
//a yyparse pero esta vez ya tenemos en arbol lleno.  
int main(int argc, char** argv) {
	if (argc>1) yyin=fopen(argv[1],"r");
	yyparse();
}

void yyerror(char* mens) {
	// Este if es un apaño cutre porque si no muestra muchos mensajes de error
	// que no dicen nada y de los que luego se recupera
	if(strcmp(mens,"syntax error") != 0 ) 
		printf("---------Error en linea %i: %s\n",numlin,mens);
}

/* Vano intento por reducir la parte de method code, lo pongo aqui 
	para referencias futuras
code : 
	method_definition
	| class_definition
	| method_code
	;

method_code :
	sentences
	| sentences method_code
	;
	
sentences : 
	separator
	| method_call
   	| loop
   	| if_construction
	| assignment
	;
*/

