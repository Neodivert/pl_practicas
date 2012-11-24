%{
#include <stdio.h>
#include <string.h>
#include "symbolsTable.h"
#include "semanticUtilities.h"

extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */

// We use this when defining an array to get its size.
extern unsigned int arraySize;

void yyerror(char* mens);

struct Symbol* currentMethod = NULL;
struct Symbol* currentClass = NULL; 
int nArguments = 0; 
int firstParse = 1;

%}

%union { int integer; char string[30]; struct Symbol *symbol; 
	struct MethodInfo *methodInfo; struct Method* method;
	struct SymbolInfo* symbolInfo;}

%type <symbol> expression
%type <symbol> logical_expression
%type <symbol> relational_expression
%type <symbol> aritmetic_expression
%type <symbol> term
%type <symbol> factor
%type <symbol> literal
%type <symbol> right_side
%type <symbol> simple_method_call
%type <symbol> method_call_argument
%type <symbol> assignment
%type <symbol> method_call
%type <symbol> method_code

%token <symbol> INTEGER
%token <symbol> FLOAT 
%token <symbol> CHAR

%type <symbolInfo> atribute
%type <symbolInfo> left_side
%type <symbolInfo> array_content

%type <integer> arguments_definition
%type <integer> more_arguments_definition
%type <integer> arguments
%type <integer> more_arguments
%type <integer> class_content

%type <string> relational_operator

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
%token <symbol> BOOL 
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
	DEF IDENTIF { $<methodInfo>$ = checkMethodDefinition( $2 ); } arguments_definition separator method_code END separator 
		{//printf("--------> En method def el identif vale %s\n", $2); 
			if($<methodInfo>3->result == 0)
			{
				setNArguments( $4 ); 
			}	
			goInScope($<methodInfo>3->scope);
			setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $6);			
			free($<methodInfo>3);		
		}
	| DEF IDENTIF { $<methodInfo>$ = checkMethodDefinition( $2 ); } separator method_code END separator
		{//printf("--------> En method def el identif vale %s\n", $2); 
			if($<methodInfo>3->result == 0)
			{
				setNArguments( 0 ); 
			}
			goInScope($<methodInfo>3->scope);
			setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $5);			
			free($<methodInfo>3);			
		}	
	| DEF error END separator {yyerror( "Sintax error on method definition" ); yyerrok;}
	;

method_code : 
	separator {$$ = NULL;} 
	| assignment
	| assignment method_code 
		{
			if($2 == NULL){
				$$ = $1;
			}else{
				$$ = $2;
			}
		}
	| method_call
	| method_call method_code
		{
			if($2 == NULL){
				$$ = $1;
			}else{
				$$ = $2;
			}
		}	
	| separator method_code {$$ = $2;}
	| loop {$$ = NULL;}
	| loop method_code {$$ = NULL;}
	| if_construction {$$ = NULL;}	 
	| if_construction method_code {$$ = NULL;} 
	;

/*
Despues de cada IDENTIF:
Añadir argumento de nombre IDENTIF en el metodo actual.
*/
arguments_definition : 
	'(' IDENTIF { checkArgumentDefinition($2); } more_arguments_definition ')' { $$ = 1 + $4; }
	| '(' ')' {$$ = 0;}
	//| IDENTIF more_arguments_definition {//printf("--------> En argument def el identif vale %s\n", $1);}
	;
