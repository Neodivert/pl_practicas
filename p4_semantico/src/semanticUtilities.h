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

struct Symbol* getCreateVariable( int symType, const char* const name);

#endif
// SEMANTIC_UTILITIES_H
