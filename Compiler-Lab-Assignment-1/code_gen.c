#include <stdio.h>
#include "lex.h"
#include "name.c"
#include "lex.c"

char    *factor     ( void );
char    *term       ( void );
char    *expression ( void );
char    *relation   ( void );

extern char *newname( void       );
extern void freename( char *name );
extern int loop_count;
extern FILE *fp;

statements()
{
    /* stmt -> id := expr | if expr then stmt | while expr do stmt | begin opt_stmt   */
    while( !match(EOI) )
    {
        statement();
    }
}

statement()
{
    /*  statements -> expression SEMI  |  expression SEMI statements  */

    int loop_local, id_len;
    char *tempvar, *id;

    if( match (IF) )
    {
        
        advance();
        tempvar = relation();
        fprintf(fp, "   IfZ %s Goto _L%d\n",tempvar,loop_count);
        freename(tempvar);
        loop_local = loop_count;
        loop_count++;
        if( match(THEN) )
        {
            advance();
            statement();
            
            fprintf(fp, "_L%d:\n",loop_local);
        }
        else
        {
            fprintf(stderr, "%d: No THEN block after IF\n", yylineno );
        }
    }
    
    else if( match(WHILE) )
    {
        advance();
        fprintf(fp, " _L%d:\n",loop_count);
        loop_local = loop_count;
        loop_count++;
        tempvar = relation();
        fprintf(fp, "    IfZ %s Goto _L%d\n",tempvar,loop_count);
        if( match(DO) )
        {
            advance();
            statement();
            fprintf(fp, "    Goto _L%d\n",loop_local);
            fprintf(fp, "_L%d:\n",loop_count);
        }
        else
        {
            fprintf(stderr, "%d: No DO block after WHILE\n", yylineno);   
        }
    }
    
    else if ( match (BEGIN) )
    {
        advance();
        while (!match(END) && !match (EOF) )
            statement();
        if ( match (EOF) )
            fprintf(stderr, "%d: No END statement after BEGIN\n", yylineno );
        else
            advance();
        return 0;
    }
    
    else if( match(ID) )
    {
        id = yytext;
        id_len = yyleng;
        advance();
        if( match(COL) )
        {
            advance();
            if ( match(EQUALS) ) {
                advance();
                tempvar = relation();
                fprintf(fp, "    %0.*s = %s\n", id_len, id, tempvar);
            }
            else
                fprintf(stderr, "%d: no equal to after : in assignment operator\n", yylineno );
        }
        else {
            yytext = id;
            yyleng = 0;
            advance();
            tempvar = relation();
        }
        if( match( SEMI ) )
                advance();
        else
            fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno );
        freename( tempvar );
    }
    else {
        tempvar = relation();
        
        if( match( SEMI ) )
                advance();
            else
                fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno );
    
            freename( tempvar );
    }
}

char *relation()
{
    char *tempvar, *tempvar2;
    tempvar = expression();
    if ( match(LT) || match(GT) || match(EQUALS) )
    {
        char symbol = *yytext;
        advance();
        // fprintf(fp, "%c %d\n",symbol, symbol == EQUALS);
        if ( match(EQUALS) )
        {
            char symbol2 = *yytext;
            advance();
            tempvar2 = relation();
            fprintf(fp, "    %s = %s %c%c %s\n", tempvar, tempvar, symbol, symbol2, tempvar2 );
        }
        else {
            if ( symbol == '=' )
                fprintf(fp, stderr, "%s\n", "Missing = for comparision operator");
            tempvar2 = relation();
            fprintf(fp, "    %s = %s %c %s\n", tempvar, tempvar, symbol, tempvar2 );
        }
        freename( tempvar2 );   
    }
    return tempvar;
}

char    *expression()
{
    /* expression -> term expression'
     * expression' -> PLUS term expression' |  epsilon
     */

    char  *tempvar, *tempvar2;

    tempvar = term();
    while( match( PLUS ) || match( MINUS ) )
    {
        char symbol = *yytext;
        advance();
        tempvar2 = term();
        fprintf(fp, "    %s %c= %s\n", tempvar, symbol, tempvar2 );
        freename( tempvar2 );
    }

    return tempvar;
}

char    *term()
{
    char  *tempvar, *tempvar2 ;

    tempvar = factor();
    while( match( TIMES ) || match (DIV) )
    {
        char symbol = *yytext;
        advance();
        tempvar2 = factor();
        fprintf(fp, "    %s %c= %s\n", tempvar, symbol, tempvar2 );
        freename( tempvar2 );
    }

    return tempvar;
}

char    *factor()
{
    char *tempvar;

    if( match(NUM_OR_ID) )
    {
	/* Print the assignment instruction. The %0.*s conversion is a form of
	 * %X.Ys, where X is the field width and Y is the maximum number of
	 * characters that will be printed (even if the string is longer). I'm
	 * using the %0.*s to print the string because it's not \0 terminated.
	 * The field has a default width of 0, but it will grow the size needed
	 * to print the string. The ".*" tells fprintf(fp, ) to take the maximum-
	 * number-of-characters count from the next argument (yyleng).
	 */

        fprintf(fp, "    %s = %0.*s\n", tempvar = newname(), yyleng, yytext );
        advance();
    }
    else if( match(LP) )
    {
        advance();
        tempvar = relation();
        if( match(RP) )
            advance();
        else
            fprintf(fp, stderr, "%d: Mismatched parenthesis\n", yylineno );
    }
    else
	fprintf(stderr, "%d: Number or identifier expected\n", yylineno );

    return tempvar;
}
