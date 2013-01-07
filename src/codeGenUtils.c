#include "codeGenUtils.h"

/*This file intends to keep the implementation of the functions and procedures
needed for the code generation part*/

//The last two values are R6 and R7, because they are used by the
//Q to track the variables and the stack, we do not allow the user
//to used them.
int intRegs[8] = {0,0,0,0,0,0,1,1};
int nR = 6;

int floatRegs[3] = {0,0,0};
int nRR = 3;

int nLabels = 0;
unsigned int topAddress = Z;

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
    nLabels++;
    return nLabels;
}


/**************************************************************************/
/*Returns an available register						  */
/**************************************************************************/
/*0 -> entero, 1-> Flotante*/
int assignRegisters(int type)
{
    int i=0;
	/*Buscar un Registro*/
    if ((type == 0) && (nR>0))
    {
        for (i=0;i<6;i++)
        {
            if (intRegs[i]==0)
            {
                intRegs[i] = 1;
                nR--;
                return i;
            }
        }
    }
    else if ((type == 1) && (nRR>0)){
    //{ers>1))
        for (i=0;i<4;i++)
        {
            if (floatRegs[i]==0)
            {
                floatRegs[i] = 1;
                nRR--;
                return i;
            }
        }
    }

    return -1;   
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
    for (i=0;i<3;i++){
        intRegs[i] = 0;
		  floatRegs[i] = 0;
	}
	for( i; i<6; i++ ){
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
			printf( "Liberando registro R%d - OK\n", i );
    }
	else if ((type == 1) /*&& (nRR<5)*/)
	{
    	//if ((i % 2) == 1) return -4;
    	floatRegs[i]=0;
		nRR++;
		printf( "Liberando registro RR%d - OK\n", i );
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

	struct Symbol* currentGlobal = NULL;
	
	currentGlobal = nextGlobalVariablePointer();

	if(currentGlobal == NULL) return -1;

	int size = 0;

	int type = 0;
	while(currentGlobal!=NULL)
	{
		size = ((struct Type*)(((struct Variable*)(currentGlobal->info))->type->info))->size;
		type = ((struct Type*)(((struct Variable*)(currentGlobal->info))->type->info))->id;
	
		topAddress = topAddress - size;
		((struct Variable*)(currentGlobal->info))->address = topAddress;

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
	struct Method* main = getCurrentScope();
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
	return ((struct Variable*)(variable->info))->address;
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
			fprintf(yyout, "\tI(R7) = RR%d;\t//Store RR%d in stack before method call\n", i, i);
		}
	}	
}

/* Load registers form stack */
void loadRegisters(FILE* yyout, int reg, int freg)
{
	int i;
	for(i = 0; i < 5; i++)
	{
		if(intRegs[i] == 1 && reg != i){
			fprintf(yyout, "\tR%d = I(R7);\t//Load R%d after method call\n", i, i);
			fprintf(yyout, "\tR7 = R7 + 4;\t//Free space in stack for one register\n");			
		}
	}
	
	for(i = 0; i < 2; i++)
	{
		if(floatRegs[i] == 1 && freg != i){
			fprintf(yyout, "\tRR%d = I(R7);\t//Load RR%d after method call\n", i, i);
			fprintf(yyout, "\tR7 = R7 + 4;\t//Free space in stack for one register\n");	
		}
	}	
}

/*                            Assignement                              */

// FIXME: Terminar integracion con floats.
struct Symbol* genAssignement(FILE* yyout, struct SymbolInfo* leftSide, struct Symbol* rightSide, int insideIfLoop)
{
	struct ExtraInfo* rightInfo = (struct ExtraInfo*)(rightSide->info);
	struct Variable* leftInfo = (struct Variable*)(leftSide->varSymbol->info);
	int i, arraySize, elementSize;

	if( rightInfo->assignmentType == TYPE_ARRAY || leftSide->info == TYPE_ARRAY ){
		arraySize = ((struct Type*)(leftInfo->type->info))->arrayInfo->nElements;
		elementSize = ((struct Type*)(((struct Type*)(leftInfo->type->info))->arrayInfo->type->info))->size;
	}		

	int isFloat_ = isFloat(leftSide->varSymbol);
	cstr regStr = getRegStr( isFloat_ );

