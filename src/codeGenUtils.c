#include "codeGenUtils.h"

/*This file intends to keep the implementation of the functions and procedures
needed for the code generation part*/

//The last two values are R6 and R7, because they are used by the
//Q to track the variables and the stack, we do not allow the user
//to used them.
int intRegs[8] = {0,0,0,0,0,0,1,1};
int LRURegs[6] = {0,1,2,3,4,5};
int nR = 6;
int nMaxR = 6;

int floatRegs[4] = {0,0,0,0};
int nRR = 4;
int nMaxRR = 4;


int nLabels = 0;
unsigned int topAddress = Z;

extern ExtraInfo* extraInfoPerRegister[8];
extern ExtraInfo* extraInfoPerDoubleRegister[8];
extern int nextRegisterOverflow;
extern int nextDoubleRegisterOverflow ;

const char regStr[][3] = { "R", "RR" };

cstr getRegStr( int isFloat )
{
	return regStr[isFloat];
}



/**************************************************************************/
/*registers a new label and returns the identifier			  */
/**************************************************************************/

int newLabel()
{
    nLabels += 1;
    return nLabels;
}


/**************************************************************************/
/*Returns an available register						  */
/**************************************************************************/
/*0 -> entero, 1-> Flotante*/

void LRU(int reg){
	int index,i,j;
	int flag = 0;
	if(reg != LRURegs[(nMaxR-1)]){
		for(i=0;i<(nMaxR-1);i++){
			if(LRURegs[i] == reg){
				index = i;
				flag = 1;
			}
			if (flag){
				LRURegs[i] = LRURegs[i+1];
			}
		}
	}
	LRURegs[nMaxR-1] = reg;
}

int assignRegisters(int type)
{
    int i=0, aux, index;
    int flag = 0;
    int reg = -1;
	/*Buscar un Registro*/
    if ((type == 0) && (nR>0))
    {
        for (i=0;i<nMaxR;i++)
        {
            if (intRegs[i]==0)
            {
                intRegs[i] = 1;
                nR--;
                reg = i;
                break;
            }
        }
        LRU(reg);
    }
    else if ((type == 1) && (nRR>0)){
    //{ers>1))
        for (i=0;i<nMaxRR;i++)
        {
            if (floatRegs[i]==0)
            {
                floatRegs[i] = 1;
                nRR--;
                reg = i;
                break;
            }
        }
    }

    return reg;   
    /*Si llegamos aquí es que no hay registros libres :(*/
    /*En caso de que no haya registros libres habrá que tirar de pila (A deliberar)*/
}


/**************************************************************************/
/*Frees all the registers						  */
/**************************************************************************/
int freeRegisters()
{
    int i=0;
    nR = 6;
    nRR = 3;
    for (i=0;i<nMaxRR;i++){
        intRegs[i] = 0;
		floatRegs[i] = 0;
	}
	for(i; i<nMaxR; i++ ){
		intRegs[i] = 0;
	}

	return 0;
}




/**************************************************************************/
/*Frees the i-th register						  */
/**************************************************************************/
int freeRegister(int i, int type)
{
   if (i < 0) return -1;
    if (i > 7) return -2;
    if ((type != 0) && (type != 1)) return -3;

    if ((type == 0) /*&& (nR<6)*/){
        intRegs[i]=0;
			nR++;
			DEBUG_MSG( "Liberando registro R%d - OK\n", i );
    }
	else if ((type == 1) /*&& (nRR<5)*/)
	{
    	//if ((i % 2) == 1) return -4;
    	if( i > 3 ) return -4;
    	floatRegs[i]=0;
		nRR++;
		DEBUG_MSG( "Liberando registro RR%d - OK\n", i );
    }
	
    return 0;
}


/**************************************************************************/
/*Code generation for variables						  */
/**************************************************************************/
int variableCodeGeneration;


/**************************************************************************/
/*Searchs all the global variables and also creates the associated code
/**************************************************************************/

int getAllGlobals(FILE* yyout)
{

	Symbol* currentGlobal = NULL;
	
	currentGlobal = nextGlobalVariablePointer();

	if(currentGlobal == NULL) return -1;

	int size = 0;

	int type = 0;
	while(currentGlobal!=NULL)
	{
		size = ((Type*)(((Variable*)(currentGlobal->info))->type->info))->size;
		type = ((Type*)(((Variable*)(currentGlobal->info))->type->info))->id;
	
		topAddress = topAddress - size;
		((Variable*)(currentGlobal->info))->address = topAddress;

		if(type == TYPE_STRING){
		//TODO PENDIENTE DE OBTENER EL TAMAÑO Y MULTIPLICAR		
		}		
		fprintf(yyout,"\tMEM(0x%x,%d); //Memory for var %s \n",topAddress,size, currentGlobal->name);
		/*switch (type){
		case(TYPE_INTEGER):
			fprint("\tI(%d)\n",);
			break;
		case(TYPE_FLOAT):
			
			break;
		case(TYPE_STRING):
			
			break;
		case(TYPE_CHAR):
			
			break;
		case(TYPE_BOOLEAN):
			
			break;
		case(TYPE_CLASS):
			
			break;
		case(TYPE_ARRAY):
			
			break;
		}*/	
				
		currentGlobal = nextGlobalVariablePointer();			
	}
	return 0;
}


