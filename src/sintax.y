%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "symbolsTable.h"
#include "semanticUtilities.h"
#include "codeGenUtils.h"
#include <errno.h>

extern FILE *yyin; /* declarado en lexico */
extern int numlin; /* lexico le da valores */
//extern int yylex();
int yydebug=1; /* modo debug si -t */
FILE *yyout; /*fichero compilado*/

extern struct ExtraInfo* extraInfoPerRegister[8];
int nextRegisterOverflow = 0;

extern int compilationState; 
int errors = 0;
// Lexical parser fill this value when it finds an integer. We use it when 
// defining an array to get its size.
extern int arraySize;

extern float floatVal;

void yyerror(char* fmt, ...);

struct Symbol* currentMethodCall = NULL;
struct Symbol* currentClass = NULL; 
int nArguments = 0; 

int nextCodeLabel = 0;

int insideIfLoop = 0;

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
%type <string> string
%type <string> substring
%type <string> substring_part
%type <string> string_struct
%type <integer> get;

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
%token PUTS
%token GETI
%token GETF
%token GETC

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
	| method_call 	{ GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
				EGC}
   	| loop
   	| if_construction
	| separator
	| puts
	| assignment { GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
				EGC}
	;

// Method definition - Semantic actions:
// checkMethodDefinition search for a method IDENTIF in symbols' table and
// create it if didn't exist. This function return an struct MethodInfo with
// a pointer to method's info (scope) and an integer (result) which indicates
// if method was already in symbols table (1) or not (0).
method_definition : 
	DEF IDENTIF { GC nextCodeLabel = newLabel(); fprintf( yyout,"\tGT(%d); //Jump to next code\n", nextCodeLabel); EGC $<methodInfo>$ = checkMethodDefinition( $2 ); } 
	arguments_definition { GC genMethodBegin( yyout, $2 ); EGC; } 
	separator method_code END separator 
		{	NGC 
				if($<methodInfo>3->result == 0){
					// If method wasn't already in symbols' table, set its number
					// of arguments.
					setNArguments( $4 ); 
				} 
			
				setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $7);
			ENGC
			goInScope( $<methodInfo>3->scope );
			GC 
				genMethodEnd( yyout, $2 ); 
				fprintf( yyout,"L %d: //Continue code\n", nextCodeLabel);
			EGC

			free($<methodInfo>3);
		}
	| DEF IDENTIF {  $<methodInfo>$ = checkMethodDefinition( $2 ); 
		GC 
			nextCodeLabel = newLabel(); 
			fprintf( yyout,"\tGT(%d); //Jump to next code\n", nextCodeLabel);
			genMethodBegin( yyout, $2 );
			
		EGC } 
		separator method_code END separator
		{
			if($<methodInfo>3->result == 0){
				// If method wasn't already in symbols' table, set its number
				// of arguments.
				setNArguments( 0 ); 
			}
			goInScope($<methodInfo>3->scope);
			setMethodReturnType(searchTopLevel( SYM_METHOD, $2), $5);	

			GC 
				genMethodEnd( yyout, $2 ); 
				fprintf( yyout,"L %d: //Continue code\n", nextCodeLabel);
			EGC
	
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
	| assignment { GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
					$$ = NULL;
				EGC}
	| assignment method_code { NGC $$ = $2 ? $2 : $1; ENGC 
				GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
					$$ = NULL;
				EGC}
	| method_call { GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
					$$ = NULL;
				EGC}
	| method_call method_code { NGC $$ = $2 ? $2 : $1; ENGC 
				GC  					
					if($1->symType == SYM_EXTRA_INFO ){
						freeSymbol($1); 
					}	
					$$ = NULL;
				EGC}	
	| separator method_code {$$ = $2;}
	| loop {$$ = NULL;}
	| loop method_code {$$ = $2;}
	| if_construction {$$ = NULL;}	 
	| puts {$$ = NULL;}
	| puts method_code {$$ = $2;}
	| if_construction method_code {$$ = $2;} 
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
	IDENTIF '(' { 	currentMethodCall = searchTopLevel( SYM_METHOD, $1);				
					if(currentMethodCall && currentMethodCall->info ){						
						nArguments = ((struct Method *)(currentMethodCall->info))->nArguments;						
					}
					$<symbol>$ = currentMethodCall;

					GC genMethodCallBegin( yyout, $1 ); EGC
				}			
		arguments ')' { NGC $$ = checkMethodCall( $1, nArguments, $4, currentMethodCall);ENGC 
			GC 
				int reg = assignRegisters(0); 
				$$ = createExtraInfoSymbol(reg);
				genMethodCall( yyout, (struct Method* )(currentMethodCall->info), reg ); 
				if(!insideIfLoop && ((struct Method *)(currentMethodCall->info))->returnType){
					struct Method* method = getCurrentScope();					
					if(method->returnType){	
						int size = method->argumentsSize;									
						fprintf(yyout,"\t%c(R6+%d) = R%d; //Store return value\n",
							pointerType(method->returnType), size, reg);
					}
				}				
			EGC }  
	| IDENTIF  error separator {yyerror( "Sintax error on method call %s", $1 ); yyerrok; $$ = NULL;}
	;


// arguments - semantic actions:
// Check if every argument in method call match the corresponding argument in
// method definition.
arguments : 
	 method_call_argument
	 {
		GC
			nArguments--;
			genArgumentPass( yyout, $1, currentMethodCall, nArguments );
		EGC	 
	 }
	  more_arguments 
							{ 
								NGC
								if(currentMethodCall != NULL){
								  	int result = checkMethodCallArguments(currentMethodCall, $1, nArguments - $3);
									if(result == 0){
										// Valid argument, count it.
										$$ = $3 + 1;
									}else{
										// Invalid argument.
										$$ = -1;
									}	
								}
								ENGC

							}		 
	| method_call_argument  {
								NGC
								if(currentMethodCall != NULL){	
								 	int result = checkMethodCallArguments(currentMethodCall, $1, nArguments);
									if(result == 0){
										// Valid argument, count it.
										$$ = nArguments;
									}else{
										// Invalid argument.
										$$ = -1;
									}
								}
								ENGC
								GC
									genArgumentPass( yyout, $1, currentMethodCall, 0 );
								EGC
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

/*
struct ExtraInfo {
	int nRegister;
	struct Symbol* variable; 
};
*/

// FIXME: Cuando use la info que devuelve expression, liberarla.
more_arguments : 
	',' method_call_argument {  
								NGC
								if(currentMethodCall != NULL)
								{
									int result = checkMethodCallArguments(currentMethodCall, $2, nArguments);
									if(result == 0){
										$$ = 1;
									}else{
										$$ = -1;
									}	
								}
								ENGC
								GC
									nArguments--;
									genArgumentPass( yyout, $2, currentMethodCall, nArguments);
								EGC 
							}
	| ',' method_call_argument
	{
	GC 
		nArguments--;
		genArgumentPass( yyout, $2, currentMethodCall, nArguments );
	EGC			
	}
	 more_arguments 
			{ 
				NGC
				if(currentMethodCall != NULL)
				{
				  	int result = checkMethodCallArguments(currentMethodCall, $2, nArguments - $4);
					if(result == 0){
						$$ = $4 + 1;
					}else{
						$$ = -1;
					}
				}
				ENGC
			}	             
	;

// Block call - Semantic actions:
// checkBlockDefinition search for block in symbols' table and create it if
// doens't exist.
block_call : 
	IDENTIF EACH start_block '|' IDENTIF '|' { $<method>$ = checkBlockDefinition( $1, $5 ); GC insideIfLoop++; EGC } separator
		method_code
	end_block separator { goInScope($<method>7); insideIfLoop--;}
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
	WHILE {GC $<integer>$=newLabel(); fprintf(yyout,"L %d:\n", $<integer>$); insideIfLoop++; EGC ;}
	expression DO {GC 
					$<integer>$=newLabel(); 
					fprintf(yyout,"\tIF(!R%d) GT(%d);\t//begin LOOP\n",((struct ExtraInfo*)($3->info))->nRegister,$<integer>$);
					freeRegister( ((struct ExtraInfo*)($3->info))->nRegister, 0 );
					if($3->symType == SYM_EXTRA_INFO ){
						freeSymbol($3); 
					}					
				   EGC;}
	separator
		method_code { GC fprintf(yyout,"\tGT(%d);\nL %d:\t//END LOOP\n",$<integer>2,$<integer>5); EGC ;}
	END separator { NGC checkIsBoolean($3); ENGC GC insideIfLoop--; EGC }
	| 	WHILE error END separator {yyerror( "Sintax error on while loop" ); yyerrok;}
	;

// FIXME: Cuando me equivoque y escribi el nombre de una variable que no existia
// en la condicion, me dio un fallo de segmentacion (moi).
// If construction.
// Semantic verifications: expression must return a boolean.
if_construction : 
	IF expression after_if {GC 
								$<integer>$ = newLabel(); 
								fprintf(yyout,"\tIF(!R%d) GT(%d);\t//we check the condition\n",((struct ExtraInfo*)($2->info))->nRegister,$<integer>$);
								freeRegister( ((struct ExtraInfo*)($2->info))->nRegister, 0 );
								if($2->symType == SYM_EXTRA_INFO ){
									freeSymbol($2); 
								}
								insideIfLoop++;
							EGC	
							;}
		method_code 
		else_part {GC
					if($<integer>6==0){
						fprintf(yyout,"L %d:\t//tag for END IF\n",$<integer>4);
					}
				   EGC;}//El else_part deberá crear su propio código
	END separator
				{ NGC checkIsBoolean($2); ENGC GC insideIfLoop--; EGC}
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
	ELSE separator {GC 
						$<integer>$ = newLabel(); fprintf(yyout,"\tGT(%d);\nL %d:\t//Tag of Else part\n", $<integer>$, $<integer>-1);
					EGC
					;}
	method_code {GC
					fprintf(yyout,"L %d:\t//Tag for END IF\n",$<integer>3);
				EGC	;}
	| ELSE separator error {yyerror( "Sintax error on else" ); yyerrok;}
	| {$<integer>$ = 0;}
	;	

// checkAssignement search left_side in the symbols table.
// If the variable doens't exist and it has a known type, then a new variable
// symbol is created with name of right_side and type of left_side.
// If the variable already existed, check if the types of variable and right
// side match.
assignment : 
	left_side right_side separator { NGC $$ = checkAssignement( $1, $2 ); ENGC
									GC $$ = genAssignement(yyout, $1, $2, insideIfLoop); EGC }

	| left_side error separator {yyerror( "Sintax error on local variable %s assignment", $1->symbol->name ); freeSymbolInfo($1); $$ = NULL; yyerrok;}
	;

// Here we check if variable already exists. If not, it is added to symbols
// table, unless attribute is epsilon. In that case, an error must be given.
left_side :
	ID_GLOBAL_VARIABLE atribute '=' { NGC $2->symbol = getCreateVariable(SYM_GLOBAL, $1, $2); ENGC
									GC
										$2->varSymbol = searchVariable(SYM_GLOBAL,(cstr)$1);
										if($2->info == SYM_CLASS_VARIABLE){
											//varSymbol gets the struct Symbol of the variable
											$2->varSymbol = getClassVar($2->varSymbol,$2->name);
										}
									EGC;
									$$ = $2;}
	| IDENTIF atribute '=' { NGC $2->symbol = getCreateVariable(SYM_VARIABLE, $1, $2); ENGC
									GC
										$2->varSymbol = searchVariable(SYM_VARIABLE,(cstr)$1);
										if($2->info == SYM_CLASS_VARIABLE){
											//varSymbol gets the struct Symbol of the variable
											$2->varSymbol = getClassVar($2->varSymbol,$2->name);
										}
									EGC;
							$$ = $2; }
	| ID_CONSTANT atribute '=' {$2->symbol = getCreateVariable(SYM_CONSTANT, $1, $2);
								$$ = $2;}
	;
	
// Here we check if variable is of type struct and it actually has the field
// "identif", or if variable is of type array, and expression is of type
// integer.
atribute :
	'.' IDENTIF { $$ = checkClassAtribute($2);}
	| '[' expression ']' 
		{ $$ = checkIsInteger($2);
		  GC $$->exprSymbol = $2; EGC
		}
	| { $$ = nullSymbolInfo();}
	;	

// right_side returns its type (integer, boolean, array, class, etc).	
right_side :
	expression
	| string { NGC printf( "string = [%s]\n", $1 ); 
		$$ = checkArray( searchType( TYPE_CHAR ), strlen( $1 ) ); ENGC 
		GC 								
			$$ = createExtraInfoSymbol(assignRegisters(0));
			((struct ExtraInfo *)($$->info))->assignmentType = LOAD_ADDRESS;
		EGC }
	//We save arraySize because otherwise it could be overwritten by literal
	| ARRAY NEW '(' INTEGER ',' { $<integer>$ = arraySize; } literal ')' 
		{ NGC $$ = checkArray( $7, $<integer>6); ENGC 
		  GC
		  	((struct ExtraInfo*)($7->info))->assignmentType = TYPE_ARRAY; 
			$$ = $7;	  	
		  EGC}
	| ID_CONSTANT NEW {	$$ = searchTopLevel( SYM_TYPE, $1);	}
	| '[' array_content ']' { NGC $$ = checkArray($2->symbol, $2->info ); ENGC
							GC
								$$ = createExtraInfoSymbol(assignRegisters(0));
								((struct ExtraInfo *)($$->info))->assignmentType = LOAD_ADDRESS;
							EGC
							freeSymbolInfo($2);
							}
	| get
		{ $$ = searchType( $1 );
			
			GC 
				char c = 'I';
				switch( $1 ){
					case TYPE_FLOAT: c = 'F'; break;
					case TYPE_CHAR: c = 'U'; break;
					default: c = 'I'; break;
				}
				int reg = assignRegisters(0);
				$$ = createExtraInfoSymbol(reg);
				genGetCall( yyout, c, reg );
				((struct ExtraInfo*)($$->info))->variable = createVariable( SYM_VARIABLE, "var" );
				((struct Variable*)(((struct ExtraInfo*)($$->info))->variable->info))->type = searchType( $1 );
			EGC }
/*
	| GETI 
		{ $$ = searchType( TYPE_INTEGER );
			GC 
				int reg = assignRegisters(0); 	
				$$ = createExtraInfoSymbol(reg);	
				genGetCall( yyout, 'i', reg ); 
				((struct ExtraInfo*)($$->info))->variable = createVariable( SYM_VARIABLE, "var" );
				((struct Variable*)(((struct ExtraInfo*)($$->info))->variable->info))->type = searchType( TYPE_INTEGER );
			EGC }
	| GETF
		{ $$ = searchType( TYPE_FLOAT );
			GC 
				int reg = assignRegisters(0); 	
				$$ = createExtraInfoSymbol(reg);	
				genGetCall( yyout, 'f', reg ); 
				((struct ExtraInfo*)($$->info))->variable = createVariable( SYM_VARIABLE, "var" );
				((struct Variable*)(((struct ExtraInfo*)($$->info))->variable->info))->type = searchType( TYPE_FLOAT );
			EGC }
	| GETC 
		{ $$ = searchType( TYPE_CHAR );
			GC 
				int reg = assignRegisters(0); 	
				$$ = createExtraInfoSymbol(reg);	
				genGetCall( yyout, 'c', reg ); 
				((struct ExtraInfo*)($$->info))->variable = createVariable( SYM_VARIABLE, "var" );
				((struct Variable*)(((struct ExtraInfo*)($$->info))->variable->info))->type = searchType( TYPE_CHAR );
			EGC }
*/
	;

get :
	GETI { $$ = TYPE_INTEGER; }
	| GETF { $$ = TYPE_FLOAT; }
	| GETC { $$ = TYPE_CHAR; }
	;

// Here we check if all the array content has the same type.		
array_content :   
	literal { NGC $$ = nullSymbolInfo(); $$->symbol = $1; $$->info = 1; ENGC
		GC $$ = genArrayContent( yyout, $<symbolInfo>-1, $1, nullSymbolInfo());	EGC }
	| array_content ',' literal	{ NGC $$ = checkArrayContent($3, $1); ENGC
		  GC $$ = genArrayContent( yyout, $<symbolInfo>-1, $3, $1); EGC	}
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
	| logical_expression OR expression {NGC  $$ = checkLogicalExpression($1, $3, "or"); ENGC
										GC genOperation(yyout, $1, $3, "||"); EGC }
	;
logical_expression :
	relational_expression
	| relational_expression AND logical_expression {NGC $$ = checkLogicalExpression($1, $3, "and"); ENGC
													GC genOperation(yyout, $1, $3, "&&"); EGC }
	;
	
relational_expression :
	aritmetic_expression
	| aritmetic_expression relational_operator relational_expression 
		{NGC $$ = checkRelationalExpression($1, $3, $2); ENGC
		GC genOperation(yyout, $1, $3, $2); EGC}
	;

aritmetic_expression :
	term
	| term '+' aritmetic_expression {NGC $$ = checkAritmeticExpression($1, $3, "+"); ENGC
									GC genOperation(yyout, $1, $3, "+"); EGC }
	| term '-' aritmetic_expression {NGC $$ = checkAritmeticExpression($1, $3, "-"); ENGC
									GC genOperation(yyout, $1, $3, "-"); EGC }
	;
	
term :
	factor 
	| factor '*' term { NGC $$ = checkAritmeticExpression($1, $3, "*"); ENGC
						GC genOperation(yyout, $1, $3, "*"); EGC }
	| factor '/' term { NGC $$ = checkAritmeticExpression($1, $3, "/"); ENGC
						GC genOperation(yyout, $1, $3, "/"); EGC }
	;

factor :
	IDENTIF atribute { NGC $$ = getVariableType( SYM_VARIABLE, $1, $2 ); ENGC
						GC 	$$ = genAccessVariable(yyout, $1, SYM_VARIABLE, $2); EGC
			}
    	| ID_CONSTANT atribute {$$ = getVariableType( SYM_CONSTANT, $1, $2 );}
    	| ID_GLOBAL_VARIABLE atribute {	NGC $$ = getVariableType( SYM_GLOBAL, $1, $2 );	ENGC
    					GC $$ = genAccessVariable(yyout, $1, SYM_GLOBAL, $2);	EGC;}
	| literal 
	| NOT factor { NGC $$ = checkNotExpression($2); ENGC
					GC	$$ = $2; EGC }
	| simple_method_call { NGC $$ = getReturnType($1); ENGC }
	| '(' expression ')' {$$ = $2;}
	| '(' error ')' {yyerror( "Sintax error on expression" ); yyerrok;}
	;

literal : 
	INTEGER		{ $$ = searchType( TYPE_INTEGER ); 
					GC 
						int reg = assignRegisters(0); 
						if (reg == -1){
							
							reg = ((struct ExtraInfo*)(extraInfoPerRegister[nextRegisterOverflow]))->nRegister;
							fprintf(yyout,"\tR7 = R7-4;\n\tI(R7) = R%d;\t//Derramamos el registro\n",reg);
							((struct ExtraInfo*)(extraInfoPerRegister[nextRegisterOverflow]))->nRegister = 7;
							nextRegisterOverflow = (nextRegisterOverflow++)%6;
						}
						$$ = createExtraInfoSymbol(reg); 
						fprintf(yyout, "\tR%d = %d; // Loading integer %d\n", reg, arraySize, arraySize);
					EGC }
	| FLOAT		{ $$ = searchType( TYPE_FLOAT ); 					
					GC 
						int reg = assignRegisters(1); 
						$$ = createExtraInfoSymbol(reg);
						fprintf(yyout, "\tRR%d = %f; // Loading float %f\n", reg, floatVal, floatVal);
					EGC }
	| CHAR		{ $$ = searchType( TYPE_CHAR ); 
					GC 
						int reg = assignRegisters(0); 
						$$ = createExtraInfoSymbol(reg); 
						fprintf(yyout, "\tR%d = %d; // Loading char %d\n", reg, arraySize, arraySize);
					EGC }	
	| BOOL		{ $$ = searchType( TYPE_BOOLEAN );
					GC 
						int reg = assignRegisters(0); 
						$$ = createExtraInfoSymbol(reg); 
						fprintf(yyout, "\tR%d = %d; // Loading bool %d\n", reg, arraySize, arraySize);
					EGC }	
	;
	
// FIXME: en el fichero de prueba es.em se invoca muchas veces a geti y a puts,
// y los registros acaban acabandose y da error. Solucionar.
// FIXME: peta con strings largas.
puts : PUTS '(' string ')' separator { GC genPuts( yyout, $3 ); EGC }
	| PUTS error separator { yyerror("Wrong arguments in puts"), yyerrok; }
	;
string :
	BEGIN_COMPLEX_STRING END_COMPLEX_STRING { strcpy( $$, "" ); }
	| BEGIN_COMPLEX_STRING substring END_COMPLEX_STRING { strcpy( $$, $2 ); }
	| BEGIN_COMPLEX_STRING error END_COMPLEX_STRING {yyerror( "Sintax error on string" ); yyerrok;}
	;
	
substring :
	substring_part { GC strcpy( $$, $1 ); EGC }
	| substring_part substring { GC strcpy( $$, $1 ); strcat( $$, $2 ); EGC }
	;
	
substring_part :
	SUBSTRING { GC strcpy( $$, $1 ); EGC }
	| string_struct { GC strcpy( $$, $1 ); EGC }
	| SEC_SCAPE { GC strcpy( $$, $1 ); EGC }
	;
	
//TODO Lexical analizer does not allow expression on strings, so here we are only getting
//simple variables
string_struct :
		/*START_STRUCT expression END_STRUCT
		|*/ START_STRUCT factor END_STRUCT { GC char* str = genVariableInterpolation( yyout, $2 ); strcpy( $$, str ); free( str ); EGC } // FIXME: si el factor es una variable falta comprobar que exista.
		| START_STRUCT error END_STRUCT {yyerror( "Sintax error on string interpolation" ); yyerrok;}
		;
%%
  
int main(int argc, char** argv) {
   extern int errno;
   char errorString[256];
	int i = 1;	
	compilationState = 0;
	initializeSymTable();
	struct Method *mainScope = getCurrentScope();

	//Filling symbol table
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
	
	//Symbol table is filled, go once more to check errors
	if (argc>2)printf("\nSintax analyzer needed %d iterations\n", i);

    // Starting code analysis
	compilationState = 1;

	errors = 0;
	numlin = 1;
	fclose (yyin);
	yyin=fopen(argv[1],"r");
	
	goInScope(mainScope);
	
	yyparse();
	
	if (argc>2)showSymTable();
	if(!errors)
	{
		fillMethodDataSizes();
		if (argc>2)showSymTable();
		
		// Starting code generation
		compilationState = 2;
		numlin = 1;
		// Peparing name of compiled file
		char aux[51];
	    char *ptr;
	    strcpy(aux, argv[1]);
	    ptr= strtok(aux ,".");
	    strcpy(aux, ptr);
		strcat(aux, ".q.c");
		
		yyin = NULL;
		yyin = fopen( argv[1],"r" );
		//if(yyin == NULL); //Source file
		//printf( "yyin: %i\n", yyin );
		if( !yyin ){
		   perror( errorString );
			printf("ERROR AL ABRIR EL ARCHIVO %s\n",argv[1]);
		}
		
		yyout = NULL;
		yyout=fopen(aux,"w");	 
		//printf( "yyout: %i\n", yyout );
		if( !yyin ){
		   perror( errorString );
			printf("ERROR AL ABRIR EL ARCHIVO %s\n",aux);
		}

		fprintf(yyout,"#include \"Q.h\"\n\n");

		fprintf(yyout,"BEGIN\n");
		fprintf(yyout,"STAT(0)\n");

		getAllGlobals(yyout);

		fprintf(yyout,"CODE(0)\n");
		fprintf(yyout,"L 0:\n");
		getAllLocalsMain(yyout);

		if (argc>2){
			printf( "*****************ANTES GENERACION CODIGO\n" );
			showSymTable();
		}

		goInScope(mainScope);
		yyparse();
		
		fprintf( yyout,"\tR7 = R7 + %d;\t// Free space for local variables\n", mainScope->localsSize );	
		fprintf(yyout,"\tGT(-2);\n");
		fprintf(yyout,"END\n");
		fclose (yyout);
	}
	fclose (yyin);

	if (argc>2)showSymTable();
	freeSymbTable();

	return errors;
}

void yyerror(char* fmt, ...)
{
	AN
		errors = 1;
		va_list args;
		//Syntax error alone gives no information, ignore it
		if(strcmp(fmt,"syntax error") != 0){
			//We printf numlin - 1 because lexical analizer is ahead one or more lines 
			printf("---------Error on line %i: ",numlin - 1);       
			va_start(args,fmt);	 
			vprintf(fmt,args);
			va_end(args);
			printf("\n");
		}	
	EAN
}

