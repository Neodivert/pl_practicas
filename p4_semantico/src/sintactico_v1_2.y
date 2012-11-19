%{
#include <stdio.h>
#include <string.h>
extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */

void yyerror(char* mens);

struct Symbol{
char name [30];
} symbolTable;

%}

%union { char string[30]; struct Symbol *symbol;}

%name <symbol> expression
%name <symbol> logical_expression
%name <symbol> relational_expression
%name <symbol> aritmetic_expression
%name <symbol> term
%name <symbol> factor
%name <symbol> literal

%token <symbol> INTEGER
%token <symbol> FLOAT 
%token <symbol> CHAR
%token <string> ID_GLOBAL_VARIABLE 
%token <string> ID_INSTANCE_VARIABLE 
%token <string> ID_CONSTANT 
%token <string> IDENTIF 
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
%token <string> SUBSTRING 
%token END_LINE 
%token NOT_EQUAL 
%token EQUAL_EQUAL 
%token LESS_EQUAL
%token GREATER_EQUAL 
%token NIL 
%token <Symbol *> BOOL 
%token <string> SEC_SCAPE 
%token NOT
%token EACH
%token NEW
%token ARRAY

%left '+'
%left '-'
%left '*'
%left '/'
%left AND
%left OR

%%

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

Al final de cada regla: poner puntero currentScope a NULL.

No vamos a permitir sobrecarga de metodos.
*/
method_definition : 
	DEF IDENTIF arguments_definition separator method_code END separator {printf("--------> En method def el identif vale %s\n", $2);}
	| DEF IDENTIF separator method_code END separator {printf("--------> En method def el identif vale %s\n", $2);}
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
Añadir argumento de nombre IDENTIF en el metodo actual.
*/
arguments_definition : 
	'(' IDENTIF more_arguments_definition ')' {printf("--------> En argument def el identif vale %s\n", $2);}
	| IDENTIF more_arguments_definition {printf("--------> En argument def el identif vale %s\n", $1);}
	;

/*
Despues de cada IDENTIF:
Añadir argumento de nombre IDENTIF en el método actual.
*/
more_arguments_definition : 
	',' IDENTIF more_arguments_definition {printf("--------> En argument def el identif vale %s\n", $2);}
	|
	;

separator : 
	END_LINE 
	| ';'
	; 


/*
Después de ID_CONSTANT: incluir registro de clase con nombre ID_CONSTANT.
*/ 
class_definition : 
	CLASS ID_CONSTANT separator
		class_content
	END separator {printf("--------> En class def el identif vale %s\n", $2);}
	|	CLASS error	END separator {yyerror( "Sintax error on class definition" ); yyerrok;}
	;

/*
Después de cada ID_INSTANCE_VARIABLE: añadir campo de nombre 
ID_INSTANCE_VARIABLE en la clase actual.
*/
class_content : 
	ID_INSTANCE_VARIABLE '=' literal {printf("--------> En class content el identif vale %s\n", $1);}
	| ID_INSTANCE_VARIABLE '=' literal separator class_content {printf("--------> En class content el identif vale %s\n", $1);}
	| separator class_content	
	|		
	;
			
/*
Buscar método llamado IDENTIF en el árbol.
- Si existe: "comprobar" el numero de argumentos con el de la definición (no 
  sé como se haría) O tenemos un campo en el struct que sea numero de argumentos o
  los contamos cada vez que se hace una llamada yendo a la tabla de simbolos.
- Si no existe: no se hace nada, a la espera de encontrar la definicion mas adelante
 y en una pasada posterio ya lo leeremos. Si fuera un error ya se detectara despues
   al no estar el metodo en la tabla de simbolos.
*/
method_call : 
	IDENTIF  arguments separator {printf("--------> En method call el identif vale %s\n", $1);}
	| IDENTIF separator {printf("--------> En method call el identif vale %s\n", $1);}
	| block_call  
	| IDENTIF  error separator {yyerror( "Sintax error on method call" ); yyerrok;}
	;		

/* 1
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
arguments : 
	 method_call_argument more_arguments 
	| method_call_argument
	;

/* 2
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
method_call_argument : 	
	expression
	| string
	;
	
/* 3
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
more_arguments : 
	',' method_call_argument
	| ',' method_call_argument more_arguments	             
	;

/*
Después del primer IDENTIF: incluir en árbol.
Después del segundo IDENTIF: incluir como argumento.
*/
block_call : 
	IDENTIF EACH DO '|' IDENTIF '|' separator
		method_code
	END separator {printf("--------> En block call el identif vale %s %s\n", $1, $5);}
	| IDENTIF EACH error END separator {yyerror( "Sintax error on each definition" ); yyerrok;}
	;			 

loop : 
	WHILE expression DO separator
		method_code 
	END separator
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
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
	
after_if : 
	THEN
	| separator
	;
	
else_part : 
	ELSE separator method_code
	| ELSE separator error {yyerror( "Sintax error on else" ); yyerrok;}
	|
	;	

/*
Buscar texto left_side en tabla de símbolos. 
Si la variable no existe y tiene un tipo definido, entonces se crea una nueva 
entrada con nombre sacado de left_side y tipo sacado de right side.
Si ya existe entonces se debe comprobar que los tipos de la variable
y del right side coincidan.
*/
assignment : 
	left_side right_side separator
	| left_side error separator {yyerror( "Sintax error on local variable assignment" ); yyerrok;}
	;

/*Aqui comprobamos si la variable existe o no
si no existe se añade a menos que atribute no sea
epsilon. En cuyo caso se debe dar un error.*/
left_side :
	ID_GLOBAL_VARIABLE atribute '=' {printf("--------> En assignation left side el identif vale %s\n", $1);}
	| IDENTIF atribute '=' {printf("--------> En assignation left side el identif vale %s\n", $1);}
	| ID_CONSTANT atribute '=' {printf("--------> En assignation left side el identif vale %s\n", $1);}
	;
	