/*Generates code for local variables of hidden main method*/
int getAllLocalsMain(FILE* yyout)
{
	fillMainMethodDataSize();
	Method* main = getCurrentScope();
	// New base.
	fprintf( yyout,"\tR6 = R7;\t// New base\n", main->label );

	// Allocate space for local variables.
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate space for local variables\n", main->localsSize );	
}

/**************************************************************************/
/*Returns the actual address for storage in memory*/
/*Also asigns it to the corresponding field in the symbols table*/
/**************************************************************************/

unsigned int returnAddress(int symbolType,cstr id)
{
	Symbol* variable = searchVariable(symbolType, id);
	return ((Variable*)(variable->info))->address;
}

/* Save current used registers to stack */
void storeRegisters(FILE* yyout)
{
	int i;
	for(i = 0; i < 5; i++)
	{
		if(intRegs[i] == 1){
			fprintf(yyout, "\tR7 = R7 - 4;\t//Locate space is stack for one register\n");
			fprintf(yyout, "\tI(R7) = R%d;\t//Store R%d in stack before method call\n", i, i);
		}
	}
	
	for(i = 0; i < 2; i++)
	{
		if(floatRegs[i] == 1){
			fprintf(yyout, "\tR7 = R7 - 4;\t//Locate space is stack for one register\n");
			fprintf(yyout, "\tF(R7) = RR%d;\t//Store RR%d in stack before method call\n", i, i);
		}
	}	
}

/* Load registers form stack */
void loadRegisters(FILE* yyout, int reg, int freg)
{
	int i;
	for(i = 5; i >= 0; i--)
	{
		if(intRegs[i] == 1 && reg != i){
			fprintf(yyout, "\tR%d = I(R7);\t//Load R%d after method call\n", i, i);
			fprintf(yyout, "\tR7 = R7 + 4;\t//Free space in stack for one register\n");			
		}
	}
	
	for(i = 2; i >= 0; i--)
	{
		if(floatRegs[i] == 1 && freg != i){
			fprintf(yyout, "\tRR%d = F(R7);\t//Load RR%d after method call\n", i, i);
			fprintf(yyout, "\tR7 = R7 + 4;\t//Free space in stack for one register\n");	
		}
	}	
}

/*                            Assignement                              */

