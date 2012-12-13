#include "semanticUtilities.h"

/*                                  1. Expressions                            */

// Check if subexpressions' types s1 and s2 are both INTEGER or FLOAT. 
// Return value:
//	s1 - if s1 and s2 are both INTEGER or FLOAT.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
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
		yyerror("Type error: %s %s %s is not permited", s1->name, op, s2->name);
		return NULL;
	}
}	

// Check if subexpressions types s1 and s2 are both INTEGER or FLOAT. 
// Return value:
// boolean type symbol - if s1 and s2 are both INTEGER or FLOAT.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
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
		yyerror("Type error: %s %s %s is not permited", s1->name, op, s2->name);
		return NULL;
	}
	//Relational expression returns a boolean value
	
}

// Check if subexpressions types s1 and s2 are both BOOLEAN.
// Return value:
//	s1 - if s1 and s2 are both BOOLEAN.
// 	NULL - if s1 and s2 are different types (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
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
		yyerror("Type error: %s %s %s is not permited", s1->name, op, s2->name);
		return NULL;
	}
}

// Check if expression's type s is TYPE_BOOLEAN (so its allowed in a NOT expr.)
// Return value:
//	s - if s is TYPE_BOOLEAN
//	NULL - otherwise (*)
// (*) If s is non-NULL, this function also generates an error message.
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
		yyerror("Type error: not applied on %s is not permitted", s->name);
		return NULL;
	} 
}

// Check if type s is an boolean. 
// Return value:
//	s - if s is an boolean.
//	NULL - otherwise.
// (*) If s is non-NULL, this function also generates an error message.
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

// Check if both subexpressions types s1 and s2 are equal.
// Return value:
//	s1 - if s1 and s2 are the same type.
// 	NULL - otherwise. (*)
// (*) If s1 and s2 are both non-NULL, this function also generates an error 
// message).
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
struct SymbolInfo* checkIsInteger(Symbol* s)
{
	int t;
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	if( s == NULL || s->info == NULL){
		info->symbol = NULL;
		info->info = TYPE_ARRAY; 	
		info->name = NULL;	
		return info;
	}	
	t = ((struct Type *)(s->info))->id;
	//Operand is integer.
	if(t == TYPE_INTEGER)
	{
		info->symbol = s;
		info->info = TYPE_ARRAY; 
		info->name = NULL;
		return info;
	}	
	else
	{	
		yyerror("Type error: expression between [] must be integer but is %s", s->name);
		info->symbol = NULL;
		info->info = TYPE_ARRAY; 
		info->name = NULL;		
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
int checkMethodCallArguments(struct Symbol *method, struct Symbol *type, int argument)
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
			//If type != NULL then there was a real change
			if(type){
				setChanged();
			}	
			return 0;	
		}
	}
	else
	{
		return 1;
	}
}	  

struct Symbol* checkMethodCall(cstr name, int methodArguments, int currentNArguments, Symbol *method)
{
	AN
		if(method){
			if(currentNArguments != methodArguments){
				yyerror("Type error: Wrong amount or undefined type arguments in method call %s", name);
			}	
		}else{
			yyerror("Type error: Method %s is not defined", name);
		}
	EAN  
	return searchTopLevel( SYM_METHOD, name);
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
				yyerror("Type error: variable %s ,block can only be used with array variables", name);
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
	
	// In newClass we'll return the class "className" (created or found here).
	struct Symbol *newClass = currentClass;
	
	// Search class "className" in symbols' table.
	struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, className );
	
	// If not found, create the class symbol and insert it in symbols¡ table.
	if( classSymbol == NULL ){
		classSymbol = createClassSymbol(className);
		insertSymbol( classSymbol );
	}
	
	// Get the class' info (number of elements and the pointers to them).
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	
	// Elements were counted but no yet inserted			
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
	// Search class "className" and its info in symbols' table.
	struct Symbol *classSymbol = searchTopLevel( SYM_TYPE, className );
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	
	if( classInfo->nElements == 0 )
	{
		// Class' fields weren't counted yet. Let's count them.
		if( nVariables != 0 ){
			classInfo->nElements = nVariables;
			setChanged();
		}else{
			yyerror("Type error: Class %s must have at least one class variable", className);
		}				
	}	
	return NULL;
}

