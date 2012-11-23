#include "semanticUtilities.h"

struct Symbol* checkAritmeticExpression(struct Symbol* s1, struct Symbol* s2, char *op){
	int t1, t2;
	if( s1 == NULL || s2 == NULL)
		return NULL; 
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Factor and term are both integer or float.
	if((t1 == t2) && (t1 <= TYPE_FLOAT))
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

struct Symbol* checkRelationalExpression(struct Symbol* s1, struct Symbol* s2, char *op){
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

struct Symbol* checkLogicalExpression(struct Symbol* s1, struct Symbol* s2, char *op){
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

struct Symbol* checkNotExpression(struct Symbol* s){
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

struct Symbol* checkIsBoolean(struct Symbol* s){
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


struct Symbol* checkSameType(struct Symbol* s1, struct Symbol* s2){
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

struct Symbol* checkArray(Symbol* type, int n)
{
	if( n >= 0)
	{
		struct Symbol *arraySymbol = createArraySymbol( type, n );
		struct Symbol *aux = searchVariable(SYM_TYPE, arraySymbol->name);
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
	else //Array size is valid
	{
		return NULL;
	}		
}

struct Symbol* checkAssignement(struct Symbol *left, struct Symbol *right)
{
	printf("--------> En assignment \n");
	switch(isVariable(left))
	{
	case 0: //It is a variable with a known type
		printf("--------> En assignment with known type %s \n", left->name);
		if(checkSameType(((struct Variable*)(left->info))->type, right) != NULL)
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
	return left;	
}	

int isVariable(struct Symbol *s)
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

int checkMethodCall(struct Symbol *method, struct Symbol *type, int argument)
{
	struct Symbol* argumentSym = searchNArgument(method, argument);
	struct Symbol* argumentType;
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

struct MethodInfo *checkMethodDefinition(const char* const name)
{
	struct MethodInfo *info = malloc( sizeof( struct MethodInfo ) );
	struct Symbol* method = searchMethod(name);
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

struct Method *checkBlockDefinition(const char* const name, const char* const argName )
{
	printf("Falla block -2 \n");
	struct Method* scope = getCurrentScope();
	printf("Falla block -1 \n");
	char *blockName = createBlockName(name, argName);
	printf("Falla block 0 \n");
	struct Symbol* block = searchVariable(SYM_BLOCK, blockName);
	printf("Falla block 1 \n");
	if(block == NULL)
	{
		printf("Falla block 2 \n");
		insertBlockDefinition(blockName, argName);
		printf("Falla block 3 \n");
	}
	else
	{
		printf("Falla block 2a \n");
		goInScope(((struct Method *)(block->info)));
		printf("Falla block 3a \n");
	}	
	free(blockName);
	printf("Falla block 4 \n");
	return scope;	
}

int checkArgumentDefinition(const char* const name)
{
	struct Symbol* variableStruct = searchVariable( SYM_VARIABLE, name );
	if( variableStruct == NULL)
	{
		variableStruct = createVariable(SYM_VARIABLE, name);
		insertVariable(variableStruct, NULL);
		return 0;
	}
	else
		return 1;	
}

void setMethodReturnType(struct Symbol *method, struct Symbol *type)
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

char *createBlockName(const char* const name, const char* const argName)
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