// FIXME: Terminar integracion con floats.
Symbol* genAssignement(FILE* yyout, SymbolInfo* leftSide, Symbol* rightSide, int insideIfLoop)
{
	ExtraInfo* rightInfo = (ExtraInfo*)(rightSide->info);
	Variable* leftInfo = (Variable*)(leftSide->varSymbol->info);
	int i, arraySize, elementSize;

	if( rightInfo->assignmentType == TYPE_ARRAY || leftSide->info == TYPE_ARRAY ){
		arraySize = ((Type*)(leftInfo->type->info))->arrayInfo->nElements;
		elementSize = ((Type*)(((Type*)(leftInfo->type->info))->arrayInfo->type->info))->size;
	}		

	int isFloat_ = isFloat(leftSide->varSymbol);
	cstr regStr = getRegStr( isFloat_ );
	
	//Left side is a global variable
	if (leftSide->varSymbol->symType == SYM_GLOBAL){					
		//Aquí no afecta el derramado porque la asignacion se hace directamente a memoria.	
		//var = Array / Class
		if( rightInfo->assignmentType == LOAD_ADDRESS ){
			//FIXME Aqui deberiamos cargar la direccion de la variable, para arrays y clases
		}else{
			//Right side is Array.new
			if( rightInfo->assignmentType == TYPE_ARRAY ){
				for( i = 0; i < arraySize; i++ ){
					fprintf(yyout,"\t%c(0x%x + %d) = %s%d; //Initializing %s array (A1)\n",pointerType(leftSide->varSymbol),
						leftInfo->address, elementSize*i, regStr, rightInfo->nRegister, leftSide->varSymbol->name);				
				}
			}else{		
				//Assignement $var[expression] = expression
				if( leftSide->info == TYPE_ARRAY ){
					int reg = ((ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
					fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
						elementSize, leftSide->varSymbol->name);
					fprintf(yyout,"\t%c(0x%x + R%d) = %s%d; //%s[expr] = expr (1)\n",pointerType(leftSide->varSymbol),
						leftInfo->address, reg, regStr, rightInfo->nRegister, leftSide->varSymbol->name );
					freeRegister( reg, 0 );	
					freeSymbol(leftSide->exprSymbol);
				//Assignement $var = expression
				}else{
					fprintf(yyout,"\t%c(0x%x) = %s%d; //%s = expr (1.1) - isFloat_: %i\n",pointerType(leftSide->varSymbol),
						leftInfo->address, regStr, rightInfo->nRegister, leftSide->varSymbol->name, isFloat_);			
				}		
			}
		}			
	//Left side not a global varialbe
	}else if (leftSide->varSymbol->symType == SYM_VARIABLE){
		//Obtenemos la direccion con el desplazamiento y almacenamos
		if( rightInfo->assignmentType == LOAD_ADDRESS ){
			//FIXME Aqui deberiamos cargar la direccion de la variable, para arrays y clases
		}else{	
			//Left side is a local variable
			if(((Variable*)(leftSide->varSymbol->info))->symSubtype == SYM_LOCAL){
				//Right side is Array.new or [e,e,..,e]
				if( rightInfo->assignmentType == TYPE_ARRAY ){
					for( i = 0; i < arraySize; i++ ){	
						fprintf(yyout,"\t%c(R6 - %d) = %s%d; //Initializing %s array (A2)\n",pointerType(leftSide->varSymbol),
							leftInfo->address - elementSize*i, regStr, rightInfo->nRegister, leftSide->varSymbol->name);				
					}	
				}else{	
					//Assignement var[expression] = expression
					if( leftSide->info == TYPE_ARRAY ){
						int reg = ((ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
						fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
							elementSize, leftSide->varSymbol->name);
						fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",reg, reg,
							leftInfo->address, leftSide->varSymbol->name);				
						fprintf(yyout,"\t%c(R6 + R%d) = %s%d; //%s[expr] = expr (2)\n",pointerType(leftSide->varSymbol),
							reg, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
							freeRegister( reg, 0 );		
						freeSymbol(leftSide->exprSymbol);
					//Assignement var = expression
					}else{	
						fprintf(yyout,"\t%c(R6 - %d) = %s%d; //%s = expr (2.1) \n",pointerType(leftSide->varSymbol),
							leftInfo->address, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
					}		
				}	
			}
			//Left side is an argument variable
			else{
				//Right side is Array.new or [e,e,..,e]
				if( rightInfo->assignmentType == TYPE_ARRAY ){
					for( i = 0; i < arraySize; i++ ){	
						fprintf(yyout,"\t%c(R6 + %d) = %s%d; //Initializing %s array (A3)\n",pointerType(leftSide->varSymbol),
							leftInfo->address + elementSize*i, regStr, rightInfo->nRegister, leftSide->varSymbol->name);				
					}	
				}else{				
					//Assignement var[expression] = expression
					if( leftSide->info == TYPE_ARRAY ){
						int reg = ((ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
						fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
							elementSize, leftSide->varSymbol->name);
						fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",reg, reg,
							leftInfo->address, leftSide->varSymbol->name);						
						fprintf(yyout,"\t%c(R6 - R%d) = %s%d; //%s[expr] = expr (3)\n",pointerType(leftSide->varSymbol),
							reg, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
						freeRegister( reg, 0 );	
						freeSymbol(leftSide->exprSymbol);
					//Assignement var = expression
					}else{			
						fprintf(yyout,"\t%c(R6 + %d) = %s%d; //%s = expr (3.1)\n",pointerType(leftSide->varSymbol),
							leftInfo->address, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
					}			
				}
			}
		}
	}

	if(!insideIfLoop){
		int reg = rightInfo->nRegister;
		Method* method = getCurrentScope();
	
		if(method->returnType){	
			int size = method->argumentsSize;	
			if(!isFloat(method->returnType) && !isFloat_){												
				fprintf(yyout,"\t%c(R6 + %d) = R%d; //Store return value\n",
					pointerType(method->returnType), size, reg);
			}else{
				if(isFloat(method->returnType) && isFloat_){
					fprintf(yyout,"\t%c(R6 + %d) = RR%d; //Store return value\n",
						pointerType(method->returnType), size, reg);				
				}
			}		
		}
	}

	freeRegister( rightInfo->nRegister, isFloat_ );
	freeSymbolInfo(leftSide);

	return rightSide;
}


// FIXME: Terminar integracion con floats.
Symbol* genAccessVariable(FILE* yyout,cstr name, int symType, SymbolInfo* atribute, ExtraInfo** extraInfoPerRegister, int* nextRegisterOverflow)
{	
	Symbol* variable = searchVariable(symType, name);
	int isFloat_ = isFloat( variable );
	int reg = assignRegisters( isFloat_ );
	cstr regStr = getRegStr( isFloat_ );

	int elementSize = 0;	
	if (isFloat_ == 0) reg = checkOverflow(yyout, reg, extraInfoPerRegister, nextRegisterOverflow, TYPE_INTEGER);
	
	Symbol* returnSymbol = createExtraInfoSymbol(reg, isFloat_);	
	ExtraInfo* aux = (ExtraInfo*)(returnSymbol->info); 	
	aux->nRegister = reg;
	aux->variable = variable;

	if(atribute->info == SYM_CLASS_VARIABLE){
		//varSymbol gets the Symbol of the variable
		aux->variable = getClassVar(aux->variable,atribute->name);
	}			

	if( atribute->info == TYPE_ARRAY ){
		elementSize = ((Type*)(((Type*)(((Variable*)(aux->variable->info))->type->info))->arrayInfo->type->info))->size;
	}	
	if( symType == SYM_VARIABLE )
	{
		if(((Variable*)(aux->variable->info))->symSubtype == SYM_LOCAL){
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",expReg, expReg,
					returnAddress(symType,aux->variable->name), aux->variable->name);						
				fprintf(yyout,"\t%s%d = %c(R6 + R%d); //%s[expr] = expr (4)\n",regStr, reg, 
					pointerType(aux->variable), expReg, aux->variable->name);	
				freeRegister( expReg, 0 );	
				DEBUG_MSG( "FS A1\n", reg );
				DEBUG_MSG( "atribute: %p\n", atribute );
				DEBUG_MSG( "atribute->exprSymbol->name: %s\n", atribute->exprSymbol->name );
				freeSymbol(atribute->exprSymbol);
				DEBUG_MSG( "FS A2\n", reg );		
			}else{
					fprintf(yyout,"\t%s%d = %c(R6 - %d); //Loading value of var (V1) %s\n",regStr, reg, 
						pointerType(aux->variable), returnAddress(symType,aux->variable->name),
						aux->variable->name);
			}	
		}else{
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout, "\tR%d = R%d + %d; //Calculate local %s position\n",expReg, expReg,
					returnAddress(symType,aux->variable->name), aux->variable->name);						
				fprintf(yyout,"\t%s%d = %c(R6 + R%d); //%s[expr] = expr (5)\n",regStr, reg, 
					pointerType(aux->variable), expReg, aux->variable->name);
				freeRegister( expReg, 0 );	
				DEBUG_MSG( "FS B1\n", reg );
				freeSymbol(atribute->exprSymbol);
				DEBUG_MSG( "FS B2\n", reg );
			}else{
				fprintf(yyout,"\t%s%d = %c(R6 + %d); //Loading value of var (V2) %s\n",regStr,reg, 
					pointerType(aux->variable), returnAddress(symType,aux->variable->name),
					aux->variable->name);
			}			
		}	
	}else{
		if( symType == SYM_GLOBAL )
		{
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout,"\t%s%d = %c(0x%x + R%d); //Loading value of var (V3) %s[expr]\n",regStr, reg, pointerType(aux->variable),
					returnAddress(symType,aux->variable->name), expReg, aux->variable->name);
				freeRegister( expReg, 0 );
				DEBUG_MSG( "FS C1\n", reg );	
				freeSymbol(atribute->exprSymbol);
				DEBUG_MSG( "FS C2\n", reg );
			}else{			
			fprintf(yyout,"\t%s%d = %c(0x%x); //Loading value of var (V4) %s\n", regStr, reg, pointerType(aux->variable), 
				returnAddress(symType,aux->variable->name), aux->variable->name);	
			}
		//FIXME Las constantes van aqui			
		}else{
		}
	}	
	DEBUG_MSG( "FSI D1\n", reg );
	freeSymbolInfo(atribute);
	DEBUG_MSG( "FSI D2\n", reg );
	return returnSymbol;
}


