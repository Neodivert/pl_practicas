#include "semanticUtilities.h"

/*                                  1. Expressions                            */

// Check if subexpressions' types s1 and s2 are both INTEGER or FLOAT. If 
// previous condition is satisfied, return s1. Otherwise, generate an error 
// message (using op) and return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkAritmeticExpression(Symbol* s1, Symbol* s2, char *op){
	int t1, t2;
	if( s1 == NULL || s2 == NULL)
		return NULL; 
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	
	if((t1 == t2) && (t1 <= TYPE_FLOAT)){
		// s1 and s2 are both integer or float.
		return s1;
	}else{
		// s1 and s2 are either, unlike types, or both different than integer and float
		// Generate error message
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: ");
		strcat(message, s1->name);
		strcat(message, " ");
		strcat(message, op);
		strcat(message, " ");
		strcat(message, s2->name);
		strcat(message, " is not permitted");
		yyerror((char *)message);
		return NULL;
	}
}	

// Check if subexpressions types s1 and s2 are both INTEGER or FLOAT. If 
// previous condition is satisfied, search for boolean type in symbols' table
// and return it. Otherwise, generate an error message (using op) and return 
// NULL. 
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkRelationalExpression(Symbol* s1, Symbol* s2, char *op){
	int t1, t2;
	if( s1 == NULL || s2 == NULL)
		return NULL; 	
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Factor and term are both integer or float.
	if((t1 == t2) && (t1 <= TYPE_FLOAT)){
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
		return searchType( TYPE_BOOLEAN );
	}
	else
	{
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: ");
		strcat(message, s1->name);
		strcat(message, " ");
		strcat(message, op);
		strcat(message, " ");
		strcat(message, s2->name);
		strcat(message, " is not permitted");
		yyerror((char *)message);
		return NULL;
	}
	//Relational expression returns a boolean value
	
}

// Check if subexpressions types s1 and s2 are both BOOLEAN. If previous 
// condition is satisfied, return s1. Otherwise, generate an error message 
// (using op) and return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkLogicalExpression(Symbol* s1, Symbol* s2, char *op){
	int t1, t2;
	if( s1 == NULL || s2 == NULL)
		return NULL; 	
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if((t1 == TYPE_BOOLEAN) && (t2 == TYPE_BOOLEAN))
	{
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
		return s1;
	}
	else
	{
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: ");
		strcat(message, s1->name);
		strcat(message, " ");
		strcat(message, op);
		strcat(message, " ");
		strcat(message, s2->name);
		strcat(message, " is not permitted");
		yyerror((char *)message);
		return NULL;
	}
}

// Check if expression's type s is TYPE_BOOLEAN (so its allowed in a NOT 
// expression).
// If s is of type TYPE_BOOLEAN, return NULL. Otherwise show an error message
// and return NULL.
// (*) If s is NULL, this function simply returns NULL.
Symbol* checkNotExpression(Symbol* s){
	int t;
	if( s == NULL )
		return NULL; 	
	t = ((struct Type *)(s->info))->id;
	//Operand is boolean.
	if(t == TYPE_BOOLEAN)
	{
		return s;
	}
	else
	{
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: not applied on ");
		strcat(message, s->name);
		strcat(message, " is not permitted");
		yyerror((char *)message);
		return NULL;
	} 
}

// Check if type s is an boolean. If previous  condition is satisfied, return s.
// Otherwise, generate an error message and return NULL (*).
// (*) If s is NULL, this function simply returns NULL.
Symbol* checkIsBoolean(Symbol* s){
	int t;
	if( s == NULL )
		return NULL; 	
	t = ((struct Type *)(s->info))->id;
	//Operand is boolean.
	if(t == TYPE_BOOLEAN)
	{
		return s;
	}	
	else
	{	
		yyerror("Type error: condition expression must return a boolean value");
		return NULL;
	}	
}

// Return s1 if both subexpressions types s1 and s2 are of the same time.
// Otherwise return NULL.
Symbol* checkSameType(Symbol* s1, Symbol* s2){
	int t1, t2;
	if( s1 == NULL || s2 == NULL)
		return NULL;
		 
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if(t1 == t2 )
	{
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
		return s1;
	}else
	{
		return NULL;
	}
}	