/*Aqui se comprueba si la variable es de tipo struct y efectivamente
tiene el campo identif, o es de tipo vector y expresion es de tipo integer*/
atribute :
	'.' IDENTIF {printf("--------> En assignation left side atribute el identif vale %s\n", $2);}
	| '[' expression ']'
	|
	;	

/*Si es un array new, nueva variable de tipo array, aunque no sabemos de que tipo es el contenido
del array, si es id_constant new hay que comprobar si la clase esta definida y ya sabemos que es una
nueva variable de esa clase. Con [ content ] sabemos que es de tipo array y ademas el tipo de los
datos del array.
En resumen hay que devolver el tipo: integer, boolean, clase, etc*/	
right_side :
	expression
	| string
	| ARRAY NEW INTEGER 	
	| ID_CONSTANT NEW {printf("--------> En assignation right side el identif vale %s\n", $1);}
	| '[' content_vector ']'
	;
	
/*Comprobar que todos los literales son del mismo tipo*/		
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

/*En todas las expresiones si se usa el operador, OR
en este caso, tenemos una expresion logica, pero si no
entonces es del tipo que sea la expresion del nivel
inferior, es decir las de nivel inferior son subconjuntos
de las de nivel superior.
En la segunda regla habria que comprobar que tanto relational_expresion como
expresion son de tipo boolean*/
expression :
	logical_expression {printf("--------> En expresion el tipo vale %s\n", $1->name);}
	| logical_expression OR expression
	;
/*En la segunda regla habria que comprobar que tanto relational_expresion como
expresion son de tipo boolean*/	
logical_expression :
	relational_expression
	| relational_expression AND logical_expression
	;

/*En la segunda regla habria que comprobar que tanto artimetic_expresion como
relational expresion son de tipo integer*/	
relational_expression :
	aritmetic_expression
	| aritmetic_expression relational_operator relational_expression {strcpy($1->name, "bool");}
	;

/*Comprobar que el tipo de term y de aritmetic expresion son compatibles:
integer integer o float float*/
aritmetic_expression :
	term
	| term '+' aritmetic_expression
	| term '-' aritmetic_expression
	;
	
/*Comprobar que el tipo de term y de aritmetic expresion son compatibles:
integer integer o float float*/
term :
	factor
	| factor '*' term {int t1, t2;
						t1 = (Type *)($1->info)->id;
						t2 = (Type *)($1->info)->id);
						//Factor and term are both integer or float.
						if(t1 == t2 && t1 <= FLOAT){
							$$ = $1;
						}else{
							yyerror("Type error %s and %s do not match or is not\ 
							integer or float");
						} }
	| factor '/' term
	;

/*Comprobar que los identificadores existen y es una variable,
Si existen comprobar los atribute.
Si no existen entonces la variable no esta definida y es un error
Si es con not entonces factor tiene que ser de tipo logico
En todos los casos hay que devolver el tipo del literal/variable
*/
factor :
	IDENTIF atribute {printf("--------> En factor el identif vale %s\n", $1); 
				struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "integer" );
				t.id = INTEGER; 
				S.info = (void *)&t; 
				$$ = &S;}
    | ID_CONSTANT atribute {printf("--------> En factor el identif vale %s\n", $1);
				struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "integer" );
				t.id = INTEGER; 
				S.info = (void *)&t; 
				$$ = &S;
    | ID_GLOBAL_VARIABLE atribute {printf("--------> En factor el identif vale %s\n", $1);
				struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "integer" );
				t.id = INTEGER; 
				S.info = (void *)&t; 
				$$ = &S;
	| literal {printf("--------> En factor el typo del literal vale %s\n", $1->name); $$ = $1;}
	| NOT factor {$$ = $2;}
	| '(' expression ')' {$$ = $2;}
	| '(' error ')' {yyerror( "Sintax error on expression" ); yyerrok;}
	;

literal : 
	INTEGER { 	struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "integer" );
				t.id = INTEGER; 
				S.info = (void *)&t; 
				$$ = &S; }
	| FLOAT{ 	struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "float" );
				t.id = FLOAT; 
				S.info = (void *)&t; 
				$$ = &S; }
	| CHAR{ 	struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "char" );
				t.id = CHAR; 
				S.info = (void *)&t; 
				$$ = &S; }
	| BOOL { 	struct Symbol S; 
				struct Type t; 
				strcpy( S.name, "boolean" );
				t.id = BOOLEAN; 
				S.info = (void *)&t; 
				$$ = &S; }
	;
	
string :
	BEGIN_COMPLEX_STRING END_COMPLEX_STRING 
	| BEGIN_COMPLEX_STRING substring END_COMPLEX_STRING 
	| BEGIN_COMPLEX_STRING error END_COMPLEX_STRING {yyerror( "Sintax error on string" ); yyerrok;}
	;
	
substring :
	substring_part
	| substring_part substring
	;
	
substring_part :
	SUBSTRING {printf("--------> En substring el identif vale %s\n", $1);}
	| string_struct
	| SEC_SCAPE {printf("--------> En sec scape el identif vale %s\n", $1);}
	;
	
// Hay que modificar el lexico, porque por ahora solo permite una variable	
// Como aqui esta expresion, ni hay que comprobar k la variable existe ni na,
// todo eso ya se hizo en expresion
string_struct :
		START_STRUCT expression END_STRUCT
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
		printf("---------Error on line %i: %s\n",numlin,mens);
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