SymbolInfo* genArrayContent( FILE* yyout, SymbolInfo* leftSide, Symbol* literalInfo,
	SymbolInfo* arrayInfo )
{
	Symbol* varSymbol = leftSide->varSymbol;
	int position = arrayInfo->info;
	int address = ((Variable*)(varSymbol->info))->address;
	int nRegister = ((ExtraInfo*)(literalInfo->info))->nRegister;
	int	elementSize = ((Type*)(((Type*)(((Variable*)(varSymbol->info))->type->info))->arrayInfo->type->info))->size;

	int isFloat_ = isFloat(leftSide->varSymbol);
	cstr regStr = getRegStr( isFloat_ );

	switch(varSymbol->symType)
	{
	case SYM_GLOBAL:
		fprintf(yyout,"\t%c(0x%x + %d) = %s%d; //Initializing %s array (A4)\n",pointerType(varSymbol),
			address, elementSize*position, regStr, nRegister, varSymbol->name);				
		break;
	case SYM_VARIABLE:
		if(((Variable*)(varSymbol->info))->symSubtype == SYM_LOCAL){
			fprintf(yyout,"\t%c(R6 + %d) = %s%d; //Initializing %s array (A5)\n",pointerType(varSymbol),
				elementSize*position - address, regStr, nRegister, varSymbol->name);	
		}else{
			fprintf(yyout,"\t%c(R6 - %d) = %s%d; //Initializing %s array (A6)\n",pointerType(varSymbol),
				elementSize*position + address, regStr, nRegister, varSymbol->name);		
		}		
		break;
	case SYM_CONSTANT:
		break;
	default:
		//Error
		printf("Error in array content\n");
		break;				
	}
	arrayInfo->info++;
	freeRegister( ((ExtraInfo*)(literalInfo->info))->nRegister, isFloat_ );	
	freeSymbol(literalInfo);		
	return arrayInfo;
}

/*                             Method definition                             */

// Generate the code for a method "begin" (set method label and new base, and
// allocate local space).
void genMethodBegin( FILE* yyout, cstr methodName, int symType )
{
   // Get the method's info from symbols' table.
	Method* method = NULL;
	if(symType == SYM_METHOD){
  		method = (Method *)( searchTopLevel( symType, methodName )->info );
  	}else{ //It is a block
  		method = (Method *)( searchVariable( symType, methodName )->info );
  		goInScope(method);
  		fillMethodDataSize(method);
  	}
   
   // Print a comment to indicate the method definitions' begin.
	fprintf( yyout, "\n\t/* Procedure [%s] - begin */\n", methodName );
   
	// Set method label. 
	fprintf( yyout,"L %i:\n", method->label );

	// New base.
	fprintf( yyout,"\tR6 = R7;\t// New base\n", method->label );

	// Allocate space for local variables.
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate space for local variables\n", method->localsSize );
}