/*                               2. Arrays                                    */

// Check if expression's type s is integer.
// Return a SymbolInfo struct, whose "info" field is set to TYPE_ARRAY, and 
// "symbol" field...
// - points to s if s is a TYPE_INTEGER.
// - points to null otherwise (in this case, an error message is shown too.
// This function is used to verify that an array index is actually a integer.
// (*) If s is NULL, this function simply returns NULL.
struct SymbolInfo* checkIsInteger(Symbol* s)
{
	int t;
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	if( s == NULL )
		return NULL; 	
	t = ((struct Type *)(s->info))->id;
	//Operand is boolean.
	if(t == TYPE_INTEGER)
	{
		info->symbol = s;
		info->info = TYPE_ARRAY; 
		return info;
	}	
	else
	{	
		yyerror("Type error: expression between [] must be integer");
		info->symbol = NULL;
		info->info = TYPE_ARRAY; 		
		return info;
	}	
}	

// Search in the symbols' table for an type array of size "n" and whose elements
// are of type "type".
// If n < 0 (invalid size), return NULL.
// Otherwise, return the array's type symbol (if array wasn't found, this 
// function also create it and insert it).
Symbol* checkArray(Symbol* type, int n)
{
	if( n >= 0)
	{
		Symbol *arraySymbol = createArraySymbol( type, n );
		Symbol *aux = searchVariable(SYM_TYPE, arraySymbol->name);
		if(aux == NULL)
		{
			//Array type did not exist
			insertArray( type, n );
			return arraySymbol;
		}
		else
		{
			//Array type did exist
			freeSymbol(arraySymbol);
			return aux;
		}	
	}
	else //Array size is invalid
	{
		return NULL;
	}		
}


/*                              3. Methods                                    */

// Return 0 if call argument with type "type" and position "argument" match the
// corresponding argument in method definition (*). Otherwise return 1.
// (*) If the argument does not have a known type we asume the method call is 
// right and assign the type of the value to the argument.
int checkMethodCall(Symbol *method, Symbol *type, int argument)
{
	Symbol* argumentSym = searchNArgument(method, argument);
	Symbol* argumentType;
	//printf("+++++ En check method call %d\n", argument);
	//Find the argument symbol
	if(argumentSym != NULL)
	{
		//printf("+++++ Encontre el argument %s\n", aux->name);
		argumentType = ((struct Variable*)(argumentSym->info))->type;
		//Argument has a known type
		if(argumentType != NULL)
		{
			argumentType = checkSameType(type, argumentType); 
			if(argumentType != NULL)
			{
				//printf("+++++ Son del mismo tipo\n");
				return 0;
			}
			else
			{
				yyerror("Type error: Arguments in method call do not match");
				return 1;
			}
		}
		else
		{//printf("+++++ No se sabe el tipo del argumento\n");
			//If the argument does not have a known type we asume
			//the method call is right and assign the type of the
			//value to the argument.
			((struct Variable*)(argumentSym->info))->type = (void *)type;
			setChanged();
			return 0;	
		}
	}
	else
	{
	//yyerror("Type error: Wrong amount of arguments in method call");
	//printf("+++++No se encontro el argumento\n");
	return 1; 
	}
}	  

// Check if method name is already in symbols' table (if not, insert it).
// Return a MethodInfo struct, whose "scope" field points to the current 
// scope's symbol and "result" is an integer which indicates if method was
// already defined (1) or not (0).
struct MethodInfo *checkMethodDefinition(cstr name)
{
	struct MethodInfo *info = malloc( sizeof( struct MethodInfo ) );
	Symbol* method = searchMethod(name);
	info->scope = getCurrentScope();
	if(method == NULL)
	{
		insertMethodDefinition(name);
		info->result = 0;
	}
	else
	{
		goInScope(((struct Method *)(method->info)));
		info->result = 1;	
	}	
	return info;	
}

// Set method's return type to type of symbols "type"
void setMethodReturnType(Symbol *method, Symbol *type)
{
	if(method != NULL)
	{
		struct Method *methodInfo = ((struct Method*)(method->info));
		if(type != NULL)
		{
			if(type->symType == SYM_METHOD)
			{
				methodInfo->returnType = ((struct Method*)(type->info))->returnType;
			}
			else //It's a variable
			{
				methodInfo->returnType = ((struct Variable*)(type->info))->type;
			}
		}	
	}	
}

