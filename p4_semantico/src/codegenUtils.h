/*This file intends to keep the defeinitions of the procedures and functions that will used in
the code generation. Also the global variables and data structures*/

/*gc creates the code line that the code generation part passes*/
//gc(char *str);

/*Registers a new label and returns the identifier*/
int ne();

/*Returns an available register*/
int assig_regs(int type);

/*Frees all the registers*/
int free_regs();

/*Frees the i-th register*/
int free_reg(int i, int type);
