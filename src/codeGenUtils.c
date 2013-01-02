#include "codeGenUtils.h"

/*This file intends to keep the implementation of the functions and procedures
needed for the code generation part*/

//The last two values are R6 and R7, because they are used by the
//Q to track the variables and the stack, we do not allow the user
//to used them.
int registers[8] = {0,0,0,0,0,0,1,1};
int nRegisters = 6;
int nLabels = 0;
unsigned int topAddress = Z;

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
    if ((type == 0) && (nRegisters>0))
    {
        for (i=0;i<6;i++)
        {
            if (registers[i]==0)
            {
                registers[i] = 1;
                nRegisters--;
                return i;
            }
        }
    }
    else if ((type == 1) && (nRegisters>1))
    {
        for (i=0;i<8;i=i+2) //0, 2, 4, 6, -> 0 1 2 3
        {
            if (registers[i]==0 && registers[i+1] == 0)
            {
                registers[i] = 1;

				registers[i+1] = 1;
                nRegisters = nRegisters - 2;
                return (i/2);    //Devuelves el identificador no la posición en el vector
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
    for (i=0;i<3;i=i++){
        registers[i]=0;
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

    if ((type == 0) && (nRegisters<6)){
        registers[i]=0;
		nRegisters++;
    }
	else if ((type == 1) && (nRegisters<5))
	{

    	if ((i % 2) == 1) return -4;

    	registers[i]=0;
        registers[i+1]=0;

		nRegisters = nRegisters+2;
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
		fprintf( yyout, "\tR%d = %c(R7+%d); // Save return value\n", reg, 
			pointerType(method->returnType), totalSize);	
		totalSize += ((struct Type*)(method->returnType->info))->size;	
	}
	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments and return value\n", totalSize );

	// Print a comment to indicate the method call's end.
	fprintf( yyout, "\t/* Call to procedure - end */\n\n" );
}

// Generate the code for a parameter pass. Arguments:
// - iRegister - index of register with the argument's value.
// - method - called method symbol.
// - iArgument - argument index.
void genArgumentPass( FILE* yyout, int iRegister, Symbol* method, int iArgument )
{
	Symbol* argument = getMethodArgument( method, iArgument );

	int address = ((struct Variable*)( argument->info ) )->address;

	// Get parameter.
	fprintf( yyout,"\t%c(R7+%d) = R%d;\t// %iº Argument\n", pointerType( argument ), address, iRegister, iArgument+1 );
}


// Gets the Q type corresponding to the type of the variable
char pointerType(Symbol* symbol)
{

	int typeId;
	
	if(symbol->symType == SYM_TYPE){
		typeId = ((struct Type*)(symbol->info))->id;
	}else{ //It is a variable
		typeId = ((struct Type*)(((struct Variable*)(symbol->info))->type->info))->id;
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
	if(r0 == 7){
		r0 = assignRegisters(0);
		((struct ExtraInfo*)(leftSide->info))->nRegister = r0;
		fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4\n", r0/*pointerType(((struct ExtraInfo*)(leftSide->info))->variable)*/);
	}
	if(r1 == 7){
		r1 = assignRegisters(0);
		((struct ExtraInfo*)(leftSide->info))->nRegister = r1;
		fprintf(yyout, "\tR%d = I(R7);\n\tR7 = R7 + 4\n", r1/*pointerType(((struct ExtraInfo*)(rightSide->info))->variable)*/);
	}
	fprintf(yyout, "\tR%d = R%d %s R%d;\n", r0, r0,op, r1);
	freeRegister(r1, 0);
	freeSymbol(rightSide);	
	
}

// Entrada: R0=etiqueta de retorno
//          R1=dirección de la ristra de formato
//          R2=valor entero a visualizar (opcional según formato)
// No modifica ningún registro ni la ristra de formato


void genPuts( FILE* yyout, cstr str )
{
	// size = 4 (return label) + 4 (pointer to string) + 1 (null character) +
	// strlen(str).
	int i, size = 9 + strlen( str );

	// Print a comment to indicate the puts call's begin.
	fprintf( yyout, "\n\t/* Call to puts - begin */\n" );

	// Allocate memory for arguments
	fprintf( yyout,"\tR7 = R7 - %d;\t// Allocate memory for arguments\n", size );
	
	// Arguments
	fprintf( yyout,"\t/* string [%s]*/\n", str );
	for( i=0; i<strlen( str ); i++ ){
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
			default:
				fprintf( yyout,"\tU(R7+%d) = '%c';\n", 8+i, str[i] );
			break;
		}
	}
	fprintf( yyout,"\tU(R7+%d) = '\\000';\n", 8+i );
	
	int newLabel_ = newLabel();

	// Pointer to string
	// FIXME: esta orden esta puesta porque P(R7+4) = R7+8; no me lo pillaba la 
	// maquina Q. Como ven, se usa el R4 por la cara.
	fprintf( yyout, "\tR4 = R7 + 8;\n" );
	fprintf( yyout, "\tP(R7+4) = R4; //Pointer to string\n" ); //+8

	// Save return label
	fprintf( yyout, "\tP(R7) = %i;\t// Save return label\n", newLabel_ );

	// Call method
	fprintf( yyout, "\tGT(-13);\t// Call puts\n" );

	// Set return label
	fprintf( yyout, "L %i:\n", newLabel_ );

	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments\n", size );
	
	// Print a comment to indicate the puts call's end.
	fprintf( yyout, "\t/* Call to puts - end */\n\n" );
}

/*
#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3
#define TYPE_CHAR 4
#define TYPE_BOOLEAN 5
#define TYPE_CLASS 6
#define TYPE_ARRAY 7
*/

char* genNumericString( Symbol* symbol )
{
	char *str = (char*)malloc( 4 );
	int reg, type;

	printf( "Generando string numerica - BEGIN\n" );

	reg = ((struct ExtraInfo*)(symbol->info))->nRegister;
	type = ((struct Type*)(((struct Variable*)(((struct ExtraInfo*)(symbol->info))->variable->info))->type->info))->id;

	str[0] = '@';
	str[1] = '0' + (char)reg;

	switch( type ){
		case TYPE_INTEGER:
		case TYPE_BOOLEAN:
			str[2] = 'i';
		break;
		case TYPE_FLOAT:
			str[2] = 'f';
		break;
		case TYPE_CHAR:
			str[2] = 'c';
		break;
		default:
			str[2] = 'E';
		break;
	}

	str[3] = 0;
	/*
	printf( "Symbol name: [%s]\n", symbol->name );
	printf( "Symbol type: [%i]\n", symbol->symType );
	printf( "Symbol register: [%i]\n", reg );
	printf( "Symbol var: [%p]\n", ((struct ExtraInfo*)(symbol->info))->variable );
	printf( "Symbol var type name: [%s]\n", ((struct Variable*)(((struct ExtraInfo*)(symbol->info))->variable->info))->type->name );
	printf( "Symbol var type id: [%i]\n", ((struct Type*)(((struct Variable*)(((struct ExtraInfo*)(symbol->info))->variable->info))->type->info))->id );
	*/

	printf( "Generando string numerica - END\n" );

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
		reg = 0;
		fprintf( yyout, "\tRR%d = %c(R7+5);\t// Save returned value\n", reg, inputType );
	}

	// Free arguments memory
	fprintf( yyout,"\tR7 = R7 + %d;\t// Free memory for arguments\n", size );
	
	// Print a comment to indicate the puts call's end.
	fprintf( yyout, "\t/* Call to get (%c) - end */\n\n", inputType );
}
