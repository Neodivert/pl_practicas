#include "Q.h"

BEGIN
STAT(0)
	MEM(0x11ffc,4); //Memory for var $b 
	MEM(0x11ff8,4); //Memory for var $a 
CODE(0)
L 0:
	R0 = 5; //Loading integer 5
	I(0x11ffc) = R0; //$b = expr
	R0 = 1; //Loading integer 1
	R1 = 2; //Loading integer 2
	R2 = I(0x11ffc); //Loading value of var $b
	R1 = R1 + R2;
	R0 = R0 + R1;
	I(0x11ff8) = R0; //$a = expr
	GT(-2);
END