// Create a instance "varName" from class' symbol "classSymbol".
// Return value: 1 if successful, 0 otherwise.
int checkClassNew( struct Symbol *classSymbol, const char* const varName )
{
	int i = 0;

	// Access to the class' info.
	struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
	struct Symbol * type = NULL;

	// This loop create all the instance's variables and insert them in 
	// symbols' table.
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

// Make effective an assigment "varName = literal" in the definition of class 
// "classSymbol" 
// Other arguments:
// 	type - type of literal.
// 	pos - position of assignment "varName = literal" in class definition.
// Return value: position of assigment + 1.
int checkClassContentDefinition( struct Symbol *classSymbol, const char* const varName, struct Symbol *type, int pos)
{
	if( classSymbol )
	{
		// Generate the variable full name.
		char classVarName[50] = "";
		strcat(classVarName, classSymbol->name);
		strcat(classVarName, varName);

		// Create and insert the variable in symbols' table.
		struct Symbol* classVar = createVariable( SYM_CLASS_VARIABLE, classVarName );
		insertVariable( classVar, type );

		// Update de class' vector of pointers to its children with a pointer
		// to the new var.
		struct ClassType *classInfo = ((struct Type*)(classSymbol->info))->classInfo;
		classInfo->elements[pos] = classVar;				
	}
	return (pos + 1);	
}

// Create and return an auxiliar SymbolInfo struct for variable "name" with a
// null symbol pointer, the symbol's type (SYM_CLASS_VARIABLE) and the var 
// name.
struct SymbolInfo* checkClassAtribute( const char* const name )
{
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	
	info->symbol = NULL;
	info->info = SYM_CLASS_VARIABLE;
	info->name = (char *)malloc( strlen( name )+1 );
	strcpy( info->name, name );
	return info;
}

// Create a attribute "varName" for class "name" and with type "type".
int createClassVar( const char* const name, const char* const varName, struct Symbol *type)
{
	char classVarName[50] = "";
	strcat(classVarName, name);
	strcat(classVarName, varName);
	struct Symbol* classVar = createVariable( SYM_VARIABLE, classVarName );
	insertVariable( classVar, type );					
}


/*                                6. Others                                   */

// Search variable in left_ in symbol table:
// If found and without known type -> assing type
// If found and with known type -> check right is the same type
// If not found -> insert in symbol table with right's type 
struct Symbol* checkAssignement(struct SymbolInfo* left_, struct Symbol *right)
{
	struct Symbol* left = left_->symbol;
	int info = left_->info;
	struct Symbol* variableType = NULL;
	// Check if left is a variable and it's type
	switch(isVariable(left))
	{
	case 0: // It is a variable with a known type, variable is in symbol table
		if(info == TYPE_ARRAY){
			// Left side is var[] so get type array's elements
			variableType = getArrayType(left);
		}else{
			if( info == SYM_CLASS_VARIABLE){
				// Left side is var.atribute so get atribute's type 
				left = getClassVar( left , left_->name);
			}
			if(left && left->info){
				// Variable is simple or atribute, get its type from  
				// variable struct
				variableType = ((struct Variable*)(left->info))->type;
			}else{
				// Variable is class type, but class or attribute does not exists
				variableType = NULL;
			}	
		}	
		// We got variable type, now check is left type and right type are the same
		if(checkSameType( variableType, right) != NULL)
		{
			// Left side and right side are the same type
		}else
		{
			// If right == NULL, right side was wrong/unknown and that was already warned
			if(right != NULL) 
			{
				if(variableType){
					// Trying to do a assignement with not compatible types
					yyerror("Type error: variable %s is %s but left side is %s", left->name, variableType->name, right->name);
				}else{
					// Operator [] or . was used on a variable that has a simple
					// type
					yyerror("Type error: variable %s is missused", left_->symbol->name);
				}	
			}
		}									
		break;
	case 1: // It is a variable without a known type, variable might be in symbol table
		// If the variable was used with [] or . do not insert it yet
		// since its type is complex, and that case is not handle here 
		if(info != SYM_CLASS_VARIABLE && info != TYPE_ARRAY){
			// Search variable in symbol table
			if(searchVariable(left->symType, left->name) == NULL)
			{ 
				// Variable is not in symbolTable
				// Check right's type
				if(right != NULL && right->info != NULL)
				{
					int type = ((struct Type*)(right->info))->id;
					if( type == TYPE_CLASS)
					{
						// Right's type is class, get class information
						struct ClassType *classInfo = ((struct Type*)(right->info))->classInfo;
						if(classInfo->elements != NULL && classInfo->elements[0])
						{
							// Class is well defined
							if( ! searchVariable(SYM_VARIABLE, left->name)){
								// If variable is not in symbol table then insert it
								insertVariable( left, right );
							}	
							// Create instance variables for the class
							checkClassNew( left, right->name);
						}else{
							// Class was wrong or not yet fully defined
							insertVariable( left, NULL );
						}
					}else{
						// Right's type is not class, so insert variable normally 
						insertVariable( left, right );
					}			
				}
				else
				{
					// Right's type is wrong/unknown, so insert variable with
					// NULL type
					insertVariable( left, NULL );
				}
			}	
			else
			{
				// Variable is in symbolTable, set its type, right might be NULL
				if(right != NULL && right->info != NULL)
				{
					// Right's type is known			
					int type = ((struct Type*)(right->info))->id;
					if( type == TYPE_CLASS)
					{
						// Right's type is class, get class information
						struct ClassType *classInfo = ((struct Type*)(right->info))->classInfo;
						if(classInfo->elements != NULL && classInfo->elements[0])
						{	
							// Class is well defined
							((struct Variable *)(left->info))->type = right;											
							checkClassNew( right, left->name);
							setChanged();
						}
					}else
					{
						// Variable had an unknow type and now is known so set it
						((struct Variable *)(left->info))->type = right;												
						setChanged();	
					}					
				}else{
					// Right is NULL, in analysis this means that we could
					// not find out which type the variable must be  
					AN
						yyerror("Type Error: Could not stablish variable %s type", left->name);
					EAN
				}
			}
		}
		break;
	case 2: // It is not a variable
		yyerror("Left side of expression is invalid\n");
		break;		
	}
	freeSymbolInfo(left_);
	// Return variable struct symbol
	return left;	
}

// Is s a variable?
// Return value: an integer with value:
//	0 - s is a variable with a known type.
//	1 - s is a variable with a uknown type.
// 	2 - s ins't a variable.
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

// Return a empty SymbolInfo struct (all its fields are NULL or 0).
struct SymbolInfo* nullSymbolInfo()
{
	struct SymbolInfo* info = malloc(sizeof(struct SymbolInfo));
	info->symbol = NULL;
	info->info = 0;
	info->name = NULL;
}

// Check if all the array content has the same type. "type" refers to the type 
// of the current element (this function is invoked once for each element),
// while "arrayInfo" informs about the type of the rest of elements in the 
// array.
// Return value : an auxiliar SymbolInfo with different info if the current 
// element and the following elements in the array (from current element to the 
// end) have the same type or not.
// 	If all elements have the same type: 
//		info = size of array (from current element to the end), 
//		symbol = type of elements.
//	If not:
//		info = -1,
//		symbol = NULL.
struct SymbolInfo* checkArrayContent( struct Symbol* type, struct SymbolInfo* arrayInfo )
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
