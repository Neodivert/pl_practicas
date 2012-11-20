#include "semanticUtilities.h"

struct Symbol* checkAritmeticExpression(struct Symbol* s1, struct Symbol* s2, char *op){
	int t1, t2;
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Factor and term are both integer or float.
	if((t1 == t2) && (t1 <= TYPE_FLOAT)){
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
	}else{
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
	}

	return s1;
}	

struct Symbol* checkRelationalExpression(struct Symbol* s1, struct Symbol* s2, char *op){
	int t1, t2;
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Factor and term are both integer or float.
	if((t1 == t2) && (t1 <= TYPE_FLOAT)){
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
	}else{
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
	}
	//Relational expression returns a boolean value
	return searchType( TYPE_BOOLEAN );
}

struct Symbol* checkLogicalExpression(struct Symbol* s1, struct Symbol* s2, char *op){
	int t1, t2;
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if((t1 == TYPE_BOOLEAN) && (t2 == TYPE_BOOLEAN)){
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
	}else{
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
	}
	return s1;
}

struct Symbol* checkNotExpression(struct Symbol* s){
	int t;
	t = ((struct Type *)(s->info))->id;
	//Operand is boolean.
	if(t != TYPE_BOOLEAN){
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: not applied on ");
		strcat(message, s->name);
		strcat(message, " is not permitted");
		yyerror((char *)message);
	} 
	return s;
}

struct Symbol* checkIsBoolean(struct Symbol* s){
	int t;
	t = ((struct Type *)(s->info))->id;
	//Operand is boolean.
	if(t != TYPE_BOOLEAN){
		yyerror("Type error: condition expression must return a boolean value");
	} 
	return s;
}


struct Symbol* checkArrayContent(struct Symbol* s1, struct Symbol* s2){
	int t1, t2;
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if(t1 == t2 ){
	//printf("Son del mismo tipo float o integer %d %d\n", t1, t2); 
	}else{
		char message[50];
		message[0] = '\0';
		strcat(message, "Type error: ");
		strcat(message, s1->name);
		strcat(message, " ");
		strcat(message, "with ");
		strcat(message, s2->name);
		strcat(message, " in array is not permitted");
		yyerror((char *)message);
	}
	return s1;
}	

struct Symbol* createSymbol_(char *name, int type){
	struct Symbol *s; 
	struct Type *t; 
	s = malloc(sizeof(struct Symbol));
	t = malloc(sizeof(struct Type));
	strcpy( s->name, name );
	t->id = type; 
	s->info = (void *)t;
	insertSymbol(s); 
	return s;
}
