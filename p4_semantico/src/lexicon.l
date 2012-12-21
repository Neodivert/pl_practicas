%{
#include "sintax.tab.h"

int numlin = 1;
void error(char*);

int compilationState;
int arraySize = 0;
float floatVal;

%}
capital [A-Z]
letter [a-zA-Z]
digit [0-9]
letdig {letter}|{digit}
%x COMPLEX_STRING
%x IDENT

%%
def                            {return DEF;}
end                            {return END;}
if                            {return IF;}
then                            {return THEN;}
else                            {return ELSE;}
while                            {return WHILE;}
do                            {return DO;}
class                            {return CLASS;}
and								{return AND;}
or								{return OR;}
not 							{return NOT;}

nil                             {return NIL;}
false						{arraySize = 0; return BOOL;}
true                    	{arraySize = 1; return BOOL;}
".each"						{return EACH;}
".new"						{return NEW;}
Array						{return ARRAY;}


$({letter}|_)({letdig}|_)*                     {strcpy(yylval.string, yytext); return ID_GLOBAL_VARIABLE;}
@({letter}|_)({letdig}|_)*                     {strcpy(yylval.string, yytext); return ID_INSTANCE_VARIABLE;}
{capital}({letdig}|_)*                               {strcpy(yylval.string, yytext); return ID_CONSTANT;}
("+"|"-")?{digit}+                            { arraySize=atoi(yytext); return INTEGER;}
({digit}+"."{digit}+|"."{digit}+|{digit}+".")((e|E){digit}+)?    {floatVal=atof(yytext); return FLOAT;}
"!="                                                     {return NOT_EQUAL;}
"=="                                                      {return EQUAL_EQUAL;}
">="                                                    {return GREATER_EQUAL;}
"<="                                                      {return LESS_EQUAL;}

\"                  {BEGIN(COMPLEX_STRING); return BEGIN_COMPLEX_STRING;}

<COMPLEX_STRING>\\t|\\\\|\\n|\\\"         {return SEC_SCAPE;}
<COMPLEX_STRING>\"          {BEGIN(INITIAL); return END_COMPLEX_STRING;}
<COMPLEX_STRING>"#{"         {BEGIN(IDENT); return START_STRUCT;}
<COMPLEX_STRING>\n         {numlin++; BEGIN(INITIAL); error("Unfinished_string");}
<COMPLEX_STRING>"#"|"{"         {return SUBSTRING;}
<COMPLEX_STRING>[^"\n#{\\]*         {strcpy(yylval.string, yytext); return SUBSTRING;}
<COMPLEX_STRING>. {BEGIN(COMPLEX_STRING); error("Weird_character_on_string");}

<IDENT>$({letter}|_)({letdig}|_)*         {strcpy(yylval.string, yytext); return ID_GLOBAL_VARIABLE;}
<IDENT>{capital}({capital}|_)*            {strcpy(yylval.string, yytext); return ID_CONSTANT;}
<IDENT>({letter}|_)({letdig}|_)*              {strcpy(yylval.string, yytext); return IDENTIF;}
<IDENT>"}"         {BEGIN(COMPLEX_STRING); return END_STRUCT;}
<IDENT>"."         			{return yytext[0];}
<IDENT>\" 			{BEGIN(INITIAL); error("Unfinished_interpolation_in_string");}
<IDENT>\n 			{numlin++; BEGIN(INITIAL); error("Unfinished_string");}
<IDENT>. 			{BEGIN(COMPLEX_STRING); error("Only_one_variable_is_allowed_on_interpolation");}

"="|"|"|"<"|">"|"-"|"+"|"*"|"/"|"("|")"|"["|"{"|"}"|"]"|"."|","|";"  {return yytext[0];}

'\\'				{error("Characters_should_follow_the_pattern_'c'_or_'\\c'");}
'\\t'|'\\\\'|'\\n'|'\\\'' 	{return CHAR;}
'.'            		{ arraySize = yytext[1]; return CHAR;}
'.[^']*'				{error("Characters_should_follow_the_pattern_'c'_or_'\\c'");}


({letter}|_)({letdig}|_)*                      {strcpy(yylval.string, yytext); return IDENTIF;}

[\r\t ] |      
#.*            {/* ignores de line */}

\n                                         {numlin++; return END_LINE;}

'            {error("Unpaired_'");}
.            {error("Weird_character");}
<<EOF>>        {return 0; /* by default */}


%%

void error(char* mens) {
	if(compilationState){
		printf("Lexical_failure_in_line_%i:_%s_caracter_=_%c.\n",numlin,mens, yytext[0]);
	}	
}

