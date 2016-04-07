%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>
    #include <fstream>
    #include <string>
    #include "functions.cpp"
    using namespace std;
    int yylex(void);
    void yyerror(char *s)
    {

        fprintf(stderr,"Unknown errors detected.\n");
        extern int yylineno;
    }
    int flag = 0;


    extern int  yylex();
    extern int yylineno;
    extern int lineno;
%}

%union{
	node *Node;
}

%token <Node>   NUMCONST SCOPE_SPECIFIER TYPE_SPECIFIER RETURN PRINT IF ELSE WHILE BREAK BOOLCONST READ IDENTIFIER COMMA
%token <Node>   SEMICOLON OPENBRACES OPENPAREN CLOSEDBRACES CLOSEDPAREN
%token <Node>   MINUS PLUS BY TIMES MOD NOT
%token <Node>   GREQ LEQ LESS GREATER NEQ EQUALS AND OR ASSIGN
%start   program
%expect 6

%type <Node> program declaration_list declaration empty scoped_type_specifier scoped_variable_dec variable_dec variable_dec_list function_dec parameters parameter_list parameter_type_list statement print_stmt compound_stmt stmt_list expression_stmt conditional_stmt iteration_stmt return_stmt break_stmt expression expr_suffix read_expr simple_expr and_expr unary_rel_expr rel_expr sum_expr sumop term mulop factor call args arg_list constant openparen closedparen closedbraces comma semicolon id

%%

program : declaration_list
{
     $$ = add_node("program",$1);

    if(flag==0)
    {
        printf("Compilation successful.\n");
        ofstream output;
	    output.open("tree.txt");
        print_nodes($$,output);
        output.close();
    }
    else
    {
        printf("Syntax errors found.\n");
    }

}

        ;
empty :
{
    $$ = NULL;
}

;

declaration_list : declaration_list declaration
{
    $$ = add_node("declaration_list",$1,$2);
}
                 | declaration
{
    $$ = add_node("declaration_list",$1);
}
                 ;

declaration : scoped_variable_dec
{
   $$ = add_node("declaration",$1);
}
            | function_dec
{
   $$ = add_node("declaration",$1);
}
            | variable_dec
{
   $$ = add_node("declaration",$1);
}
            | error {printf("Error: Missing Type Specifier at line %d\n", lineno); flag = 1;$$ = NULL;}
            ;


scoped_type_specifier   : SCOPE_SPECIFIER TYPE_SPECIFIER
{
    $$ = add_node("scoped_type_specifier",$1,$2);
}
                        | SCOPE_SPECIFIER error {printf("Error: Missing Type Specifier at line %d\n", lineno); flag = 1;$$ = NULL;}

;

scoped_variable_dec     : scoped_type_specifier variable_dec_list semicolon
{
    $$ = add_node("scoped_variable_dec",$1,$2,$3);
};

variable_dec            : TYPE_SPECIFIER variable_dec_list semicolon
{
    $$ = add_node("variable_dec",$1,$2,$3);
}
;


variable_dec_list       : variable_dec_list comma id
{
    $$ = add_node("variable_dec_list",$1,$2,$3);
}
                        | id

{
    $$ = add_node("variable_dec_list",$1);
}
;


function_dec            : TYPE_SPECIFIER IDENTIFIER OPENPAREN parameters closedparen compound_stmt
{
    $$ = add_node("function_dec",$1,$2,$3,$4,$5,$6);
}


;


parameters              : parameter_list
{
    $$ = add_node("parameters",$1);
}
| empty
{
    $$ = add_node("parameters",$1);
}
;

parameter_list          : parameter_list comma parameter_type_list
{
    $$ = add_node("parameter_list",$1,$2,$3);
}
                        | parameter_type_list
{
    $$ = add_node("parameter_list",$1);
}
;

parameter_type_list     : TYPE_SPECIFIER IDENTIFIER
{
    $$ = add_node("parameter_type_list",$1,$2);
}
                        | IDENTIFIER {printf("Error: Missing Type Specifier at line %d\n", lineno); flag = 1; $$ = NULL;};

;




statement               : print_stmt
{
    $$ = add_node("statement",$1);
}
                        | expression_stmt
{
    $$ = add_node("statement",$1);
}
                        | compound_stmt
{
    $$ = add_node("statement",$1);
}

                        | conditional_stmt
{
    $$ = add_node("statement",$1);
}
                        | iteration_stmt
{
    $$ = add_node("statement",$1);
}
                        | return_stmt
{
    $$ = add_node("statement",$1);
}
                        | break_stmt
{
    $$ = add_node("statement",$1);
}
                        | scoped_variable_dec
{
    $$ = add_node("statement",$1);
}
                        | variable_dec
{
    $$ = add_node("statement",$1);
}
                        ;

print_stmt              : PRINT  expression semicolon
{
    $$ = add_node("print_stmt",$1,$2,$3);
}
;

compound_stmt           : OPENBRACES stmt_list closedbraces
{
    $$ = add_node("compound_stmt",$1,$2,$3);
}
;

stmt_list               : stmt_list statement
{
    $$ = add_node("stmt_list",$1, $2);
}
                        | empty
{
    $$ = add_node("stmt_list",$1);
}
                        ;

expression_stmt         : expression semicolon
{
    $$ = add_node("expression_stmt",$1,$2);
}
                         ;

