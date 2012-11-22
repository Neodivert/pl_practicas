#ifndef SEMANTIC_UTILITIES_H
#define SEMANTIC_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolsTable.h"
#include "sintax.tab.h"


struct Symbol* checkAritmeticExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkRelationalExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkLogicalExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkNotExpression(struct Symbol* s);

struct Symbol* checkIsBoolean(struct Symbol* s);

struct Symbol* checkSameType(struct Symbol* s1, struct Symbol* s2);

int checkMethodCall(struct Symbol *method, struct Symbol *type, int argument);

int checkMethodDefinition(const char* const name);

int checkArgumentDefinition(const char* const name);

int isVariable(struct Symbol *s);


// Check if a method call has the same number of arguments that the method
// definition.
//void checkMethodCall( const char* const name );

#endif
// SEMANTIC_UTILITIES_H