// Generate the code for a method "end" (free local data and return).
void genMethodEnd( FILE* yyout, cstr methodName, int symType )
{
	// Get the method's info from symbols' table.
	Method* method = NULL;
	if(symType == SYM_METHOD){
  		method = (Method *)( searchTopLevel( symType, methodName )->info );
  	}else{
  		method = (Method *)( searchVariable( symType, methodName )->info );
  	}	

	// Free local memory.
	fprintf( yyout,"\tR7 = R6;\t// Free local variables\n", method->localsSize );

	// Retrieve previous base.
	fprintf( yyout,"\tR6 = P(R7+4);\t// Retrieve base\n", method->localsSize );

	// Get return label.
	fprintf( yyout,"\tR5 = P(R7);\t// Get return label\n", method->localsSize );

	// Return from method.
	fprintf( yyout,"\tGT(R5);\t// Return to caller\n", method->localsSize );

	// Print a comment to indicate the method definition's end.
	fprintf( yyout, "\t/* Procedure [%s] - end */\n\n", methodName );
}

// Generate the code for a block "begin" (set method label and new base, and
// allocate local space).
struct Symbol* genBlockBegin( FILE* yyout, cstr varName, cstr argumentName )
{
	//Get current scope
	struct Method* scope = getCurrentScope();
	
	//Get a label for the hidden method
	int nextCodeLabel = newLabel(); 
	
	//Save the scope in the ExtraInfo struct
	struct Symbol* currentScope = createExtraInfoSymbol(nextCodeLabel, 0);
	((struct ExtraInfo*)(currentScope->info))->variable = (struct Symbol*)scope;
	
	//This is a method definition so do not execute yet	
	fprintf( yyout,"\tGT(%d); //Jump to next code block\n", nextCodeLabel); 
	
	char *blockName = createBlockName(varName, argumentName);
	
	//Generate block definition using method function
	genMethodBegin(yyout, (cstr)blockName, SYM_BLOCK);
	
	free(blockName);
	return currentScope;
}

// Generate the code for a block "end" (free local data and return).
void genBlockEnd( FILE* yyout, cstr varName, cstr argumentName,struct Symbol* blockInfo)
{
	char *blockName = createBlockName(varName, argumentName);
	struct ExtraInfo* extraInfo = (struct ExtraInfo*)(blockInfo->info);
	
	//Retrieve label for the execution of code
	int nextCodeLabel = extraInfo->nRegister;
	
	//Retrive previous scope 
	struct Method* method = (struct Method*)(extraInfo->variable);
	
	//Create block defintion end with method definition
	genMethodEnd(yyout, (cstr)blockName, SYM_BLOCK);
	
	free(blockName);	
	goInScope(method);
	fprintf( yyout,"L %d: //Continue code block\n", nextCodeLabel);
}


/*                               Method call                                 */

// Generate the code for a method call "begin" (arguments memory allocation).
void genMethodCallBegin( FILE* yyout, cstr methodName, int symType )
{
	// Get the method's info from symbols' table.
	Method* method = NULL;
	if(symType == SYM_METHOD){
  		method = (Method *)( searchTopLevel( symType, methodName )->info );
  	}else{
  		method = (Method *)( searchVariable( symType, methodName )->info );
  	}	

	// Print a comment to indicate the method call's begin.
	fprintf( yyout, "\n\t/* Call to procedure [%s] - begin */\n", methodName );
	
	// Allocate memory for arguments (+8 bytes more for previous base and return
	// label).
	int totalSize = method->argumentsSize;
	if(method->returnType){
		totalSize += ((Type*)(method->returnType->info))->size;
	}	
	
	//Store used registers is stack
	storeRegisters(yyout);
		
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments and return value\n", totalSize );
}



// Generate the code for a method call (save base and return label and call
// method).
void genMethodCall( FILE* yyout, Method* method, int reg )
{
	int newLabel_ = newLabel();
	int totalSize = method->argumentsSize;
	int freg = -1;
	
	// Save base
	fprintf( yyout, "\tP(R7+4) = R6;\t// Save base\n" );

	// Save return label
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	
	// Call method
	fprintf( yyout, "\tGT(%i);\t// Call method\n", method->label );

	// Set return label
	fprintf( yyout, "L %i:\n", newLabel_ );
	
	if(method->returnType){
		if(!isFloat(method->returnType)){
		// Save return value
		fprintf( yyout, "\tR%d = %c(R7+%d); // Get return value\n", reg, 
			pointerType(method->returnType), totalSize);		
		}else{
		// Save return value
		fprintf( yyout, "\tRR%d = %c(R7+%d); // Get return value\n", reg, 
			pointerType(method->returnType), totalSize);		
			freg = reg;
			reg = -1;				
		}	
		totalSize += ((Type*)(method->returnType->info))->size;
	}
	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments and return value\n", totalSize );

	// Load the used registers from the stack
	loadRegisters(yyout, reg, freg);	
	// Print a comment to indicate the method call's end.
	fprintf( yyout, "\t/* Call to procedure - end */\n\n" );
}