// Return 0 if variable name exists in symbols' table. If not, create and insert
// it, and then return 1.
int checkArgumentDefinition(cstr name)
{
	Symbol* variableStruct = searchVariable( SYM_VARIABLE, name );
	if( variableStruct == NULL)
	{
		variableStruct = createVariable(SYM_VARIABLE, name);
		insertVariable(variableStruct, NULL);
		return 0;
	}
	else
		return 1;	
}

/*                             4. Blocks                                      */

// Search in symbols' table for block "name" with argument "argName". If it is
// not in the symbols' table, create it and insert it. This functions returns
// and Method* that points to the current scope's (the block) symbol.
struct Method *checkBlockDefinition(cstr name, cstr argName )
{
	struct Method* scope = getCurrentScope();
	char *blockName = createBlockName(name, argName);
	Symbol* block = searchVariable(SYM_BLOCK, blockName);
	if(block == NULL)
	{
		insertBlockDefinition(blockName, argName);
	}
	else
	{
		goInScope(((struct Method *)(block->info)));
	}	
	free(blockName);
	return scope;	
}

// Generate a name for block whose name is "name" and argument is "argName".
char *createBlockName(cstr name, cstr argName)
{
	printf("create 0\n");
	char *blockName = (char *)malloc(sizeof(char) * 50);
	printf("create 1\n");
	blockName[0] = '\0';
	printf("create 2\n");
	strcat(blockName, name);
	strcat(blockName, "_");
	strcat(blockName, argName);
	printf("create 5\n");
	return blockName;
}


Symbol* checkAssignement(struct SymbolInfo* left_, Symbol *right)
{
	printf("--------> En assignment \n");
	Symbol* left = left_->symbol;
	int info = left_->info;
	Symbol* variableType = NULL;
	
	switch(isVariable(left))
	{
	case 0: //It is a variable with a known type
		printf("--------> En assignment with known type %s \n", left->name);
		if(info == TYPE_ARRAY){
			variableType = getArrayType(left);
		}else{
			variableType = ((struct Variable*)(left->info))->type;
		}		
		if(checkSameType( variableType, right) != NULL)
		{
			//Left side and right side are the same type
			//Generar codigo
			showSymTable();
		}else
		{
			//If right = NULL right side was wrong/unknown and that was already warned
			showSymTable();
			if(right != NULL) 
			{
				char message[50];
				message[0] = '\0';
				strcat(message, "Type error: with variable ");
				strcat(message, left->name);
				yyerror((char *)message);
			}
		}	
		printf("--------> En assignment with known type end %s \n", left->name);									
		break;
	case 1: //It is a variable without a known type
		printf("--------> En assignment with not known type %s \n", left->name);
		showSymTable();
		if(searchVariable(left->symType, left->name) == NULL)
		{ 
			//Variable is not in symbolTable, insert it
			if(right != NULL)
			{
				printf("Insertando con tipo %s\n", right->name);
				insertVariable( left, right );
			}
			else
			{
				printf("Insertando con tipo NULL\n");
				insertVariable( left, NULL );
			}
		}	
		else
		{
			//Variable is in symbolTable, set its type, right might be NULL
			if(right != NULL)
			{
				((struct Variable *)(left->info))->type = right;												
				setChanged();
			}
		}	
		showSymTable();
		printf("--------> En assignment with not known type end %s \n", left->name);
		//Generar codigo o no xD
		break;
	case 2: //It is not a variable
		printf("--------> En assignment left side error\n");
		yyerror("Left side of expression is invalid\n");
		break;		
	}
	free(left_);
	return left;	
}	

int isVariable(Symbol *s)
{
	if(s == NULL || s->info == NULL)
	{
		return 2;
	}
	if(s->symType == SYM_VARIABLE || s->symType == SYM_GLOBAL
		|| s->symType == SYM_CONSTANT)
	{	
		if(((struct Variable *)(s->info))->type == NULL)
			return 1;
		else
			return 0;		
	}
	else
	{
		return 2;
	}
}



struct SymbolInfo* nullSymbolInfo()
{
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	info->symbol = NULL;
	info->info = 0;
	
}
