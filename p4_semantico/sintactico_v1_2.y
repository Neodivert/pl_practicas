/* miint.y: evaluador de lista de expresiones */
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
%token AND
%token OR
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
	| assignation
	;

method_definition : 
	DEF IDENTIF arguments_definition separator method_code END separator
	| DEF IDENTIF separator method_code END separator
	| DEF error END separator {yyerror( "Sintax error on method definition" ); yyerrok;}
	;

method_code : 
	separator
	| assignation
	| assignation method_code
	| method_call
	| method_call method_code
	| separator method_code
	| loop
	| loop method_code
	| if_construction	
	| if_construction method_code 
	/*| */
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
	'(' method_call_argument more_arguments ')'  
	| method_call_argument more_arguments 
	|'(' method_call_argument ')' 
	| method_call_argument
	;
		
method_call_argument : 	
	IDENTIF
	| literal
	| string
	;
	
more_arguments : 
	',' method_call_argument
	| ',' method_call_argument more_arguments	             
	;

block_call : 
	IDENTIF EACH DO '|' IDENTIF '|'
		method_code
	END separator
	| IDENTIF EACH error END separator {yyerror( "Sintax error on each definition" ); yyerrok;}
	;			 

loop : 
	WHILE condition DO separator
		method_code 
	END separator
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
	;

condition : 
	relational_expression
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
	IF relational_expression after_if
		method_code
		else_part
	END separator
	| IF relational_expression after_if
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

assignation : 
	ID_GLOBAL_VARIABLE '=' expression separator //Asignacion a variable global
	| ID_GLOBAL_VARIABLE '=' string separator
	| ID_GLOBAL_VARIABLE '[' expression ']' '=' expression separator 
	| ID_GLOBAL_VARIABLE '.' IDENTIF '=' expression separator 
	| ID_GLOBAL_VARIABLE '=' ID_CONSTANT NEW separator
	| ID_GLOBAL_VARIABLE '=' ARRAY NEW INTEGER separator
	| ID_GLOBAL_VARIABLE '=' '[' content_vector ']' separator
	| IDENTIF '=' expression separator  // Asignacion a variable local
	| IDENTIF '[' expression ']' '=' expression separator 
	| IDENTIF '.' IDENTIF '=' expression separator   
	| IDENTIF '=' ID_CONSTANT NEW separator
	| IDENTIF '=' string separator
	| IDENTIF '=' '[' content_vector ']' separator
	| IDENTIF '=' ARRAY NEW INTEGER separator
	| ID_CONSTANT '=' expression separator // Asignacion a constante.
	| ID_CONSTANT '=' string separator
	| ID_CONSTANT '=' '[' content_vector ']' separator 
	| ID_CONSTANT '=' ARRAY NEW INTEGER separator// Manejo de errores
	| ID_GLOBAL_VARIABLE '=' error separator {yyerror( "Sintax error on global variable assignation" ); yyerrok;}
	| ID_GLOBAL_VARIABLE '[' error separator {yyerror( "Sintax error on global variable assignation" ); yyerrok;}
	| ID_GLOBAL_VARIABLE '.' error separator {yyerror( "Sintax error on global variable assignation" ); yyerrok;}
	| ID_GLOBAL_VARIABLE '=' '[' error separator {yyerror( "Sintax error on global variable assignation" ); yyerrok;}
	| IDENTIF '=' error separator {yyerror( "Sintax error on local variable assignation" ); yyerrok;}
	| IDENTIF '[' error separator {yyerror( "Sintax error on local variable assignation" ); yyerrok;}
	| IDENTIF '.' error separator {yyerror( "Sintax error on local variable assignation" ); yyerrok;}
	| IDENTIF '=' '[' error separator {yyerror( "Sintax error on local variable assignation" ); yyerrok;}
	| ID_CONSTANT '=' error separator {yyerror( "Sintax error on constant variable assignation" ); yyerrok;}
	| ID_CONSTANT '[' error separator {yyerror( "Sintax error on constant variable assignation" ); yyerrok;}
	| ID_CONSTANT '.' error separator {yyerror( "Sintax error on constant variable assignation" ); yyerrok;}
	| ID_CONSTANT '=' '[' error separator {yyerror( "Sintax error on constant variable assignation" ); yyerrok;}	
	;
	
content_vector :   literal
                 | literal ',' content_vector
	             ;	
	             
relational_expression :
	expression relational_operator expression ;

relational_operator :
	EQUAL_EQUAL
	| LESS_EQUAL
	| GREATER_EQUAL
	| '<'
	| '>'
	| NOT_EQUAL
	;

expression :
	term
	| term '+' expression
	| term '-' expression
	| term OR expression
	| '(' term '+' expression ')'
	| '(' term '-' expression ')'
	| '(' term OR expression ')'
	;

term :
	factor
	| factor '*' term
	| factor '/' term
	| factor AND term
	| '(' term ')'
	;

factor :
	| IDENTIF
    | ID_CONSTANT
	| literal
	| NOT factor
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
		
string_struct :
		START_STRUCT ID_GLOBAL_VARIABLE END_STRUCT
		| START_STRUCT ID_CONSTANT END_STRUCT
		| START_STRUCT IDENTIF END_STRUCT
		| START_STRUCT IDENTIF '.' IDENTIF END_STRUCT
		| START_STRUCT error END_STRUCT {yyerror( "Sintax error on string interpolation" ); yyerrok;}
		;
%%

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