/*
Despues de cada IDENTIF:
Añadir argumento de nombre IDENTIF en el método actual.
*/
more_arguments_definition : 
	',' IDENTIF { checkArgumentDefinition($2); } more_arguments_definition { $$ = 1 + $4; }
	| { $$ = 0; }
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
		{ 
			int i = 0;
			struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, $2 );
			if( classSymbol == NULL ){
				classSymbol = createClassSymbol($2);
				insertSymbol( classSymbol );
			}
			
			struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
			
			//Elements were counted but no yet inserted			
			if( classInfo->nElements != 0 && classInfo->elements == NULL)
			{
				currentClass = classSymbol;
				classInfo->elements = malloc(sizeof( struct Symbol *) * classInfo->nElements );
				for(i = 0; i < classInfo->nElements; i++){
					classInfo->elements[i] = NULL;
				}
			}
		}
		
		class_content END separator 
		
		{
			currentClass = NULL;
			struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, $2 );
			struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
			if( classInfo->nElements == 0 )
			{
				if( $5 != 0 ){
					classInfo->nElements = $5;
					setChanged();
				}else{
					yyerror("Type error: Classess must have at least one class variable");
				}				
			}	
		}
	|	CLASS error	END separator {yyerror( "Sintax error on class definition" ); yyerrok;}
	;

/*
Después de cada ID_INSTANCE_VARIABLE: añadir campo de nombre 
ID_INSTANCE_VARIABLE en la clase actual.
*/
class_content : 
	ID_INSTANCE_VARIABLE '=' literal  { $$ = checkClassDefinition(currentClass, $1, $3, 0); }
	| ID_INSTANCE_VARIABLE '=' literal separator class_content { $$ = checkClassDefinition(currentClass, $1, $3, $5); }
	| separator class_content {$$ = $2;}	
	| {$$ = 0;}		
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
	simple_method_call separator
	| block_call {$$ = NULL;} 
	;		

simple_method_call:  
	IDENTIF '(' { 	//printf("--------> En method call el identif vale %s\n", $1);
					currentMethod = searchTopLevel( SYM_METHOD, $1);
					//printf("--------> En method call despues\n");
					if(currentMethod && currentMethod->info )
					{
						//printf("+++++Encontre el currentmethod %s\n", currentMethod->name);
						nArguments = ((struct Method *)(currentMethod->info))->nArguments;
						//printf("+++++Tiene %d argumentos\n", nArguments);
					}
					$<symbol>$ = currentMethod;
				}			
		arguments ')' {
					  //printf("+++++SE leyeron bien %d argumentos\n", $4);
					  if(currentMethod != NULL && $4 == nArguments)
					  {
					  	//Todo fue bien
					  }
					  else
					  {
					  	yyerror("Type error: Wrong amount/undefined of arguments in method call");
					  } 
					  $$ = searchTopLevel( SYM_METHOD, $1);
					 }  
	| IDENTIF  error separator {yyerror( "Sintax error on method call" ); yyerrok;}
	;
/* 1
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
arguments : 
	 method_call_argument more_arguments 
							{ 
								////printf("+++++En arguments leidos bien %d\n", $2);
								if(currentMethod != NULL)
								{
								  	int result = checkMethodCall(currentMethod, $1, nArguments - $2);
								  	////printf("+++++El check method call dio %d\n", result);
									if(result == 0)
										$$ = $2 + 1;
									else
										$$ = -1;
								}		
							}		 
	| method_call_argument  {
								if(currentMethod != NULL)
								{	
								 	int result = checkMethodCall(currentMethod, $1, nArguments);
									if(result == 0)
										$$ = nArguments;
									else
										$$ = -1;
								}		
						   }
	| {$$ = 0;}
	;

/* 2
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
method_call_argument : 	
	expression
	| string {$$ = searchType( TYPE_STRING );}
	;
	
/* 3
Chequeo de tipos (argumento que pasas vs. argumento esperado) y de nº de 
argumentos esperado
*/
more_arguments : 
	',' method_call_argument {  
								if(currentMethod != NULL)
								{
									int result = checkMethodCall(currentMethod, $2, nArguments);
									////printf("+++++El check method call dio %d\n", result);
									if(result == 0)
										$$ = 1;
									else
										$$ = -1;
								}	 
							}
	| ',' method_call_argument more_arguments 
			{ 
				////printf("+++++En more arguments leidos bien %d\n", $3);
				if(currentMethod != NULL)
				{
				  	int result = checkMethodCall(currentMethod, $2, nArguments - $3);
				  	////printf("+++++El check method call dio %d\n", result);
					if(result == 0)
						$$ = $3 + 1;
					else
						$$ = -1;
				}		
			}	             
	;

