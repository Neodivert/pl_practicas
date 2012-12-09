#include "codeGenUtils.h"

/*This file intends to keep the implementation of the functions and procedures
needed for the code generation part*/


int Registers[8] = {0,0,0,0,0,0,0,0};
int NRegs = 8;
int NEtiquetas = 0;
unsigned int topDirMem = Z;

/**************************************************************************/
/*Registers a new label and returns the identifier*/
/**************************************************************************/

int ne()
{
    NEtiquetas++;
    return NEtiquetas;
}



/**************************************************************************/
/*Returns an available register*/
/**************************************************************************/
/*0 -> entero, 1-> Flotante*/
int assig_regs(int type)
{
    int i=0;

	/*Buscar un Registro*/
    if ((type == 0) && (NRegs>0))
    {
        for (i=0;i<8;i++)
        {
            if (Registers[i]==0)
            {
                Registers[i] = 1;
                NRegs--;
                return i;
            }
        }
    }
    else if ((type == 1) && (NRegs>1))
    {
        for (i=0;i<8;i=i+2) //0, 2, 4, 6, -> 0 1 2 3
        {
            if (Registers[i]==0 && Registers[i+1] == 0)
            {
                Registers[i] = 1;

				Registers[i+1] = 1;
                NRegs = NRegs - 2;
                return (i/2);    //Devuelves el identificador no la posición en el vector
            }
        }
    }
   
    /*Si llegamos aquí es que no hay registros libres :(*/
    /*En caso de que no haya registros libres habrá que tirar de pila (A deliberar)*/
}


/**************************************************************************/
/*Frees all the registers*/
/**************************************************************************/
int free_regs()
{
    int i=0;
    for (i=0;i<8;i=i++){
        Registers[i]=0;
	}

	return 0;
}




/**************************************************************************/
/*Frees the i-th register*/
/**************************************************************************/
int free_reg(int i, int type)
{
    if (i < 0) return -1;
    if (i > 7) return -2;
    if ((type != 0) && (type != 1)) return -3;

    if ((type == 0) && (NRegs<8))
	{
        Registers[i]=0;
		NRegs--;
    }

	else if ((type == 1) && (NRegs<7))
	{

    	if ((i % 2) == 1) return -4;

    	Registers[i]=0;
        Registers[i+1]=0;

		NRegs = NRegs-2;
    }
    return 0;
}





























