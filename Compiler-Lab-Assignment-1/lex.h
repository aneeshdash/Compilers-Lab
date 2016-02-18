#define EOI		0	/* End of input			*/
#define SEMI		1	/* ; 				*/
#define PLUS 		2	/* + 				*/
#define TIMES		3	/* * 				*/
#define LP		    4	/* (				*/
#define RP		    5	/* )				*/

#define NUM_OR_ID	6	/* Decimal Number or Identifier */
#define GT          7   /* >                */
#define LT          8   /* <                */
#define MINUS       9   /* -                */
#define DIV        10   /* /                */
#define EQUALS     11   /* =                */
#define IF         12   /* if               */
#define THEN       13   /* then             */
#define WHILE      14   /* WHILE            */
#define DO         15   /* do               */
#define BEGIN      16   /* begin            */
#define END        17   /* end              */
#define ID         18   /* identifier       */ 
#define COL         19   /* : colon       */ 
#define TEMPID         20   /* : colon       */
#define LABELID         21   /* : colon       */ 


extern char *yytext;		/* in lex.c			*/
extern int yyleng;
extern yylineno;
