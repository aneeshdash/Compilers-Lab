char  *Names[] = { "_t0", "_t1", "_t2", "_t3", "_t4", "_t5", "_t6", "_t7" };   
char  **Namep  = Names;
int   loop_count =0 ;
   
char  *newname()   
{   
    if( Namep >= &Names[ sizeof(Names)/sizeof(*Names) ] )   
    {   
        fprintf( stderr, "%d: Expression too complex\n", yylineno );   
        exit( 1 );   
    }   
   
    return( *Namep++ );   
}   
   
void freename(char *s)   

{
    // char    *s;   
    if( Namep > Names )   
    *--Namep = s;   
    else   
    fprintf(stderr, "%d: (Internal error) Name stack underflow\n",   
                                yylineno );   
}   