/*
Después del primer IDENTIF: incluir en árbol.
Después del segundo IDENTIF: incluir como argumento.
*/

block_call : 
	IDENTIF EACH start_block '|' IDENTIF '|' { $<method>$ = checkBlockDefinition( $1, $5 ); } separator
		method_code
	end_block separator { goInScope($<method>7); }
	| IDENTIF EACH error END separator {yyerror( "Sintax error on each definition" ); yyerrok;}
	;			 

start_block:
	DO
	| '{'
	;

end_block:
	END
	| '}'
	;
	
loop : 
	WHILE expression DO separator
		method_code 
	END separator {checkIsBoolean($2);}
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
	;
	
if_construction : 
	IF expression after_if
		method_code
		else_part	
	END separator
				{checkIsBoolean($2);}
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
	left_side right_side separator { $$ = checkAssignement( $1, $2 ); }
	| left_side error separator {yyerror( "Sintax error on local variable assignment" ); yyerrok;}
	;

/*Aqui comprobamos si la variable existe o no
si no existe se añade a menos que atribute no sea
epsilon. En cuyo caso se debe dar un error.*/
left_side :
	ID_GLOBAL_VARIABLE atribute '=' {$2->symbol = getCreateVariable(SYM_GLOBAL, $1, $2);
									$$ = $2;}
	| IDENTIF atribute '=' {$2->symbol = getCreateVariable(SYM_VARIABLE, $1, $2);
							$$ = $2; }
	| ID_CONSTANT atribute '=' {$2->symbol = getCreateVariable(SYM_CONSTANT, $1, $2);
								$$ = $2;}
	;
	
/*Aqui se comprueba si la variable es de tipo struct y efectivamente
tiene el campo identif, o es de tipo vector y expresion es de tipo integer*/
atribute :
	'.' IDENTIF { $$ = checkClassAtribute($2);}
	| '[' expression ']' { $$ = checkIsInteger($2); }
	| { $$ = nullSymbolInfo();}
	;	

/*Si es un array new, nueva variable de tipo array, aunque no sabemos de que tipo es el contenido
del array, si es id_constant new hay que comprobar si la clase esta definida y ya sabemos que es una
nueva variable de esa clase. Con [ content ] sabemos que es de tipo array y ademas el tipo de los
datos del array.
En resumen hay que devolver el tipo: integer, boolean, clase, etc*/	
right_side :
	expression
	| string {$$ = searchType( TYPE_STRING );}
	//We save arraySize because otherwise it could be overwritten by literal
	| ARRAY NEW '(' INTEGER ',' { $<integer>$ = arraySize; } literal ')' {$$ = checkArray( $7, $<integer>6);}
	| ID_CONSTANT NEW 
		{
			//printf("--------> En assignation right side con %s new \n", $1);
			$$ = searchTopLevel( SYM_TYPE, $1);	
		}
	| '[' array_content ']' {$$ = checkArray($2->symbol, $2->info );}  
	;
			
array_content :   
	literal { $$ = nullSymbolInfo(); $$->symbol = $1; $$->info = 1; }
	| literal ',' array_content { $$ = checkArrayContent($1, $3); }
	;		      

relational_operator :
	EQUAL_EQUAL {strcpy($$, "==");}
	| LESS_EQUAL {strcpy($$,"<=");}
	| GREATER_EQUAL {strcpy($$, ">=");}
	| '<' {strcpy($$, "<");}
	| '>' {strcpy($$,">");}
	| NOT_EQUAL {strcpy($$,"!=");}
	;