// Generate the code for a parameter pass. Arguments:
// - iRegister - index of register with the argument's value.
// - method - called method symbol.
// - iArgument - argument index.
void genArgumentPass( FILE* yyout, Symbol* argumentSymbol, Symbol* method, int iArgument )
{
	Symbol* argument = getMethodArgument( method, iArgument );
	int iRegister = ((ExtraInfo*)(argumentSymbol->info))->nRegister;
	int address = ((Variable*)( argument->info ) )->address;

	int isFloat_ = isFloat( argumentSymbol );
	cstr regStr = getRegStr( isFloat_ );

	// Get parameter.
	fprintf( yyout,"\t%c(R7+%d) = %s%d;\t// %iº Argument\n", pointerType( argument ), address, regStr, iRegister, iArgument+1 );
	freeRegister( iRegister, isFloat_ );
	freeSymbol(argumentSymbol);
}

//Generate all the calls to the block
void genBlockCall( FILE* yyout, cstr varName, cstr argumentName )
{
	char *blockName = createBlockName(varName, argumentName);
	
	struct Symbol* block = searchVariable( SYM_BLOCK, blockName );
	
	struct Symbol* variable = searchVariable( SYM_VARIABLE, varName );
	struct Symbol* varType = ((struct Variable*)(variable->info))->type;
	struct SymbolInfo* info = NULL;
	
	int reg, expReg, i;
	int varIsFloat = isFloat( variable );
	int size = ((struct Type*)(varType->info))->arrayInfo->nElements;
	//reg = assignRegisters(varIsFloat);
	for( i = 0; i < size;i++){
		expReg = assignRegisters(0);

		struct Symbol* extraInfo;// = createExtraInfoSymbol(reg);
		//((struct ExtraInfo*)(extraInfo->info))->variable = varType;
	
		struct Symbol* expExtraInfo = createExtraInfoSymbol(expReg, varIsFloat);
	
		((struct ExtraInfo*)(extraInfo->info))->variable = searchType(TYPE_INTEGER);	

		genMethodCallBegin( yyout, blockName, SYM_BLOCK );
		
		fprintf( yyout,"\tR%d = %d;\t// Loading literal %d\n", expReg, i, i);
	
		info = malloc(sizeof(struct SymbolInfo));
		info->symbol = NULL;
		info->info = TYPE_ARRAY; 
		info->name = NULL;
		info->exprSymbol = expExtraInfo;

		if (varIsFloat){
			extraInfo = genAccessVariable(yyout, varName, SYM_VARIABLE, info, extraInfoPerDoubleRegister, &nextDoubleRegisterOverflow);
		}else{ 
			extraInfo = genAccessVariable(yyout, varName, SYM_VARIABLE, info, extraInfoPerRegister, &nextRegisterOverflow);
		}	
		
		genArgumentPass( yyout, extraInfo, block, 0 );
	
		genMethodCall(yyout, (struct Method*)(block->info), -1 );
	}
	free(blockName);	

}

// Gets the Q type corresponding to the type of the variable
char pointerType(Symbol* symbol)
{
	int typeId;
	Type* type = NULL;
	
	if(symbol->symType == SYM_TYPE){
		// Symbol is a type
		type = (Type*)(symbol->info);		
	}else{ // symbol is a variable
		type = (Type*)(((Variable*)(symbol->info))->type->info);
	}
	
	typeId = type->id;
	
	if(typeId == TYPE_ARRAY){
		typeId = ((Type*)(type->arrayInfo->type->info))->id;
	}
	
	switch( typeId ){
		case TYPE_INTEGER:
		case TYPE_BOOLEAN:
			return 'I';
		break;
		case TYPE_FLOAT:
			return 'F';
		break;
		case TYPE_CHAR:
			return 'U';
		break;	
	}
	return 'E';
}


void genOperation(FILE* yyout, Symbol* leftSide, Symbol* rightSide, char* op )
{
	int r0, r1;
	r0 = ((ExtraInfo*)(leftSide->info))->nRegister;
	r1 = ((ExtraInfo*)(rightSide->info))->nRegister;
	int isFloat_ = isFloat( leftSide );

	if( !isFloat_ ){
		if(r0 == 7){
			r0 = assignRegisters(0);
			((ExtraInfo*)(leftSide->info))->nRegister = r0;
			fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4;\n", r0/*pointerType(((ExtraInfo*)(leftSide->info))->variable)*/);
		}
		if(r1 == 7){
			r1 = assignRegisters(0);
			((ExtraInfo*)(leftSide->info))->nRegister = r1;
			fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4;\n", r1/*pointerType(((ExtraInfo*)(rightSide->info))->variable)*/);
		}
		fprintf(yyout, "\tR%d = R%d %s R%d;\n", r0, r0,op, r1);
		//freeRegister(r1, 0);
	}else{
		if(r0 == 77){
			r0 = assignRegisters(1);
			((ExtraInfo*)(leftSide->info))->nRegister = r0;
			fprintf(yyout, "\tRR%d = I(R7);\n\tR7 = R7 + 4;\n", r0/*pointerType(((ExtraInfo*)(leftSide->info))->variable)*/);
		}
		if(r1 == 77){
			r1 = assignRegisters(1);
			((ExtraInfo*)(leftSide->info))->nRegister = r1;
			fprintf(yyout, "\tRR%d = I(R7);\n\tR7 = R7 + 4;\n", r1/*pointerType(((ExtraInfo*)(rightSide->info))->variable)*/);
		}
		fprintf(yyout, "\tRR%d = RR%d %s RR%d;\n", r0, r0,op, r1);
		//freeRegister(r1, 1);
	}
	
	freeRegister( r1, isFloat_ );
	freeSymbol(rightSide);
}


