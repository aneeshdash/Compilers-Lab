DIGIT [0-9]
NUMBER {DIGIT}+
TEXT [a-zA-Z ]+
TEXT_NUMBERS [a-zA-Z0-9]
KEYWORD "static"|"int"|"bool"|"break"|"return"|"print"|"read"|"if"|"else"|"while"
BOOLCONST "true"|"false"
DELIMITER [;{}]
WHITESPACE [; :\t\n]
IDENTIFIER [a-zA-Z]{TEXT_NUMBERS}*|[a-zA-Z]{TEXT_NUMBERS}*[[{NUMBER}+]]
ARITHMETIC_OP "+"|"-"|"*"|"/"|"="|"("|")"
RELATIONAL_OP ">"|"<"|">="|"<="|"=="|"!="
LOGICAL_OP "&"|"|"
FUNCTION {IDENTIFIER}+{WHITESPACE}*"("{WHITESPACE}*{TEXT}{TEXT_NUMBERS}*{WHITESPACE}*")"
%%

{NUMBER} { printf("<NUMCONST,%s>\n", yytext); }
{DELIMITER} { printf("<DELIMITER,%s>\n", yytext); }
{KEYWORD} { printf("<KEYWORD,%s>\n", yytext); }
{BOOLCONST} { printf("<BOOLCONST,%s>", yytext); }
{IDENTIFIER} { printf("<IDENTIFIER,%s>\n", yytext); }
{ARITHMETIC_OP} { printf("<OPERATOR,%s>\n", yytext); }
{LOGICAL_OP} { printf("<OPERATOR,%s>\n", yytext); }
{RELATIONAL_OP} { printf("<OPERATOR,%s>\n", yytext); }
{FUNCTION} { printf("<FUNCTION,%s>\n", yytext); }
%%

int main(int argc, char *argv[]) {
	yylex();
	return 0;
}
