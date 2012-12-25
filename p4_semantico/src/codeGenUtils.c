#include "codeGenUtils.h"

/*This file intends to keep the implementation of the functions and procedures
needed for the code generation part*/


int registers[8] = {0,0,0,0,0,0,0,0};
int nRegisters = 8;
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
        for (i=0;i<8;i++)
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
    for (i=0;i<8;i=i++){
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

    if ((type == 0) && (nRegisters<8))
	{
        registers[i]=0;
		nRegisters++;
    }

	else if ((type == 1) && (nRegisters<7))
	{

    	if ((i % 2) == 1) return -4;

    	registers[i]=0;
        registers[i+1]=0;

		nRegisters = nRegisters-2;
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


/**************************************************************************/
/*Returns the actual address for storage in memory*/
/*Also asigns it to the corresponding field in the symbols table*/
/**************************************************************************/

unsigned int returnAddress(int symbolType,cstr id)
{
	Symbol* variable = searchVariable(symbolType, id);
	return ((struct Variable*)(variable->info))->address;
}


/*                               Methods                                      */

// Generate the code for a method "begin" (method label and local space 
// allocation).
void genMethodBegin( FILE* yyout, cstr methodName )
{
   // Get the method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );
   
   // Print the method name in a comment.
	fprintf( yyout, "\n\t\\\\ Procedure [%s]\n", methodName );
   
	// Set method label. 
	fprintf( yyout,"\tL %i:\n", method->label );

	// New base.
	fprintf( yyout,"t\tR6 = R7;\t\\\\ New base\n", method->label );

	// Allocate space for local variables.
	fprintf( yyout,"\t\tR7 = R7 - %d;\t\\\\ Allocate space for local variables\n", method->localsSize );
}


// Generate the code for a method "end" (local data free and return).
void genMethodEnd( FILE* yyout, cstr methodName )
{
	// Get the method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );

	// Free local memory.
	fprintf( yyout,"\t\tR7 = R7 + %d;\t\\\\ Free local variables\n", method->localsSize );

	// Retrieve previous base.
	fprintf( yyout,"\t\tR6 = P(R7+4);\t\\\\ Retrieve base\n", method->localsSize );

	// Return from method.
	fprintf( yyout,"\t\tR5 = P(R7);\t\\\\ Get return label\n", method->localsSize );
	fprintf( yyout,"\t\tGT(R5);\t\\\\ Return to caller\n", method->localsSize );
}


// Generate the code for a method call "begin" (parameters memory allocation).
void genMethodCallBegin( FILE* yyout, cstr methodName )
{
	// Get the called method's info from symbols' table.
   struct Method* method = (struct Method *)( searchTopLevel( SYM_METHOD, methodName )->info );

	// Print the called method name in a comment.
	fprintf( yyout, "\t\\\\ Starting call to procedure [%s]\n", methodName );
	
	// Allocate memory for parameters (and 8 bytes more for R6 and return label).
	fprintf( yyout,"\tR7 = R7 - %d;\t\\\\ Allocate memory for parameters\n", (method->argumentsSize+8) );
}


// Generate the code for a parameter pass. Arguments:
// - vRegister - index of register with the parameter's value.
// - offset - parameter position in stack.
void genParameterPass( FILE* yyout, int vRegister, int offset )
{
	// Get parameter.
	fprintf( yyout,"\tI(R7 + %d) = R%d;\t\\\\ Parameter\n", (8+offset), vRegister );
}


// Gets the Q type corresponding to the type of the variable
char pointerType(Symbol* symbol)
{
	switch(((struct Type*)(((struct Variable*)(symbol->info))->type->info))->id)
	{
	case (TYPE_INTEGER || TYPE_BOOLEAN):
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
	fprintf(yyout, "\tR%d = R%d %s R%d;\n", r0, r0,op, r1);
	freeRegister(r1, 0);
	freeSymbol(rightSide);	
}