	fprintf( yyout, "\n//GENERANDO ASIGNACION [%s] - BEGIN\n", regStr );
	
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
					fprintf(yyout,"\t%c(0x%x + %d) = R%d; //Initializing %s array (A1)\n",pointerType(leftSide->varSymbol),
						leftInfo->address, elementSize*i, rightInfo->nRegister, leftSide->varSymbol->name);				
				}
			}else{		
				//Assignement $var[expression] = expression
				if( leftSide->info == TYPE_ARRAY ){
					int reg = ((struct ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
					fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
						elementSize, leftSide->varSymbol->name);
					fprintf(yyout,"\t%c(0x%x + R%d) = R%d; //%s[expr] = expr (1)\n",pointerType(leftSide->varSymbol),
						leftInfo->address, reg, rightInfo->nRegister, leftSide->varSymbol->name);
					freeRegister( reg, isFloat_ );	
					freeSymbol(leftSide->exprSymbol);
				//Assignement $var = expression
				}else{
					fprintf(yyout,"\t%c(0x%x) = R%d; //%s = expr\n",pointerType(leftSide->varSymbol),
						leftInfo->address, rightInfo->nRegister, leftSide->varSymbol->name);			
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
			if(((struct Variable*)(leftSide->varSymbol->info))->symSubtype == SYM_LOCAL){
				//Right side is Array.new or [e,e,..,e]
				if( rightInfo->assignmentType == TYPE_ARRAY ){
					for( i = 0; i < arraySize; i++ ){	
						fprintf(yyout,"\t%c(R6 - %d) = %s%d; //Initializing %s array (A2)\n",pointerType(leftSide->varSymbol),
							leftInfo->address - elementSize*i, regStr, rightInfo->nRegister, leftSide->varSymbol->name);				
					}	
				}else{	
					//Assignement var[expression] = expression
					if( leftSide->info == TYPE_ARRAY ){
						int reg = ((struct ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
						fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
							elementSize, leftSide->varSymbol->name);
						fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",reg, reg,
							leftInfo->address, leftSide->varSymbol->name);				
						fprintf(yyout,"\t%c(R6 + R%d) = %s%d; //%s[expr] = expr (2)\n",pointerType(leftSide->varSymbol),
							reg, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
							freeRegister( reg, isFloat_ );		
						freeSymbol(leftSide->exprSymbol);
					//Assignement var = expression
					}else{	
						fprintf(yyout,"\t%c(R6 - %d) = %s%d; //%s = expr\n",pointerType(leftSide->varSymbol),
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
						int reg = ((struct ExtraInfo*)(leftSide->exprSymbol->info))->nRegister;
						fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",reg, reg,
							elementSize, leftSide->varSymbol->name);
						fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",reg, reg,
							leftInfo->address, leftSide->varSymbol->name);						
						fprintf(yyout,"\t%c(R6 - R%d) = R%d; //%s[expr] = expr (3)\n",pointerType(leftSide->varSymbol),
							reg, rightInfo->nRegister, leftSide->varSymbol->name);
						freeRegister( reg, isFloat_ );	
						freeSymbol(leftSide->exprSymbol);
					//Assignement var = expression
					}else{			
						fprintf(yyout,"\t%c(R6 + %d) = %s%d; //%s = expr\n",pointerType(leftSide->varSymbol),
							leftInfo->address, regStr, rightInfo->nRegister, leftSide->varSymbol->name);
					}			
				}
			}
		}
	}

	if(!insideIfLoop){
		int reg = rightInfo->nRegister;
		struct Method* method = getCurrentScope();
	
		if(method->returnType){	
			int size = method->argumentsSize;									
			fprintf(yyout,"\t%c(R6 + %d) = R%d; //Store return value\n",
				pointerType(method->returnType), size, reg);
		}
	}

	freeRegister( rightInfo->nRegister, isFloat_ );
	freeSymbolInfo(leftSide);

	fprintf( yyout, "\n//GENERANDO ASIGNACION [%s] - END\n", regStr );
	return rightSide;
}

// FIXME: Terminar integracion con floats.
struct Symbol* genAccessVariable(FILE* yyout,cstr name, int symType, struct SymbolInfo* atribute)
{
	int reg = assignRegisters(0); 
	int elementSize = 0;	
	struct Symbol* returnSymbol = createExtraInfoSymbol(reg);	
	struct ExtraInfo* aux = (struct ExtraInfo*)(returnSymbol->info); 	
	aux->nRegister = reg;
	aux->variable = searchVariable(symType, name);

