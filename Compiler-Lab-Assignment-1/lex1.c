#include "lex1.h"
#include <stdio.h>
#include <ctype.h>


char* yytext = ""; /* Lexeme (not '\0'
                      terminated)              */
int yyleng   = 0;  /* Lexeme length.           */
int yylineno = 0;  /* Input line number        */
extern FILE *fp;

int lex(void){

   static char input_buffer[1024];
   char        *current;

   current = yytext + yyleng; /* Skip current
                                 lexeme        */

   while(1){       /* Get the next one         */
      while(!*current ){
         /* Get new lines, skipping any leading
         * white space on the line,
         * until a nonblank line is found.
         */

         current = input_buffer;
         // if(!gets(input_buffer)){
         if(!fgets(input_buffer, 1000, fp )){
            *current = '\0' ;

            return EOI;
         }
         ++yylineno;
         while(isspace(*current))
            ++current;
      }
      for(; *current; ++current){
         /* Get the next token */
         yytext = current;
         yyleng = 1;
         switch( *current ){
           case ';':
            return SEMI;
           case '+':
            return PLUS;
           case '-':
            return MINUS;
           case '*':
            return TIMES;
           case '/':
            return DIV;
           case '(':
            return LP;
           case ')':
            return RP;
           case '>':
            return GT;
           case '<':
            return LT;
           case '=':
            return EQUALS;
           case ':':
            return COL;
            case '_':
            ++current;
            if(*current == 't')
            {
                yyleng = 3;
                return TEMPID;
            }
            else
            {
                while(isalnum(*current))
                  ++current;
               yyleng = current - yytext + 1;
                return LABELID;
            }
           case '\n':
           case '\t':
           case ' ' :
            break;
           default:
            if(!isalnum(*current))
               fprintf(stderr, "Not alphanumeric <%c>\n", *current);
            else{
               while(isalnum(*current))
                  ++current;
               yyleng = current - yytext;
               char subbuff[yyleng+1];
               memcpy( subbuff, yytext, yyleng );
               subbuff[yyleng] = '\0';
               if(strcmp(subbuff, "IfZ") == 0)
               {
                  return IFZ;
               }
               else if(strcmp(subbuff, "Goto") == 0)
               {
                  return GOTO;
               }
               return NUM_OR_ID;
            }
            break;
         }
      }
   }
}


static int Lookahead = -1; /* Lookahead token  */

int match(int token){
   /* Return true if "token" matches the
      current lookahead symbol.                */
      

   if(Lookahead == -1)
      Lookahead = lex();
   if(token == ID && Lookahead == NUM_OR_ID)
   {
       int i;
       char *current = yytext;
       for(i=0;i<yyleng;i++)
       {
           if(!isdigit(*current))
                return 1;
            current++;
       }
       return 0;
       
   }

   return token == Lookahead;
}

void advance(void){
/* Advance the lookahead to the next
   input symbol.                               */

    Lookahead = lex();
}
