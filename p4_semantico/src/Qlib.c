// Qlib.c 3.7.3     BIBLIOTECA DE Q

// Se permite su modificacion, en cuyo caso ha de recompilarse IQ (ver Qman.txt)

// Conservar la siguiente línea
#include "Q.h"


// Definiciones auxiliares (rutinas, variables, ...)

/* inv_str() permite invertir el orden de las strings en Q para       
acomodarlo al de las rutinas de la biblioteca de C, y reinv_str()     
realiza la operación inversa, para lo cual precisa de los punteros de 
comienzo y final */                                                   

void reinv_str(unsigned char *p, unsigned char *r) {             
  while (p<r) {
    unsigned char t=*p;
    *p++=(unsigned char)*r;
    *r--=(unsigned char)t; 
  }
}                                                                     
                                                                      
unsigned char *inv_str(unsigned char *r) {
  unsigned char *p=(unsigned char *)r;                    
  while (*p) p--;    // va al '\0'
  reinv_str(p,r);    // invierte                                    
  return p;                                                           
}                                                                     


/* Rutinas de biblioteca de Q
*****************************

Utilizables tanto para código Q compilado como interpretado.
Segmentos de código Q sin restricciones (podemos usar construcciones
C) que implementan las distintas rutinas.  No tienen por qué seguir
el mismo esquema de paso de argumentos que nuestro código generado.
Se apoyan en las anteriores (y extendibles) definiciones
auxiliares.

Manténgase el esquema BEGINLIB {etiqueta: codigo-C} ENDLIB

Opcionalmente, defínanse macros para etiquetas en Qlib.h
*/
 

BEGINLIB

// void exit(int)                    
// Entrada: R0=código de salida
// No retorna
L exit_: exit(R0);  // termina el programa con código en R0

// void* new(int_size)           
// Entrada: R0=etiqueta de retorno
//          R1=tamaño (>=0)
// Salida: R0=puntero al tramo de memoria asignado 
// Sólo modifica R0                            
L new_: {//entre llaves por usar variable local
         int r=R0;
         IF(R1<0) GT(exit_);         // no permite tamaños negativos
         NH(R1);                     // reserva tramo de memoria en heap   
         R0=HP;                      // devuelve dirección más baja del tramo 
         GT(r);                      // retorna                  
        }                            

// void putf(const unsigned char*, int)
// Entrada: R0=etiqueta de retorno
//          R1=dirección de la ristra de formato
//          R2=valor entero a visualizar (opcional según formato)
// No modifica ningún registro ni la ristra de formato
L putf_: {unsigned char *p=inv_str(&U(R1)); // invierte: nva. dir. real 1er char
	  printf((char*)p,R2);             // traslada                 
          reinv_str(p,&U(R1));   	    // re-invierte                
	  GT(R0);                           // retorna
	}      

L puts_: {
	char *p=inv_str(&U(P(R7+4)));
	int i = 0;
	while( i < strlen(p) ){
		if( p[i] != '@' ){
			putchar( p[i] );
			i++;
		}else{
			switch( p[i+2] ){
				printf(p,I(R7+4));
				case 'i':
					printf( "%i", R[p[i+1]-(int)('0')] );
				break;
				case 'f':
					printf( "%f", R[p[i+1]-(int)('0')] );
				break;
				case 'c':
					printf( "%c", R[p[i+1]-(int)('0')] );
				break;
				default:
					printf( "E" );
				break;
			}
			i += 3;
		}
	}
	//printf((char*)p);
	reinv_str(p,&U(P(R7+4)));
	GT(P(R7));
}      

L get_: {
	char input[30];
	char inputType = U(R7+4);

	fgets( input, 29, stdin );

	printf( "get de tipo [%c]\n", inputType );
	switch( inputType ){
		case 'c':
			I(R7+5) = input[0];
		break;
		case 'i':
			I(R7+5) = atoi( input );
		break;
		case 'f':
			I(R7+5) = atof( input );
		break;
		default:
			printf( "ERROR: undefined type in input (%c)\n", inputType );
		break;
	}

	GT(P(R7));
}

ENDLIB