conditional_stmt        : IF  openparen expression closedparen statement
{
    $$ = add_node("conditional_stmt",$1,$2,$3,$4,$5);
}

                        | IF openparen expression closedparen statement ELSE statement
{
    $$ = add_node("conditional_stmt",$1,$2,$3,$4,$5,$6,$7);
}
                        ;

iteration_stmt          : WHILE openparen expression closedparen statement
{
    $$ = add_node("iteration_stmt",$1,$2,$3,$4,$5);
}
;

return_stmt             : RETURN  semicolon
{
    $$ = add_node("return_stmt",$1,$2);
}
                        | RETURN  expression semicolon
{
    $$ = add_node("return_stmt",$1,$2,$3);
};

break_stmt              : BREAK semicolon
{
    $$ = add_node("break_stmt",$1,$2);
}
;
expression              : read_expr
{
    $$ = add_node("expression",$1);
}
                        | IDENTIFIER ASSIGN simple_expr
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | IDENTIFIER PLUS expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | IDENTIFIER MINUS expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | IDENTIFIER TIMES expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | IDENTIFIER BY expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | IDENTIFIER MOD expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
}
                        | simple_expr
{
    $$ = add_node("expression",$1);
}
                        ;

expr_suffix             : simple_expr
{
    $$ = add_node("expr_suffix",$1);
}
                        | ASSIGN simple_expr
{
    $$ = add_node("expr_suffix",$1,$2);
}
                        ;

read_expr               : READ IDENTIFIER
{
    $$ = add_node("read_expr",$1,$2);
}

;


simple_expr             : OPENPAREN simple_expr OR and_expr closedparen
{
    $$ = add_node("simple_expr",$1,$2,$3,$4,$5);
}
                        | and_expr
{
    $$ = add_node("simple_expr",$1);
}
                        ;

and_expr                : and_expr AND unary_rel_expr
{
    $$ = add_node("and_expr",$1,$2,$3);
}
                        | unary_rel_expr
{
    $$ = add_node("and_expr",$1);
}
                        ;

unary_rel_expr          : NOT unary_rel_expr
{
    $$ = add_node("unary_rel_expr",$1,$2);
}
                        | rel_expr
{
    $$ = add_node("unary_rel_expr",$1);
}
                        ;

rel_expr                : sum_expr GREATER sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr LESS sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr EQUALS sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr GREQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr LEQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr NEQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
}
                        | sum_expr
{
    $$ = add_node("rel_expr",$1);
}
                        ;

sum_expr                : sum_expr sumop term
{
    $$ = add_node("sum_expr",$1,$2,$3);
}
                        | term
{
    $$ = add_node("sum_expr",$1);
}
                        ;

sumop                   : PLUS
{
    $$ = add_node("sumop",$1);
}
                        | MINUS
{
    $$ = add_node("sumop",$1);
}
                        ;

term                    : term mulop factor
{
    $$ = add_node("term",$1,$2,$3);
}
                        | factor
{
    $$ = add_node("term",$1);
}
                        ;

mulop                   : TIMES
{
    $$ = add_node("mulop",$1);
}

                        | BY
{
    $$ = add_node("mulop",$1);
}
                        | MOD
{
    $$ = add_node("mulop",$1);
}
;

factor                  : IDENTIFIER
{
    $$ = add_node("factor",$1);
}
                        | OPENPAREN expression closedparen
{
    $$ = add_node("factor",$1,$2,$3);
}
                        |  call
{
    $$ = add_node("factor",$1);
}
                        | constant
{
    $$ = add_node("factor",$1);
}
                        ;

call                    : IDENTIFIER OPENPAREN args closedparen
{
    $$ = add_node("call",$1,$2,$3,$4);
}
;

args                    : arg_list
{
    $$ = add_node("args",$1);
}

                        | empty
{
    $$ = add_node("args",$1);
}
                        ;

arg_list                : arg_list comma expression
{
    $$ = add_node("arg_list",$1,$2,$3);
}
                        | expression
{
    $$ = add_node("arg_list",$1);
}
                        ;

constant                : NUMCONST
{
    $$ = add_node("constant",$1);
}
                        | BOOLCONST
{
    $$ = add_node("constant",$1);
}
                        ;

openparen               : OPENPAREN

{
    $$ = add_node("openparen",$1);
}
                        | error
{printf("Error: '(' expected after IF or WHILE at line %d\n", lineno);
flag = 1;
$$ = NULL;
};


closedparen             : CLOSEDPAREN

{
    $$ = add_node("closedparen",$1);
}
                        | error {printf("Error: Missing ')' at line %d\n", lineno); flag = 1; $$ = NULL;};


closedbraces            : CLOSEDBRACES
{
    $$ = add_node("closedbraces",$1);
}
                        | error {printf("Error: Missing '}' at line %d\n", lineno); flag = 1; $$ = NULL;};

comma                   : COMMA
{
    $$ = add_node("comma",$1);
}
                        ;

semicolon               : SEMICOLON
{
    $$ = add_node("semicolon",$1);
}
                        | error {printf("Error: Missing ';' or ',' near line %d\n", lineno); flag = 1; $$ = NULL;};

id                      : IDENTIFIER
{
    $$ = add_node("id",$1);
}
                        | comma {printf("Error: Extra ',' at line %d\n", lineno); flag = 1;}
                        | error {printf("Error: Missing identifier name at line %d\n", lineno); flag = 1; $$ = NULL;};

%%
int main(){
    //yydebug = 1;
    yyparse();
    return 0 ;
}
