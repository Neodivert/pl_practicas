%{
#include <stdio.h>
#include <string.h>
#include "symbolsTable.h"
#include "semanticUtilities.h"

extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */

// Lexical parser fill this value when it finds an integer. We use it when 
// defining an array to get its size.
extern unsigned int arraySize;

void yyerror(char* mens);

struct Symbol* currentMethod = NULL;
struct Symbol* currentClass = NULL; 
int nArguments = 0; 
int firstParse = 1;

%}

// Possible data returned by a token or no terminal.
%union { int integer; char string[30]; struct Symbol *symbol; 
	struct MethodInfo *methodInfo; struct Method* method;
	struct SymbolInfo* symbolInfo;}

// No terminals returning a value.
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
%type <symbolInfo> atribute
%type <symbolInfo> left_side
%type <symbolInfo> array_content
%type <integer> arguments_definition
%type <integer> more_arguments_definition
%type <integer> arguments
%type <integer> more_arguments
%type <integer> class_content
%type <string> relational_operator

// Tokens
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
%token <symbol> BOOL 
%token <string> SEC_SCAPE 
%token NOT
%token EACH
%token NEW
%token ARRAY

// Operators precedence
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

// Method definition - Semantic actions:
// checkMethodDefinition search for a method IDENTIF in symbols' table and
// create it if didn't exist. This function return an struct MethodInfo with
// a pointer to method's info (scope) and an integer (result) which indicates
// if method was already in symbols table (1) or not (0).
method_definition : 
	DEF IDENTIF { $<methodInfo>$ = checkMethodDefinition( $2 ); } arguments_definition separator method_code END separator 
		{	if($<methodInfo>3->result == 0){
				// If method wasn't already in symbols' table, set its number
				// of arguments.
				setNArguments( $4 ); 
			}
			goInScope($<methodInfo>3->scope);
			setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $6);		
			free($<methodInfo>3);		
		}
	| DEF IDENTIF { $<methodInfo>$ = checkMethodDefinition( $2 ); } separator method_code END separator
		{
			if($<methodInfo>3->result == 0){
				// If method wasn't already in symbols' table, set its number
				// of arguments.
				setNArguments( 0 ); 
			}
			goInScope($<methodInfo>3->scope);
			setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $5);		
			free($<methodInfo>3);			
		}
	| DEF error END separator {yyerror( "Sintax error on method definition" ); yyerrok;}
	;

// An Emerald's method return a value if its last sentence is an assignment or
// a method call. If method's last sentence is like that, we return its type
// symbol in $$, otherwise we return NULL.
method_code : 
	separator { $$ = NULL; } 
	| assignment
	| assignment method_code { $$ = $2 ? $2 : $1; }
	| method_call
	| method_call method_code { $$ = $2 ? $2 : $1; }	
	| separator method_code {$$ = $2;}
	| loop {$$ = NULL;}
	| loop method_code {$$ = NULL;}
	| if_construction {$$ = NULL;}	 
	| if_construction method_code {$$ = NULL;} 
	;

// checkArgumentDefinition insert argument's symbol with name $2 in symbols
// table (if it doesn't exist yet).
// In $$ we return the number of defined arguments (integer).
arguments_definition : 
	'(' IDENTIF { checkArgumentDefinition($2); } more_arguments_definition ')' { $$ = 1 + $4; }
	| '(' ')' {$$ = 0;}
	;

// checkArgumentDefinition insert argument's symbol with name $2 in symbols
// table (if it doesn't exist yet).
// In $$ we return the number of defined arguments (integer).
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

// In $$ we return symple_method_call return type, or NULL, if we have a
// block_call. See method_code.
method_call : 
	simple_method_call separator
	| block_call {$$ = NULL;} 
	;		

// Check if we are making a correct call (same number or arguments) to a defined 
// method.
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


// arguments - semantic actions:
// Check if every argument in method call match the corresponding argument in
// method definition.
arguments : 
	 method_call_argument more_arguments 
							{ 
							printf("llege0\n");
								if(currentMethod != NULL){
								  	int result = checkMethodCall(currentMethod, $1, nArguments - $2);
									if(result == 0)
										// Valid argument, count it.
										$$ = $2 + 1;
									else
										// Invalid argument.
										$$ = -1;
								}		
							}		 
	| method_call_argument  {
		printf("llege\n");
								if(currentMethod != NULL){	
								 	int result = checkMethodCall(currentMethod, $1, nArguments);
									if(result == 0){
										// Valid argument, count it.
										$$ = nArguments;
									}else{
										// Invalid argument.
										$$ = -1;
									}
								}
								
						   }
	| {$$ = 0;}
	;

// In $$ we return a pointer to method's type symbol.
method_call_argument : 	
	expression
	| string {$$ = searchType( TYPE_STRING );}
	;
	
// more_arguments - semantic actions:
// Check if every argument in method call match the corresponding argument in
// method definition.
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

// Block call - Semantic actions:
// checkBlockDefinition search for block in symbols' table and create it if
// doens't exist.
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

// While loop. 
// Semantic verifications: expression must return a boolean. 
loop : 
	WHILE expression DO separator
		method_code 
	END separator {checkIsBoolean($2);}
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
	;

// If construction.
// Semantic verifications: expression must return a boolean.
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

// checkAssignement search left_side in the symbols table.
// If the variable doens't exist and it has a known type, then a new variable
// symbol is created with name of right_side and type of left_side.
// If the variable already existed, check if the types of variable and right
// side match.
assignment : 
	left_side right_side separator { $$ = checkAssignement( $1, $2 ); }
	| left_side error separator {yyerror( "Sintax error on local variable assignment" ); yyerrok;}
	;

// Here we check if variable already exists. If not, it is added to symbols
// table, unless attribute is epsilon. In that case, an error must be given.
left_side :
	ID_GLOBAL_VARIABLE atribute '=' {$2->symbol = getCreateVariable(SYM_GLOBAL, $1, $2);
									$$ = $2;}
	| IDENTIF atribute '=' {$2->symbol = getCreateVariable(SYM_VARIABLE, $1, $2);
							$$ = $2; }
	| ID_CONSTANT atribute '=' {$2->symbol = getCreateVariable(SYM_CONSTANT, $1, $2);
								$$ = $2;}
	;
	
// Here we check if variable is of type struct and it actually has the field
// "identif", or if variable is of type array, and expression is of type
// integer.
atribute :
	'.' IDENTIF { $$ = checkClassAtribute($2);}
	| '[' expression ']' { $$ = checkIsInteger($2); }
	| { $$ = nullSymbolInfo();}
	;	

// right_side returns its type (integer, boolean, array, class, etc).	
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

// Here we check if all the array content has the same type.		
array_content :   
	literal { $$ = nullSymbolInfo(); $$->symbol = $1; $$->info = 1; }
	| literal ',' array_content { $$ = checkArrayContent($1, $3); }
	;		      


// Return in a string the operators "name". This is useful for showing an error
// message if an error is detected. 
relational_operator :
	EQUAL_EQUAL {strcpy($$, "==");}
	| LESS_EQUAL {strcpy($$,"<=");}
	| GREATER_EQUAL {strcpy($$, ">=");}
	| '<' {strcpy($$, "<");}
	| '>' {strcpy($$,">");}
	| NOT_EQUAL {strcpy($$,"!=");}
	;

// In all the expressions, if an operator is used, current expression becomes
// its operator's type, otherwise type does not change
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
factor :
	IDENTIF atribute {//printf("--------> En factor el identif vale %s\n", $1);
			printf("--------------------------------------Estoy en la linea %d\n", numlin);
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

