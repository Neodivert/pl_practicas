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