/*                                  I/O                                      */

// Generate a comment indicating the start of a puts call.
void genPutsCallHeader( FILE* yyout )
{
	// Print a comment to indicate the puts call's begin.
	fprintf( yyout, "\n\t/* Call to puts - begin*/\n" );
}


// Generate the code for a puts call (variable interpolation are not handle 
// here).
void genPutsCall( FILE* yyout, cstr str )
{
	// argumentsSize = 4 (return label) 4 (n variables) + 1 (null character) 
	// + strlen(str).
	// (*) Code for variables pass to puts is generated before calling this
	// method. That's why we don't allocate memory here for them. However,
	// once puts is called, all the memory has to be deallocated, so as the 
	// string literal is processed, we calculate the real "argumentsSize" for
	// futher deallocation.
	int argumentsSize = 9 + strlen( str );
	int i;
	
	// Allocate memory for arguments.
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments\n", argumentsSize );
	
	// Process string literal and pass it to puts.
	fprintf( yyout, "\t/* Call to puts - string */\n" );
	for( i=0; i<strlen(str); i++ ){
		switch( str[i] ){
			case '\n':
				fprintf( yyout,"\tU(R7+%d) = '\\n';\n", 8+i );
			break;
			case '\t':
				fprintf( yyout,"\tU(R7+%d) = '\\t';\n", 8+i );
			break;
			case '\\':
				fprintf( yyout,"\tU(R7+%d) = '\\\\';\n", 8+i );
			break;
			case '\"':
				fprintf( yyout,"\tU(R7+%d) = '\"';\n", 8+i );
			break;
			case '\'':
				fprintf( yyout,"\tU(R7+%d) = '\\'';\n", 8+i );
			break;
			case '%':
				if( str[i+1] == 'U' ){
					argumentsSize++;
				}else{
					argumentsSize += 4;
				}
			default:
				fprintf( yyout,"\tU(R7+%d) = '%c';\n", 8+i, str[i] );
			break;
		}
	}
	fprintf( yyout,"\tU(R7+%d) = '\\000';\n", 8+i ); // Null character.
	
	int newLabel_ = newLabel();

	// Save arguments size.
	fprintf( yyout, "\tP(R7+4) = %i;\t// Set arguments size (string + values)\n", argumentsSize );

	// Save return label.
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	// Call method.
	fprintf( yyout, "\tGT(-13);\t// Call puts\n" );

	// Set return label.
	fprintf( yyout, "L %i:\n", newLabel_ );

	// Free arguments memory.
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments\n", argumentsSize );
	
	// Print a comment to indicate the puts call's end.
	fprintf( yyout, "\t/* Call to puts - end */\n\n" );
}


// Generate code for a "#{<variable>}" pattern in string literals. Also return 
// an "interpolationMark" ("%I", "%F", "%U", "%E") so grammar can build the
// string literal that will be past to the puts call.
cstr genVariableInterpolation( FILE* yyout, Symbol* symbol )
{
	static char interpolationMarks[][3] = { "%I", "%F", "%U", "%E" };

	// Get type and register of symbol.
	ExtraInfo* info = (ExtraInfo*)(symbol->info);
	int reg = info->nRegister;
	int type = getType( symbol );
	int isFloat_ = isFloat( symbol );

	// Generate code for passing the variable to puts. Also index the 
	// interpolationMarks vector.
	int mark = 0;
	switch( type ){
			case TYPE_INTEGER:
				fprintf( yyout, "\tR7 = R7-4;\n" );
				fprintf( yyout, "\tI(R7) = R%d;\n", reg );
				mark = 0;
			break;
			case TYPE_FLOAT:
				fprintf( yyout, "\tR7 = R7-4;\n" );
				fprintf( yyout, "\tF(R7) = RR%d;\n", reg );
				mark = 1;
			break;
			case TYPE_CHAR:
				fprintf( yyout, "\tR7 = R7-1;\n" );
				fprintf( yyout, "\tU(R7) = R%d;\n", reg );
				mark = 2;
			break;
			default:
				// FIXME: Better idea about showing error?
				// FIXME: Outside this is not tested.
				mark = 3;
			break;
	}
	
	// Free resources and return the interpolation mark.
	// FIXME: Good idea free them here?
	freeRegister( reg, isFloat_ );
	freeSymbol( symbol );
	return interpolationMarks[mark];
}

