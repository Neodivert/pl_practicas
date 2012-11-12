%{
#include <stdio.h>
extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
int yydebug=1; /* modo debug si -t */

void yyerror(char* mens);
%}

%token INTEGER 
%token FLOAT 
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

arguments_definition : 
	'(' IDENTIF more_arguments_definition ')' 
	| IDENTIF more_arguments_definition
	;
	
more_arguments_definition : 
	',' IDENTIF more_arguments_definition
	|
	;

separator : 
	END_LINE 
	| ';'
	; 

class_content : 
	ID_INSTANCE_VARIABLE '=' literal 
	| ID_INSTANCE_VARIABLE '=' literal separator class_content	
	| separator class_content	
	|		
	;
				 
class_definition : 
	CLASS ID_CONSTANT separator
		class_content
	END separator
	|	CLASS error	END separator {yyerror( "Sintax error on class definition" ); yyerrok;}
	;
  
method_call : 
	IDENTIF  arguments separator
	| IDENTIF separator
	| block_call  
	| IDENTIF  error separator {yyerror( "Sintax error on method call" ); yyerrok;}
	;		
		
arguments : 
	 method_call_argument more_arguments 
	| method_call_argument
	;
	
method_call_argument : 	
	expression
	| string
	;
	
more_arguments : 
	',' method_call_argument
	| ',' method_call_argument more_arguments	             
	;

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
		
relational_expression :
	aritmetic_expression
	| aritmetic_expression relational_operator relational_expression
	;
	
aritmetic_expression :
	term
	| term '+' aritmetic_expression
	| term '-' aritmetic_expression
	;

term :
	factor
	| factor '*' term
	| factor '/' term
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

literal : 
	INTEGER 
	| FLOAT 
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
		
//En vez de esto mejor poner expression entre los structs. 	
//Pero hay que modificar el lexico, porque ahora solo permite una variable	
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

