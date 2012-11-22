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
	struct Symbol* aux = searchNArgument(method, argument);
	//printf("+++++ En check method call %d\n", argument);
	//Find the argument symbol
	if(aux != NULL)
	{
		//printf("+++++ Encontre el argument %s\n", aux->name);
		aux = ((struct Variable*)(aux->info))->type;
		//Argument has a known type
		if(aux != NULL)
		{
			aux = checkSameType(type, aux); 
			if(aux != NULL)
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
		//printf("+++++ No se sabe el tipo del argumento\n");
		return 1;	
	}
	else
	{
	//yyerror("Type error: Wrong amount of arguments in method call");
	//printf("+++++No se encontro el argumento\n");
	return 1;
	}
}	  