// Generate code for a call to a member of the "get" family of functions (geti, 
// getf, etc). 
// The argument "inputType" can be 'I' (geti), 'F' (getf) or 'U' (getu).
void genGetCall( FILE* yyout, char inputType, int reg )
{
	// size = 4 (used register) 4 (return label) + 1 (inputType)
	int size = 9;

	// Print a comment to indicate the get call's begin.
	fprintf( yyout, "\n\t/* Call to get (%c) - begin */\n", inputType );

	// Allocate memory for arguments
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments\n", size );
	
	int newLabel_ = newLabel();

	// Pointer to string
	fprintf( yyout, "\tU(R7+4) = '%c';\t// Save inputType\n", inputType );

	// Save return label
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	// Call method
	fprintf( yyout, "\tGT(-14);\t// Call get (%c)\n", inputType );

	// Set return label
	fprintf( yyout, "L %i:\n", newLabel_ );

	// Save returned value
	if( inputType != 'F' ){
		fprintf( yyout, "\tR%d = %c(R7+5);\t// Save returned value\n", reg, inputType );
	}else{
		// FIXME: Aun falla con los flotantes.
		//reg = 0;
		fprintf( yyout, "\tRR%d = %c(R7+5);\t// Save returned value\n", reg, inputType );
	}

	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments\n", size );
	
	// Print a comment to indicate the puts call's end.
	fprintf( yyout, "\t/* Call to get (%c) - end */\n\n", inputType );
}

/*				Overflow				*/
int checkOverflow(FILE* yyout, int reg, ExtraInfo** extraInfoPerRegister, int* nextRegisterOverflow, int type){

	if (reg == -1)
	{
		switch(type)
		{
			case TYPE_INTEGER:
			case TYPE_CHAR:
			case TYPE_BOOLEAN:
				DEBUG_MSG( "CHECK_OVER_FLOW_INTEGERS_1", reg );
				//reg = extraInfoPerRegister[LRURegs[0]]->nRegister;
				reg = extraInfoPerRegister[*nextRegisterOverflow]->nRegister;
				fprintf(yyout,"\tR7 = R7-4;\n\tI(R7) = R%d;\t//register overflow\n",reg);
				//extraInfoPerRegister[LRURegs[0]]->nRegister = 7;
				extraInfoPerRegister[*nextRegisterOverflow]->nRegister = 7;
				printf("nextReg almacenaria %d\n", *nextRegisterOverflow);
				printf("LRU almacenaria %d\n",LRURegs[0]);
				LRU(reg);
				
				*nextRegisterOverflow = ((*nextRegisterOverflow)+1)%nMaxR;
				DEBUG_MSG( "CHECK_OVER_FLOW_INTEGERS_2", reg );
			
			break;
			case TYPE_FLOAT:
				DEBUG_MSG( "CHECK_OVER_FLOW_FLOATS_1", reg );
				reg = extraInfoPerRegister[*nextRegisterOverflow]->nRegister;
				fprintf(yyout,"\tR7 = R7-4;\n\tF(R7) = R%d;\t//register overflow\n",reg);
				extraInfoPerRegister[*nextRegisterOverflow]->nRegister = 77;
				*nextRegisterOverflow = ((*nextRegisterOverflow)++)%nMaxRR;
				DEBUG_MSG( "CHECK_OVER_FLOW_FLOATS_2", reg );
			
			break;
			default:
				reg = -1;
			break;
		}
	}

	return reg;
	
}

/*                              Auxiliar functions                           */

// If symbol refers to a variable/value (directly or indirectly), return its 
// type.
// FIXME: Terminar integracion con floats.
// FIXME: Y con las clases???
// FIXME: Hasta ahora no se comprueba desde fuera que getType devuelva -1
// o no.
int getType( Symbol* symbol )
{
	Symbol* symVariable = NULL;
	Type* type = NULL;

	// If symbol is NULL, return.
	if( !symbol ){
		DEBUG_MSG( "getType( NULL ) - Returning\n", type );
		return -1;
	}

	DEBUG_MSG( "getType( %i ) - BEGIN\n", symbol->symType );

	// Access to the type struct of the symbol.
	switch( symbol->symType ){
		case SYM_VARIABLE:
			type = ((Type*)((Variable*)(symbol->info))->type->info);
		break;
		case SYM_EXTRA_INFO:
			symVariable = ((ExtraInfo*)(symbol->info))->variable;
			return getType( symVariable );
			/*
			if( symVariable->symType == SYM_VARIABLE ){
				type = ((Type*)((Variable*)(symVariable->info))->type->info);
			}else{
				type = ((Type*)(symVariable->info));
			}
			*/
		break;
		case SYM_TYPE:
			type = ((Type*)(symbol->info));
		break;
		default:
			DEBUG_MSG( "\n\ngetType returned -1!!!\n\n", type );
			return -1;
		break;
	}
	if( type->id == TYPE_ARRAY ){
		type = ((Type*)(type->arrayInfo->type->info));
	}	

	DEBUG_MSG( "getType() - END\n", type );

	if( type->id < 1 || type->id > 2 ){
		DEBUG_MSG( "\n\ngetType something different to 1 (INTEGER) and 2 (FLOAT) (%i)!!!\n\n", type->id );
	}
	return type->id;
}

// Return 1 if symbol refers to a float variable (directly or indirectly). 
// Otherwise return 0.
int isFloat( Symbol* symbol )
{
	return ( getType( symbol ) == TYPE_FLOAT );
}

