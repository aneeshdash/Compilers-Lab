%{
#include "functions2.cpp"
#include "y.tab.h"
using namespace std;
extern "C" int yywrap() { }
int lineno = 1;
%}

%option yylineno

DIGIT [0-9]
NUMBER {DIGIT}+
TEXT [a-zA-Z ]+
TEXT_NUMBERS [a-zA-Z0-9]*
SCOPE_SPECIFIER "static"
TYPE_SPECIFIER "int"|"bool"
RETURN "return"
PRINT "print"
IF "if"
ELSE "else"
WHILE "while"
BREAK "break"
BOOLCONST "true"|"false"
READ "read"
WHITESPACE [; :\t\n]
IDENTIFIER [a-zA-Z]{TEXT_NUMBERS}|[a-zA-Z]{TEXT_NUMBERS}[[{NUMBER}+]]

%%

{NUMBER}            { yylval.Node = add_node_leaf("NUMBER", string(yytext)); return NUMCONST; }
{SCOPE_SPECIFIER}   { yylval.Node = add_node_leaf("STATIC", "static"); return SCOPE_SPECIFIER; }
"int"               { yylval.Node = add_node_leaf("TYPE_SPECIFIER", "int"); return TYPE_SPECIFIER; }
"bool"              { yylval.Node = add_node_leaf("TYPE_SPECIFIER", "bool"); return TYPE_SPECIFIER; }
{RETURN}            { yylval.Node = add_node_leaf("RETURN", "return"); return RETURN; }
{PRINT}             { yylval.Node = add_node_leaf("PRINT", "print"); return PRINT; }
{IF}                { yylval.Node = add_node_leaf("IF", "if"); return IF; }
{ELSE}              { yylval.Node = add_node_leaf("ELSE", "else"); return ELSE; }
{WHILE}             { yylval.Node = add_node_leaf("WHILE", "while"); return WHILE; }
{BREAK}             { yylval.Node = add_node_leaf("BREAK", "break"); return BREAK; }
"true"              { yylval.Node = add_node_leaf("BOOLCONST","true"); return BOOLCONST; }
"false"             { yylval.Node = add_node_leaf("BOOLCONST","false"); return BOOLCONST; }
{READ}              { yylval.Node = add_node_leaf("READ", "read"); return READ; }
{IDENTIFIER}        { yylval.Node = add_node_leaf("IDENTIFIER", string(yytext)); return IDENTIFIER; }
","					{ yylval.Node = add_node_leaf("COMMA", ","); return COMMA; }
";"                 { yylval.Node = add_node_leaf("SEMICOLON", ";"); return SEMICOLON; }
"{"                 { yylval.Node = add_node_leaf("OPENBRACES", "{"); return OPENBRACES; }
"}"                 { yylval.Node = add_node_leaf("CLOSEDBRACES", "}"); return CLOSEDBRACES; }
"("                 { yylval.Node = add_node_leaf("OPENPAREN", "("); return OPENPAREN; }
")"                 { yylval.Node = add_node_leaf("CLOSEDPAREN", ")"); return CLOSEDPAREN; }
"="                 { yylval.Node = add_node_leaf("ASSIGN", "="); return ASSIGN; }
"+"                 { yylval.Node = add_node_leaf("PLUS", "+"); return PLUS; }
"-"                 { yylval.Node = add_node_leaf("MINUS", "-"); return MINUS; }
"/"                 { yylval.Node = add_node_leaf("BY", "/"); return BY; }
"*"                 { yylval.Node = add_node_leaf("TIMES", "*"); return TIMES; }
">"                 { yylval.Node = add_node_leaf("GREATER", ">"); return GREATER; }
"<"                 { yylval.Node = add_node_leaf("LESS", "<"); return LESS; }
">="                { yylval.Node = add_node_leaf("GREQ", ">="); return GREQ; }
"<="                { yylval.Node = add_node_leaf("LEQ", "<="); return LEQ; }
"=="                { yylval.Node = add_node_leaf("EQUALS", "=="); return EQUALS; }
"!="                { yylval.Node = add_node_leaf("NEQ", "!="); return NEQ; }
"&"                 { yylval.Node = add_node_leaf("AND", "&"); return AND; }
"|"                 { yylval.Node = add_node_leaf("OR", "|"); return OR; }
"!"                 { yylval.Node = add_node_leaf("NOT", "!"); return NOT; }
"%"                 { yylval.Node = add_node_leaf("MOD", "%"); return MOD; }
[\n]                {yylineno++; lineno++;}

%%