	int isFloat_ = isFloat(returnSymbol);
	// FIXME: este if esta hecho a lo chanada. Cambiarlo y usar getRegStr();
	char regStr[3];
	if( isFloat_ ){
		strcpy( regStr, "RR" );
		freeRegister( reg, 0 );
		reg = assignRegisters(1);
		aux->nRegister = reg;
	}else{
		strcpy( regStr, "R" );
	}

	if(atribute->info == SYM_CLASS_VARIABLE){
		//varSymbol gets the struct Symbol of the variable
		aux->variable = getClassVar(aux->variable,atribute->name);
	}			

	if( atribute->info == TYPE_ARRAY ){
		elementSize = ((struct Type*)(((struct Type*)(((struct Variable*)(aux->variable->info))->type->info))->arrayInfo->type->info))->size;
	}	
	
	if( symType == SYM_VARIABLE )
	{
		if(((struct Variable*)(aux->variable->info))->symSubtype == SYM_LOCAL){
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((struct ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout, "\tR%d = R%d - %d; //Calculate local %s position\n",expReg, expReg,
					returnAddress(symType,aux->variable->name), aux->variable->name);						
				fprintf(yyout,"\t%s%d = %c(R6 + R%d); //%s[expr] = expr (4)\n",regStr, reg, 
					pointerType(aux->variable), expReg, aux->variable->name);	
				freeRegister( expReg, isFloat_ );	
				freeSymbol(atribute->exprSymbol);			
			}else{
					fprintf(yyout,"\t%s%d = %c(R6 - %d); //Loading value of var - %s\n",regStr, reg, 
						pointerType(aux->variable), returnAddress(symType,aux->variable->name),
						aux->variable->name);
			}	
		}else{
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((struct ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout, "\tR%d = R%d + %d; //Calculate local %s position\n",expReg, expReg,
					returnAddress(symType,aux->variable->name), aux->variable->name);						
				fprintf(yyout,"\t%s%d = %c(R6 + R%d); //%s[expr] = expr (5)\n",regStr, reg, 
					pointerType(aux->variable), expReg, aux->variable->name);
				freeRegister( expReg, isFloat_ );	
				freeSymbol(atribute->exprSymbol);			
			}else{
				fprintf(yyout,"\tR%d = %c(R6 + %d); //Loading value of var %s\n",reg, 
					pointerType(aux->variable), returnAddress(symType,aux->variable->name),
					aux->variable->name);
			}			
		}	
	}else{
		if( symType == SYM_GLOBAL )
		{
			if( atribute->info == TYPE_ARRAY ){
				int expReg = ((struct ExtraInfo*)(atribute->exprSymbol->info))->nRegister;
				fprintf(yyout, "\tR%d = R%d * %d; //Calculate array %s position\n",expReg, expReg,
					elementSize, aux->variable->name);
				fprintf(yyout,"\t%s%d = %c(0x%x + R%d); //Loading value of var %s[expr]\n",regStr, reg, pointerType(aux->variable),
					returnAddress(symType,aux->variable->name), expReg, aux->variable->name);
				freeRegister( expReg, isFloat_ );	
				freeSymbol(atribute->exprSymbol);
			}else{			
			fprintf(yyout,"\tR%d = %c(0x%x); //Loading value of var %s\n", reg, pointerType(aux->variable), 
				returnAddress(symType,aux->variable->name), aux->variable->name);	
			}
		//FIXME Las constantes van aqui			
		}else{
		}
	}	
	freeSymbolInfo(atribute);
	return returnSymbol;
}


struct SymbolInfo* genArrayContent( FILE* yyout, struct SymbolInfo* leftSide, struct Symbol* literalInfo,
	struct SymbolInfo* arrayInfo )
{
	struct Symbol* varSymbol = leftSide->varSymbol;
	int position = arrayInfo->info;
	int address = ((struct Variable*)(varSymbol->info))->address;
	int nRegister = ((struct ExtraInfo*)(literalInfo->info))->nRegister;
	int	elementSize = ((struct Type*)(((struct Type*)(((struct Variable*)(varSymbol->info))->type->info))->arrayInfo->type->info))->size;

	int isFloat_ = isFloat(leftSide->varSymbol);
	cstr regStr = getRegStr( isFloat_ );

