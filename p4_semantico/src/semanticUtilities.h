#ifndef SEMANTIC_UTILITIES_H
#define SEMANTIC_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolsTable.h"
#include "sintax.tab.h"

struct MethodInfo
{
	struct Method *scope;
	int result;
};

struct Symbol* checkAritmeticExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkRelationalExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkLogicalExpression(struct Symbol* s1, struct Symbol* s2, char *op);

struct Symbol* checkNotExpression(struct Symbol* s);

struct Symbol* checkIsBoolean(struct Symbol* s);

struct SymbolInfo* checkIsInteger(struct Symbol* s);

struct Symbol* checkSameType(struct Symbol* s1, struct Symbol* s2);

struct Symbol* checkArray(Symbol* type, int n);

struct Symbol* checkAssignement(struct SymbolInfo* left_, struct Symbol *right);

int checkMethodCall(struct Symbol *method, struct Symbol *type, int argument);

struct MethodInfo *checkMethodDefinition(const char* const name);

int checkArgumentDefinition(const char* const name);

void setMethodReturnType(struct Symbol *method, struct Symbol *type);

struct Method * checkBlockDefinition(const char* const name, const char* const argName);

char *createBlockName(const char* const name, const char* const argName);

int isVariable(struct Symbol *s);

struct SymbolInfo* nullSymbolInfo();

struct SymbolInfo* checkArrayContent(struct Symbol* type, struct SymbolInfo* arrayInfo );

// Check if a method call has the same number of arguments that the method
// definition.
//void checkMethodCall( const char* const name );

#endif
// SEMANTIC_UTILITIES_H