/*En todas las expresiones si se usa el operador, OR
en este caso, tenemos una expresion logica, pero si no
entonces es del tipo que sea la expresion del nivel
inferior, es decir las de nivel inferior son subconjuntos
de las de nivel superior.
*/
expression :
	logical_expression //{//printf("--------> En expresion el tipo vale %s\n", $1->name);}
	| logical_expression OR expression {$$ = checkLogicalExpression($1, $3, "or");}
	;
logical_expression :
	relational_expression
	| relational_expression AND logical_expression {$$ = checkLogicalExpression($1, $3, "and");}
	;
	
relational_expression :
	aritmetic_expression
	| aritmetic_expression relational_operator relational_expression {$$ = checkRelationalExpression($1, $3, $2);}
	;

aritmetic_expression :
	term
	| term '+' aritmetic_expression {$$ = checkAritmeticExpression($1, $3, "+");}
	| term '-' aritmetic_expression {$$ = checkAritmeticExpression($1, $3, "-");}
	;
	
term :
	factor //{//printf("De factor a term tipo = %s %d \n",$1->name, ((struct Type *)($1->info))->id);}
	| factor '*' term {$$ = checkAritmeticExpression($1, $3, "*");}
	| factor '/' term {$$ = checkAritmeticExpression($1, $3, "/");}
	;

/*Comprobar que los identificadores existen y es una variable,
Si existen comprobar los atribute.
Si no existen entonces la variable no esta definida y es un error
Si es con not entonces factor tiene que ser de tipo logico
En todos los casos hay que devolver el tipo del literal/variable
*/
//TODO En realidad aqui y en literal no se deberian de crear los simbolos
//sino buscarlos en la tabla. Ademas los de variables habria que acceder a 
//la tabla a ver si existe, etc.
factor :
	IDENTIF atribute {//printf("--------> En factor el identif vale %s\n", $1);
				$$ = getVariableType( SYM_VARIABLE, $1, $2 );
				}
    | ID_CONSTANT atribute {//printf("--------> En factor el identif vale %s\n", $1);
				$$ = getVariableType( SYM_CONSTANT, $1, $2 );	
				}
    | ID_GLOBAL_VARIABLE atribute {//printf("--------> En factor el identif vale %s\n", $1);
				$$ = getVariableType( SYM_GLOBAL, $1, $2 );	
				}
	| literal 
	| NOT factor {$$ = checkNotExpression($2);}
	| simple_method_call {$$ = getReturnType($1);}
	| '(' expression ')' {$$ = $2;}
	| '(' error ')' {yyerror( "Sintax error on expression" ); yyerrok;}
	;

literal : 
	INTEGER		{ $$ = searchType( TYPE_INTEGER ); }
	| FLOAT		{ $$ = searchType( TYPE_FLOAT ); }
	| CHAR		{$$ = searchType( TYPE_CHAR ); }
	| BOOL		{$$ = searchType( TYPE_BOOLEAN );}
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
	SUBSTRING 
	| string_struct
	| SEC_SCAPE 
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
	// Inicializa la tabla de simbolos con los tipos basicos.
	initializeSymTable();

	if (argc>1) yyin=fopen(argv[1],"r");
	yyparse();

	firstParse = 0;
	int i = 1;
  	//Codigo para cada iteracion
  	while(getChange() && i < 6)
  	{ 	
  		resetChange();
  		numlin = 1;
		fclose (yyin);
		yyin=fopen(argv[1],"r");
		resetFlex();
		yyparse();
		i++;		
		printf("\n\nIteracion %d\n\n", i);
	}
	//Codigo para cuando se sale del bucle
	finishFlex();
	//printf("Termine\n");
	showSymTable();

	//printf("Termine2\n" );

	// Libera la tabla de simbolos
	freeSymbTable();
}

void yyerror(char* mens) {
	// Este if es un apaño cutre porque si no muestra muchos mensajes de error
	// que no dicen nada y de los que luego se recupera
	if(strcmp(mens,"syntax error") != 0 ) 
		//Numlin - 1 porque siempre detecta el error cuando ya paso la linea
		printf("---------Error on line %i: %s\n",numlin - 1,mens);
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

