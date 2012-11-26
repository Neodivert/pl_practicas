#include "semanticUtilities.h"

/*                                  1. Expressions                            */

// Check if subexpressions' types s1 and s2 are both INTEGER or FLOAT. If 
// previous condition is satisfied, return s1. Otherwise, generate an error 
// message (using op) and return NULL.
// (*) If s1 and/or s2 are NULL, this function simply returns NULL.
Symbol* checkAritmeticExpression(Symbol* s1, Symbol* s2, char *op){
	int t1, t2;		
	if( s1 == NULL || s2 == NULL || s1->info == NULL || s2->info == NULL){
		return NULL; 
	}	
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
	if( s1 == NULL || s2 == NULL || s1->info == NULL || s2->info == NULL){
		return NULL; 
	}		
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Factor and term are both integer or float.
	if((t1 == t2) && (t1 <= TYPE_FLOAT)){
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
	if( s1 == NULL || s2 == NULL || s1->info == NULL || s2->info == NULL){
		return NULL; 
	} 	
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if((t1 == TYPE_BOOLEAN) && (t2 == TYPE_BOOLEAN)){
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
	if( s == NULL || s->info == NULL ){
		return NULL; 
	}	
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
	if( s == NULL || s->info == NULL ){
		return NULL; 
	} 	
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
	if( s1 == NULL || s2 == NULL || s1->info == NULL || s2->info == NULL){
		return NULL; 
	}
		 
	t1 = ((struct Type *)(s1->info))->id;
	t2 = ((struct Type *)(s2->info))->id;
	//Both operators are boolean.
	if(t1 == t2 ){ 
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
	if( s == NULL || s->info == NULL){
		return NULL; 	
	}	
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

struct SymbolInfo* checkArrayContent(struct Symbol* type, struct SymbolInfo* arrayInfo )
{
	struct SymbolInfo* returnInfo = arrayInfo;
	if( arrayInfo != NULL && arrayInfo->info != -1 && arrayInfo->symbol != NULL){
		struct Symbol* otherType = checkSameType( type, arrayInfo->symbol);
		if(otherType == NULL)
		{
			yyerror("All elements in array must be the same type");	
			returnInfo->symbol = NULL;
			returnInfo->info = -1;						
		}else
		{ 
			returnInfo->symbol = arrayInfo->symbol;
			returnInfo->info = returnInfo->info + 1;
		}				
	}else{	
		returnInfo->symbol = NULL;
		returnInfo->info = -1;		
	}
	return returnInfo;
}

// Search in the symbols' table for an type array of size "n" and whose elements
// are of type "type".
// If n < 0 (invalid size), return NULL.
// Otherwise, return the array's type symbol (if array wasn't found, this 
// function also create it and insert it).
Symbol* checkArray(Symbol* type, int n)
{
	if( n >= 0 && type != NULL )
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
	else //Array size is invalid or type is not correct
	{
		return NULL;
	}		
}

/*                              3. Methods                                    */

// Return 0 if call argument with type "type" and position "argument" match the
// corresponding argument in method definition (*). Otherwise return 1.
// (*) If the argument does not have a known type we asume the method call is 
// right and assign the type of the value to the argument.
int checkMethodCall(struct Symbol *method, struct Symbol *type, int argument)
{
	struct Symbol* argumentSym = searchNArgument(method, argument);
	struct Symbol* argumentType;
	//Find the argument symbol
	if(argumentSym != NULL && argumentSym->info != NULL)
	{
		argumentType = ((struct Variable*)(argumentSym->info))->type;
		//Argument has a known type
		if(argumentType != NULL)
		{
			argumentType = checkSameType(type, argumentType); 
			if(argumentType != NULL)
			{
				return 0;
			}
			else
			{
				yyerror("Type error: Arguments in method call do not match");
				return 1;
			}
		}
		else
		{
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
	return 1;
	}
}	  

// Check if method name is already in symbols' table (if not, insert it).
// Return a MethodInfo struct, whose "scope" field points to the current 
// scope's symbol and "result" is an integer which indicates if method was
// already defined (1) or not (0).
struct MethodInfo *checkMethodDefinition(const char* const name)
{
	struct MethodInfo *info = malloc( sizeof( struct MethodInfo ) );
	struct Symbol* method = searchTopLevel( SYM_METHOD, name);
	info->scope = getCurrentScope();
	if(method == NULL)
	{
		if(compilationState==1){
			yyerror("Error, Method not defined");
			yyerrork;
		}
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
void setMethodReturnType(struct Symbol *method, struct Symbol *type)
{
	if(method != NULL && method->info != NULL && type != NULL && type->info != NULL )
	{
		struct Method *methodInfo = ((struct Method*)(method->info));
		if( type->symType == SYM_METHOD )
		{
			methodInfo->returnType = ((struct Method*)(type->info))->returnType;
		}
		else //It's a variable
		{
			methodInfo->returnType = ((struct Variable*)(type->info))->type;
		}	
	}	
}

// Return 0 if variable name exists in symbols' table. If not, create and insert
// it, and then return 1.
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

/*                             4. Blocks                                      */

// Search in symbols' table for block "name" with argument "argName". If it is
// not in the symbols' table, create it and insert it. This functions returns
// and Method* that points to the current scope's (the block) symbol.
struct Method *checkBlockDefinition(const char* const name, const char* const argName )
{
	struct Method* scope = getCurrentScope();
	char *blockName = createBlockName(name, argName);
	struct Symbol* block = searchVariable(SYM_BLOCK, blockName);
	if(block == NULL)
	{
		if (compilationState == 0){
			yyerror("Error, Undefined Block");
			yyerrok;
		}
		insertBlockDefinition(blockName, argName);
	}
	else
	{
		goInScope(((struct Method *)(block->info)));
	}
	struct Symbol* variable = searchVariable( SYM_VARIABLE, name);	
	if(variable != NULL)
	{	//Variabe.each is defined in symbol table
		struct Symbol* type = ((struct Variable*)(variable->info))->type;
		//Get type of variable
		if( type != NULL )
		{	//Variable type is known
			if( ((struct Type*)(type->info))->id == TYPE_ARRAY )
			{	//Variable's type is array
				type = getArrayType(variable);
				if(type != NULL)
				{	//Array type is known
					variable = searchVariable( SYM_VARIABLE, argName);
					//Get block argument and it's type
					if(variable != NULL && ((struct Variable*)(variable->info))->type == NULL )
					{	//Argument type is not defined, so set it now
						((struct Variable*)(variable->info))->type = type;
						setChanged();
					}
				}
			}
			else
			{
				yyerror("Type error: block can only be used with array variables");
			}	
		}
	}
	free(blockName);
	return scope;	
}

// Generate a name for block whose name is "name" and argument is "argName".
char *createBlockName(cstr name, cstr argName)
{
	char *blockName = (char *)malloc(sizeof(char) * 50);
	blockName[0] = '\0';
	strcat(blockName, name);
	strcat(blockName, "_");
	strcat(blockName, argName);
	return blockName;
}
	
/*                                 5. Classes                                  */

struct Symbol* checkClassDefinitonPre(const char * const className, struct Symbol *currentClass)
{
	int i = 0;
	struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, className );
	struct Symbol *newClass = currentClass;
	if( classSymbol == NULL ){
		classSymbol = createClassSymbol(className);
		insertSymbol( classSymbol );
	}
	
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	
	//Elements were counted but no yet inserted			
	if( classInfo->nElements != 0 && classInfo->elements == NULL)
	{
		newClass = classSymbol;
		classInfo->elements = malloc(sizeof( struct Symbol *) * classInfo->nElements );
		for(i = 0; i < classInfo->nElements; i++){
			classInfo->elements[i] = NULL;
		}
	}
	return newClass;
}			

struct Symbol* checkClassDefinitonPost(const char * const className, int nVariables)
{
	struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, className );
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	if( classInfo->nElements == 0 )
	{
		if( nVariables != 0 ){
			classInfo->nElements = nVariables;
			setChanged();
		}else{
			yyerror("Type error: Classess must have at least one class variable");
		}				
	}	
	return NULL;
}
	
struct SymbolInfo* checkClassAtribute( const char* const name )
{
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	
	info->symbol = NULL;
	info->info = SYM_CLASS_VARIABLE;
	info->name = (char *)malloc( strlen( name )+1 );
	strcpy( info->name, name );
	return info;
}

int checkClassContentDefinition( struct Symbol *classSymbol, const char* const varName, struct Symbol *type, int pos)
{
	if( classSymbol )
	{
		char classVarName[50] = "";
		strcat(classVarName, classSymbol->name);
		strcat(classVarName, varName);
		struct Symbol* classVar = createVariable( SYM_CLASS_VARIABLE, classVarName );
		insertVariable( classVar, type );
		struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
		classInfo->elements[pos] = classVar;				
	}
	return (pos + 1);	
}

int createClassVar( const char* const name, const char* const varName, struct Symbol *type)
{
	char classVarName[50] = "";
	strcat(classVarName, name);
	strcat(classVarName, varName);
	struct Symbol* classVar = createVariable( SYM_VARIABLE, classVarName );
	insertVariable( classVar, type );					
}

int checkClassNew(struct Symbol *classSymbol, const char* const varName)
{
	int i = 0;
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	struct Symbol * type = NULL;
	if(classInfo->elements != NULL && classInfo->elements[0]){
		for(i = 0; i < classInfo->nElements; i++)
		{
			type = ((struct Variable*)(classInfo->elements[i]->info))->type;
			createClassVar(varName, classInfo->elements[i]->name, type);
		}
		i = 1;
	}	
	return i;
}

/*                                6. Others                                   */

struct Symbol* checkAssignement(struct SymbolInfo* left_, struct Symbol *right)
{
	struct Symbol* left = left_->symbol;
	int info = left_->info;
	struct Symbol* variableType = NULL;
	
	switch(isVariable(left))
	{
	case 0: //It is a variable with a known type
		if(info == TYPE_ARRAY){
			variableType = getArrayType(left);
		}else{
			if( info == SYM_CLASS_VARIABLE){
				left = getClassVar( left , left_->name);
			}
			variableType = ((struct Variable*)(left->info))->type;
		}		
		if(checkSameType( variableType, right) != NULL)
		{
			//Left side and right side are the same type
		}else
		{
			//If right = NULL right side was wrong/unknown and that was already warned
			if(right != NULL) 
			{
				char message[50];
				message[0] = '\0';
				strcat(message, "Type error: with variable ");
				strcat(message, left->name);
				yyerror((char *)message);
			}
		}										
		break;
	case 1: //It is a variable without a known type
		if(info != SYM_CLASS_VARIABLE){
			if(searchVariable(left->symType, left->name) == NULL)
			{ 
				//Variable is not in symbolTable, insert it
				if(right != NULL && right->info != NULL)
				{
					int type = ((struct Type*)(right->info))->id;
					if( type == TYPE_CLASS)
					{
						struct ClassType *classInfo = ((struct Type*)(right->info))->classInfo;
						if(classInfo->elements != NULL && classInfo->elements[0])
						{
							if( ! searchVariable(SYM_VARIABLE, left->name)){
								insertVariable( left, right );
							}	
							checkClassNew( left, right->name);
						}else{
							insertVariable( left, NULL );
						}
					}else{
						insertVariable( left, right );
					}			
				}
				else
				{
					insertVariable( left, NULL );
				}
			}	
			else
			{
				//Variable is in symbolTable, set its type, right might be NULL
				if(right != NULL && right->info != NULL)
				{
			
					int type = ((struct Type*)(right->info))->id;
					if( type == TYPE_CLASS)
					{
						struct ClassType *classInfo = ((struct Type*)(right->info))->classInfo;
						if(classInfo->elements != NULL && classInfo->elements[0])
						{	
							((struct Variable *)(left->info))->type = right;											
							checkClassNew( right, left->name);
							setChanged();
						}
					}else
					{
						((struct Variable *)(left->info))->type = right;												
						setChanged();	
					}	
				
				}
			}
		}
		break;
	case 2: //It is not a variable
		yyerror("Left side of expression is invalid\n");
		break;		
	}
	freeSymbolInfo(left_);
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
	info->name = NULL;
	
}