	switch(varSymbol->symType)
	{
	case SYM_GLOBAL:
		fprintf(yyout,"\t%c(0x%x + %d) = %s%d; //Initializing %s array (A4)\n",pointerType(varSymbol),
			address, elementSize*position, regStr, nRegister, varSymbol->name);				
		break;
	case SYM_VARIABLE:
		if(((struct Variable*)(varSymbol->info))->symSubtype == SYM_LOCAL){
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
	freeRegister( ((struct ExtraInfo*)(literalInfo->info))->nRegister, isFloat_ );	
	freeSymbol(literalInfo);		
	return arrayInfo;
}

/*                             Method definition                             */

// Generate the code for a method "begin" (set method label and new base, and
// allocate local space).
void genMethodBegin( FILE* yyout, cstr methodName )
{
   // Get the method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );
   
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
void genMethodEnd( FILE* yyout, cstr methodName )
{
	// Get the method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );

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


/*                               Method call                                 */

// Generate the code for a method call "begin" (arguments memory allocation).
void genMethodCallBegin( FILE* yyout, cstr methodName )
{
	// Get the called method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );

	// Print a comment to indicate the method call's begin.
	fprintf( yyout, "\n\t/* Call to procedure [%s] - begin */\n", methodName );
	
	// Allocate memory for arguments (+8 bytes more for previous base and return
	// label).
	int totalSize = method->argumentsSize;
	if(method->returnType){
		totalSize += ((struct Type*)(method->returnType->info))->size;
	}	
	
	//Store used registers is stack
	storeRegisters(yyout);
		
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments and return value\n", totalSize );
}

// Generate the code for a method call (save base and return label and call
// method).
void genMethodCall( FILE* yyout, struct Method* method, int reg )
{
	int newLabel_ = newLabel();
	int totalSize = method->argumentsSize;
	
	// Save base
	fprintf( yyout, "\tP(R7+4) = R6;\t// Save base\n" );

	// Save return label
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	
	// Call method
	fprintf( yyout, "\tGT(%i);\t// Call method\n", method->label );

	// Set return label
	fprintf( yyout, "L %i:\n", newLabel_ );
	
	if(method->returnType){
		// Save return value
		fprintf( yyout, "\tR%d = %c(R7+%d); // Get return value\n", reg, 
			pointerType(method->returnType), totalSize);	
		totalSize += ((struct Type*)(method->returnType->info))->size;	
	}
	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments and return value\n", totalSize );

	// Load the used registers from the stack
	//FIXME Cambiar el 4 por el valor real del registro float
	loadRegisters(yyout, reg, 4);	
	// Print a comment to indicate the method call's end.
	fprintf( yyout, "\t/* Call to procedure - end */\n\n" );
}

// Generate the code for a parameter pass. Arguments:
// - iRegister - index of register with the argument's value.
// - method - called method symbol.
// - iArgument - argument index.
void genArgumentPass( FILE* yyout, struct Symbol* argumentSymbol, Symbol* method, int iArgument )
{
	Symbol* argument = getMethodArgument( method, iArgument );
	int iRegister = ((struct ExtraInfo*)(argumentSymbol->info))->nRegister;
	int address = ((struct Variable*)( argument->info ) )->address;

	int isFloat_ = isFloat( argumentSymbol );
	cstr regStr = getRegStr( isFloat_ );

	// Get parameter.
	fprintf( yyout,"\t%c(R7+%d) = %s%d;\t// %iº Argument\n", pointerType( argument ), address, regStr, iRegister, iArgument+1 );
	freeRegister( iRegister, isFloat_ );
	freeSymbol(argumentSymbol);
}


// Gets the Q type corresponding to the type of the variable
char pointerType(Symbol* symbol)
{
	int typeId;
	struct Type* type = NULL;
	
	if(symbol->symType == SYM_TYPE){
		// Symbol is a type
		type = (struct Type*)(symbol->info);		
	}else{ // symbol is a variable
		type = (struct Type*)(((struct Variable*)(symbol->info))->type->info);
	}
	
	typeId = type->id;
	
	if(typeId == TYPE_ARRAY){
		typeId = ((struct Type*)(type->arrayInfo->type->info))->id;
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


void genOperation(FILE* yyout, struct Symbol* leftSide, struct Symbol* rightSide, char* op )
{
	int r0, r1;
	r0 = ((struct ExtraInfo*)(leftSide->info))->nRegister;
	r1 = ((struct ExtraInfo*)(rightSide->info))->nRegister;
	if( !isFloat( leftSide ) ){
		if(r0 == 7){
			r0 = assignRegisters(0);
			((struct ExtraInfo*)(leftSide->info))->nRegister = r0;
			fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4;\n", r0/*pointerType(((struct ExtraInfo*)(leftSide->info))->variable)*/);
		}
		if(r1 == 7){
			r1 = assignRegisters(0);
			((struct ExtraInfo*)(leftSide->info))->nRegister = r1;
			fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4;\n", r1/*pointerType(((struct ExtraInfo*)(rightSide->info))->variable)*/);
		}
		fprintf(yyout, "\tR%d = R%d %s R%d;\n", r0, r0,op, r1);
		freeRegister(r1, 0);
	}else{
		// TODO: patxi, tu que estabas con el derramado. Decidi marcar los flotantes en pila con 4. xD
		if(r0 == 4){
			r0 = assignRegisters(1);
			((struct ExtraInfo*)(leftSide->info))->nRegister = r0;
			fprintf(yyout, "\tRR%d = I(R7);\n\tR7 = R7 + 4;\n", r0/*pointerType(((struct ExtraInfo*)(leftSide->info))->variable)*/);
		}
		if(r1 == 4){
			r1 = assignRegisters(1);
			((struct ExtraInfo*)(leftSide->info))->nRegister = r1;
			fprintf(yyout, "\tRR%d = I(R7);\n\tR7 = R7 + 4;\n", r1/*pointerType(((struct ExtraInfo*)(rightSide->info))->variable)*/);
		}
		fprintf(yyout, "\tRR%d = RR%d %s RR%d;\n", r0, r0,op, r1);
		freeRegister(r1, 1);
	}
	
	freeSymbol(rightSide);
}

char* genVariableInterpolation( FILE* yyout, Symbol* symbol )
{
	struct ExtraInfo* info = (struct ExtraInfo*)(symbol->info);
	int reg = info->nRegister;
	//int type = ((struct Type*)((struct Variable*)(info->variable->info))->type->info)->id;
	int type = getType( symbol );
	int isFloat = (pointerType(info->variable) == 'F');
	
	char* str = genNumericString( symbol );

	switch( type ){
			case TYPE_INTEGER:
				fprintf( yyout, "\tR7 = R7-4;\n" );
				fprintf( yyout, "\tI(R7) = R%d;\n", reg );
			break;
			case TYPE_CHAR:
				fprintf( yyout, "\tR7 = R7-1;\n" );
				fprintf( yyout, "\tU(R7) = R%d;\n", reg );
			break;
			case TYPE_FLOAT:
				fprintf( yyout, "\tR7 = R7-4;\n" );
				fprintf( yyout, "\tF(R7) = RR%d;\n", reg );
			break;
	}
	
	//fprintf( yyout, "\t// genVariableInterpolation [%s] - BEGIN\n", info->variable->name );

	//fprintf( yyout, "\tR7 = R7-4;\n" );
	//fprintf( yyout, "\tI(R7) = R%d;\n", reg );
	
	freeRegister( reg, isFloat );
	//fprintf( yyout, "\t// genVariableInterpolation - END\n" );
	freeSymbol( symbol );
	//freeSymbolInfo();
	//free( symbol );

	return str;
}

// If there are variables to be shown in the puts, the string has patterns 
// of the form "%<register><type>", where register indicates the index of 
// the register keeping the value to show, and type indicates the type of
// that value.
// ie. %2i -> load an integer from register R2
// ie. %3f -> load a float from register R3
/*
int genPutsValuesLoad( FILE* yyout, cstr str, int stringOffset  )
{
	int valueOffset = stringOffset + strlen( str ) + 1;
	int offset = 0;
	int i = 0;
	for( i=0; i<strlen( str ); i++ ){
		if( str[i] == 1 ){
			nValues++;
			switch( str[i+2] ){
				case 'I':
					fprintf( yyout, "\t%c(R7+%d) = R%d;\n", str[i+2], valueOffset, str[i+1]-'0' );
					valueOffset += 4;
				break;
				case 'U':
					fprintf( yyout, "\t%c(R7+%d) = R%d;\n", str[i+2], valueOffset, str[i+1]-'0' );
					valueOffset += 1;
				break;
				case 'F':
					fprintf( yyout, "\t%c(R7+%d) = RR%d;\n", str[i+2], valueOffset, str[i+1]-'0' );
					valueOffset += 4;
				break;
			}
		}
	}
	return valueOffset;
}
*/

void genPuts( FILE* yyout, cstr str )
{
	// argumentsSize = 4 (return label) 4 (n variables) + 1 (null character) + strlen(str).
	int argumentsSize = 9 + strlen( str );

	int nValues = 0;
	int i, j;
	
	// Print a comment to indicate the puts call's begin.
	fprintf( yyout, "\n\t/* Call to puts - begin [%s] (%d)*/\n", str, (int)strlen(str) );

	// Allocate memory for arguments
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments\n", argumentsSize );
	
	// Registers to show
	fprintf( yyout, "\t/* Call to puts - registers */\n" );
	//genPutsValuesLoad( yyout, str, 4 );

	// Arguments
	//int valueOffset = valuesOffset;

	fprintf( yyout, "\t/* Call to puts - string */\n" );

	//fprintf( yyout,"\tSTR( %d, \"%s\" );\n", R7+valuesOffset, str );
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
				nValues++;
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
	fprintf( yyout,"\tU(R7+%d) = '\\000';\n", 8+i );
	
	int newLabel_ = newLabel();

	// Save return label
	fprintf( yyout, "\tP(R7+4) = %i;\t// valueOffset\n", argumentsSize );

	// Save return label
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	// Call method
	fprintf( yyout, "\tGT(-13);\t// Call puts\n" );

	// Set return label
	fprintf( yyout, "L %i:\n", newLabel_ );

	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments\n", argumentsSize );
	
	// Print a comment to indicate the puts call's end.
	fprintf( yyout, "\t/* Call to puts - end */\n\n" );
}

char* genNumericString( Symbol* symbol )
{
	char *str = (char*)malloc( 3 );
	int reg, type;

	reg = ((struct ExtraInfo*)(symbol->info))->nRegister;
	/*
	type = ((struct Type*)(((struct Variable*)(((struct ExtraInfo*)(symbol->info))->variable->info))->type->info))->id;

	if(type == TYPE_ARRAY){
		type = ((struct Type*)(getArrayType( ((struct ExtraInfo*)(symbol->info))->variable )))->id;
	}
	*/
	type = getType( symbol );
	
	str[0] = '%';

	switch( type ){
		case TYPE_INTEGER:
		case TYPE_BOOLEAN:
			str[1] = 'I';
		break;
		case TYPE_FLOAT:
			str[1] = 'F';
		break;
		case TYPE_CHAR:
			str[1] = 'U';
		break;
		default:
			str[1] = 'E';
		break;
	}
	str[2] = 0;

	return str;
}


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

// FIXME: Terminar integracion con floats.
// Esto soporta los arrays?
int getType( Symbol* symbol )
{
	struct Symbol* symVariable = NULL;
	struct Type* type = NULL;

	if( symbol == NULL ){
		printf( "\n\ngetType(NULL)\n\n" );
	}

	printf( "\n\ngetType( %i )\n\n", symbol->symType );

	switch( symbol->symType ){
		case SYM_VARIABLE:
			type = ((struct Type*)((struct Variable*)(symbol->info))->type->info);
		break;
		case SYM_EXTRA_INFO:
			symVariable = ((struct ExtraInfo*)(symbol->info))->variable;
			return getType( symVariable );
			/*
			if( symVariable->symType == SYM_VARIABLE ){
				type = ((struct Type*)((struct Variable*)(symVariable->info))->type->info);
			}else{
				type = ((struct Type*)(symVariable->info));
			}
			*/
		break;
		case SYM_TYPE:
			type = ((struct Type*)(symbol->info));
		break;
		default:
			printf( "\n\ngetType ha devuelto -1!!!\n\n" );
			return -1;
		break;
	}

	if( type->id == TYPE_ARRAY ){
		type = ((struct Type*)(type->arrayInfo->type->info));
	}	

	if( type->id < 0 || type->id > 1 ){
		printf( "\n\ngetType ha devuelto algo diferente de 0 y 1 (%i)!!!\n\n", type->id );
	}
	return type->id;
}

/*
struct Type {
	int id;
	unsigned int size;
	union 
	{
		struct ArrayType* arrayInfo; // Used if id == TYPE_ARRAY;
		struct ClassType* classInfo;   // If id == TYPE_ARRAY, this points to first element.
   	};
};

struct ArrayType {
	Symbol* type;
	unsigned int nElements;
};
*/

int isFloat( Symbol* symbol )
{
	return ( getType( symbol ) == TYPE_FLOAT );
}
