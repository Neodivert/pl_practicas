%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "symbolsTable.h"
#include "semanticUtilities.h"

extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */

extern int compilationState; 

// Lexical parser fill this value when it finds an integer. We use it when 
// defining an array to get its size.
extern unsigned int arraySize;

void yyerror(char* fmt, ...);

struct Symbol* currentMethodCall = NULL;
struct Symbol* currentClass = NULL; 
int nArguments = 0; 

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
			goInScope( $<methodInfo>3->scope );
			
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
	| DEF error END separator { yyerror( "Sintax error on method definition" ); yyerrok;}
	| DEF IDENTIF error END separator { goInScope(getParentScope()); yyerror( "Sintax error on method definition %s",$2); yyerrok;}
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

class_definition : 
	CLASS ID_CONSTANT separator { currentClass = checkClassDefinitonPre($2, currentClass);}
		class_content 
	END separator {currentClass = checkClassDefinitonPost( $2, $5 );}
	| CLASS error END separator {yyerror( "Sintax error on class definition"); yyerrok;}
	| CLASS ID_CONSTANT error END separator {yyerror( "Sintax error on class definition %s", $2); yyerrok;}
	;

class_content : 
	ID_INSTANCE_VARIABLE '=' literal  { $$ = checkClassContentDefinition(currentClass, $1, $3, 0); }
	| ID_INSTANCE_VARIABLE '=' literal separator class_content { $$ = checkClassContentDefinition(currentClass, $1, $3, $5); }
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
	IDENTIF '(' { 	
					currentMethodCall = searchTopLevel( SYM_METHOD, $1);					
					if(currentMethodCall && currentMethodCall->info ){						
						nArguments = ((struct Method *)(currentMethodCall->info))->nArguments;						
					}
					$<symbol>$ = currentMethodCall;
				}			
		arguments ')' {
						  if( !(currentMethodCall != NULL && $4 == nArguments) ){
						  	yyerror("Type error: Wrong or undefined amount of arguments in method call %s", $1);
						  } 
						  $$ = searchTopLevel( SYM_METHOD, $1);
					 }  
	| IDENTIF  error separator {yyerror( "Sintax error on method call %s", $1 ); yyerrok;}
	;


// arguments - semantic actions:
// Check if every argument in method call match the corresponding argument in
// method definition.
arguments : 
	 method_call_argument more_arguments 
							{ 
								if(currentMethodCall != NULL){
								  	int result = checkMethodCall(currentMethodCall, $1, nArguments - $2);
									if(result == 0){
										// Valid argument, count it.
										$$ = $2 + 1;
									}else{
										// Invalid argument.
										$$ = -1;
									}	
								}		
							}		 
	| method_call_argument  {
								if(currentMethodCall != NULL){	
								 	int result = checkMethodCall(currentMethodCall, $1, nArguments);
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
								if(currentMethodCall != NULL)
								{
									int result = checkMethodCall(currentMethodCall, $2, nArguments);
									if(result == 0){
										$$ = 1;
									}else{
										$$ = -1;
									}	
								}	 
							}
	| ',' method_call_argument more_arguments 
			{ 
				if(currentMethodCall != NULL)
				{
				  	int result = checkMethodCall(currentMethodCall, $2, nArguments - $3);
					if(result == 0){
						$$ = $3 + 1;
					}else{
						$$ = -1;
					}	
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
	| IDENTIF EACH error END separator {yyerror( "Sintax error on %s.each definition", $1 ); yyerrok;}
	| IDENTIF EACH start_block '|' IDENTIF '|' error END separator {goInScope(getParentScope()); yyerror( "Sintax error on %s.each definition",$1 ); yyerrok;}
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
	| left_side error separator {yyerror( "Sintax error on local variable %s assignment", $1->symbol->name ); freeSymbolInfo($1); $$ = NULL; yyerrok;}
	;

// Here we check if variable already exists. If not, it is added to symbols
// table, unless attribute is epsilon. In that case, an error must be given.
left_side :
	ID_GLOBAL_VARIABLE atribute '=' { $2->symbol = getCreateVariable(SYM_GLOBAL, $1, $2);
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
	| ID_CONSTANT NEW {	$$ = searchTopLevel( SYM_TYPE, $1);	}
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
	logical_expression 
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
	factor 
	| factor '*' term {$$ = checkAritmeticExpression($1, $3, "*");}
	| factor '/' term {$$ = checkAritmeticExpression($1, $3, "/");}
	;

factor :
	IDENTIF atribute {$$ = getVariableType( SYM_VARIABLE, $1, $2 );	}
    | ID_CONSTANT atribute {$$ = getVariableType( SYM_CONSTANT, $1, $2 );}
    | ID_GLOBAL_VARIABLE atribute {	$$ = getVariableType( SYM_GLOBAL, $1, $2 );	}
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
	
//TODO Lexical analizer does not allow expression on strings, so here we are only getting
//simple variables
string_struct :
		START_STRUCT expression END_STRUCT
		| START_STRUCT error END_STRUCT {yyerror( "Sintax error on string interpolation" ); yyerrok;}
		;
%%
  
int main(int argc, char** argv) {

	int i = 1;	
	compilationState = 0;
	initializeSymTable();
	struct Method *mainScope = getCurrentScope();

	if (argc>1)yyin=fopen(argv[1],"r");
	yyparse();	

  	while(getChange() && i < 20)
  	{ 	
  		resetChange();
  		numlin = 1;
		fclose (yyin);
		yyin=fopen(argv[1],"r");
		
		//In case something went really wrong
		//set current scope to main again
		//This should not be necessary
		goInScope(mainScope);
		
		yyparse();
		i++;		
	}
	
	if (argc>2)printf("\nSintax analyzer needed %d iterations\n", i);
	
	compilationState = 1;

	numlin = 1;
	fclose (yyin);
	yyin=fopen(argv[1],"r");
	
	goInScope(mainScope);
	
	yyparse();
	
	compilationState = 2;
	
	if (argc>2)showSymTable();
	
	freeSymbTable();
}

void yyerror(char* fmt, ...)
{
    va_list args;
	//Syntax error alone gives no information, ignore it
	if(strcmp(fmt,"syntax error") != 0 && compilationState == 1){
		//We printf numlin - 1 because lexical analizer is ahead one or more lines 
		printf("---------Error on line %i: ",numlin - 1);       
		va_start(args,fmt);	 
		vprintf(fmt,args);
		va_end(args);
		printf("\n");
	}	
}

