#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex1.h"
#include "lex1.c"

char* symbol_table[1024];
int tail = 0;
int rel_jmp=0;
FILE *fp, *fp2;

int search(char* a)
{
	int t=0;
	while(t < tail)
	{
		if(strcmp(symbol_table[t], a) == 0)
			return t;
		t++;
	}
	return -1;
}

int get_symbol_mem()
{
	char subbuff[yyleng+1];
	int memory_no;
    memcpy( subbuff, yytext, yyleng );
    subbuff[yyleng] = '\0';
	memory_no = search(subbuff);
	if(memory_no == -1)
	{
		symbol_table[tail] = (char *)malloc(yyleng+1);
		memcpy( symbol_table[tail], subbuff, yyleng+1 );
    	fprintf(fp2, "MOV BX, %d\n", tail*2+610);
    	fprintf(fp2, "MOV [BX], 0\n");
    	memory_no = tail;
		tail++;
	}
	return memory_no*2+610;
}

int get_register_no()
{
	return ((*(yytext+2)-48)*2)+600;
	// return regs[*(yytext+2)-48];
}

int get_no()
{
	
    char subbuff[yyleng+1];
    memcpy( subbuff, yytext, yyleng );
    subbuff[yyleng] = '\0';
	return atoi(subbuff);
}


void generate()
{
	while( !match (EOI) )
	{
		if ( match(LABELID) )
		{
			//print label
			fprintf(fp2, "%s", yytext);
			advance();
		}
		else if( match(GOTO) )
		{
			advance();
			// get label string
			// print JP label
			fprintf(fp2, "JMP %s",yytext);
			advance();
		}
		else if( match(IFZ) )
		{
			advance();
			int operand1 = get_register_no();
			// mov operand one to accumulator
			// MOV A 00
			// CMP A operand1
			//
			fprintf(fp2, "MOV AX, 0\n");
			fprintf(fp2, "MOV BX, %d\n", operand1);
			fprintf(fp2, "CMP AX, [BX]\n");
			advance();
			if( match(GOTO) )
			{
				advance();
				if( match(LABELID) )
				{
					// get_label_no
					//print jnz label1
					fprintf(fp2, "JZ %s\n",yytext);
					// JZ Label1
					advance();
				}
				
			}
		}
		else if( match(ID) )
		{
			int operand1 = get_symbol_mem();
			advance();
			
			if(match(PLUS))
			{
				advance();
				advance();
				if(match(TEMPID))
				{
					//getregister number
					int operand2 = get_register_no();
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "ADD [BX], AX\n");
					advance();
				}
				else if(match(ID))
				{
					int operand2 = get_symbol_mem();
					//
					//MOV A M%operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					//ADD M%operand1 A
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "ADD [BX], AX\n");
					advance();
				}
			}
			else if(match(MINUS))
			{
				advance();
				advance();
				if(match(TEMPID))
				{
					//getregister number
					int operand2 = get_register_no();
					// print ADD M%operand1 B 
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n", operand2);
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "SUB [BX], AX\n");
					advance();
				}
				else if(match(ID))
				{
					int operand2 = get_symbol_mem();
					//
					//MOV A M%operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					//ADD M%operand1 A
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "ADD [BX], AX\n");
					advance();
				}
			}
			else if(match(EQUALS))
			{
				advance();
				if(match(ID))
				{
					int operand2 = get_symbol_mem();
					//print MOV M%operand1 M%operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "MOV [BX], AX\n");
					advance();
				}
				else if (match(TEMPID))
				{
					int operand2 = get_register_no();
					// print mov M%operand1 operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "MOV [BX], AX\n");
					advance();
				}
			}
			
		}
		else if( match(TEMPID))
		{
			int operand1 = get_register_no();
			advance();
			if(match(PLUS))
			{
				advance();
				advance();
				if(match(NUM_OR_ID))
				{
					if(match(ID))
					{
						int operand2 = get_symbol_mem();
						// print ADD T%register_no mem_no
						fprintf(fp2, "MOV BX, %d\n", operand2);
						fprintf(fp2, "MOV AX, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n", operand1);
						fprintf(fp2, "ADD [BX], AX\n");
						advance();
					}
					else
					{
						//get number
						int operand2 = get_no();
						//print immediate 1
						fprintf(fp2, "MOV BX, %d\n", operand1);
						fprintf(fp2, "ADD [BX], %d\n", operand2);
						advance();
					}
				}
				else if(match(TEMPID))
				{
					int operand2 = get_register_no();
					// print add T%operand1 T%operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "ADD [BX], AX\n");
					advance();
				}
			}
			else if(match(MINUS))
			{
				advance();
				advance();
				if(match(NUM_OR_ID))
				{
					if(match(ID))
					{
						int operand2 = get_symbol_mem();
						// print ADD T%register_no mem_no
						fprintf(fp2, "MOV BX, %d\n", operand2);
						fprintf(fp2, "MOV AX, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n", operand1);
						fprintf(fp2, "SUB [BX], AX\n");
						advance();
					}
					else
					{
						//get number
						int operand2 = get_no();
						//print immediate 1
						fprintf(fp2, "MOV BX, %d\n", operand1);
						fprintf(fp2, "SUB [BX], %d\n", operand2);
						advance();
					}
				}
				else if(match(TEMPID))
				{
					int operand2 = get_register_no();
					// print add T%operand1 T%operand2
					fprintf(fp2, "MOV BX, %d\n", operand2);
					fprintf(fp2, "MOV AX, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n", operand1);
					fprintf(fp2, "SUB [BX], AX\n");
					advance();
				}
			}
			else if(match(TIMES))
			{
				advance();
				advance();
				if(match(NUM_OR_ID))
				{
					if(match(ID))
					{
						int operand2 = get_symbol_mem();
						// print ADD T%register_no mem_no
						fprintf(fp2, "MOV BX, %d\n" ,operand2);
						fprintf(fp2, "MOV AL, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n" , operand1);
						fprintf(fp2, "MUL [BX]\n" );
						fprintf(fp2, "MOV [BX],AL \n");
						advance();
					}
					else
					{
						//get number
						int operand2 = get_no();
						//print immediate 1
						fprintf(fp2, "MOV AL, %d\n" ,operand2);
						fprintf(fp2, "MOV BX, %d\n" , operand1);
						fprintf(fp2, "MUL [BX]\n" );
						fprintf(fp2, "MOV [BX],AL \n");
						advance();
					}
				}
				else if(match(TEMPID))
				{
					int operand2 = get_register_no();
					// print add T%operand1 T%operand2
					fprintf(fp2, "MOV BX, %d\n" ,operand2);
					fprintf(fp2, "MOV AL, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n" , operand1);
					fprintf(fp2, "MUL [BX]\n" );
					fprintf(fp2, "MOV [BX],AL \n");
					advance();
				}
			}
			else if(match(DIV))
			{
				advance();
				advance();
				if(match(NUM_OR_ID))
				{
					if(match(ID))
					{
						int operand2 = get_symbol_mem();
						// print ADD T%register_no mem_no 
						fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV AL, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n" , operand2);
						fprintf(fp2, "DIV [BX]\n" );
						fprintf(fp2, "MOV BX, %d\n",operand1);
						fprintf(fp2, "MOV [BX],AL \n");
						advance();
					}
					else
					{
						//get number
						int operand2 = get_no();
						//print immediate 1
						fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV AL, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n" , operand2);
						fprintf(fp2, "DIV BX\n" );
						fprintf(fp2, "MOV BX, %d\n",operand1);
						fprintf(fp2, "MOV [BX],AL \n");
						advance();
					}
				}
				else if(match(TEMPID))
				{
					int operand2 = get_register_no();
					// print add T%operand1 T%operand2
					fprintf(fp2, "MOV BX, %d\n" ,operand1);
					fprintf(fp2, "MOV AL, [BX]\n");
					fprintf(fp2, "MOV BX, %d\n" , operand2);
					fprintf(fp2, "DIV [BX]\n" );
					fprintf(fp2, "MOV BX, %d\n",operand1);
					fprintf(fp2, "MOV [BX],AL \n");
					advance();
				}
			}
			else if(match(EQUALS))
			{
				advance();
				if(match(NUM_OR_ID))
				{
					if(match(ID))
					{
						int operand2 = get_symbol_mem();
						// print ADD T%register_no mem_no
						fprintf(fp2, "MOV BX, %d\n" ,operand2);
						fprintf(fp2, "MOV AX, [BX]\n");
						fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV [BX], AX\n");
						advance();
					}
					else
					{
						//get number
						int operand2 = get_no();
						//print immediate 1
						fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV [BX], %d\n", operand2);
						advance();
					}
				}
				else if (match(TEMPID))
				{
					int operand2 = get_register_no();
					advance();
					if ( match(GT) || match(LT) || match(EQUALS) )
					{
					    int rel[3];
					    rel[2] = '\0';
					    rel[0] = *yytext;
					    advance();
					    fprintf(fp2, "MOV BX, %d\n" ,operand2);
					    fprintf(fp2, "MOV AX, [BX]\n");
					    if ( match (EQUALS) )
					    {
					        rel[1] = *(yytext);
					        advance();
					        int operand3 = get_register_no();
					        fprintf(fp2, "MOV BX, %d\n" ,operand3);
					    	fprintf(fp2, "CMP AX, [BX]\n");
					        if (strcmp(rel, "=="))
					        {
					            /* a == b
					        	mov ax,a
								cmp ax,b
								jnz L1
								mov a1,1
								jmp L2
								L1: mov a1,0
								L2:
								*/
								fprintf(fp2, "JNZ R%d\n",rel_jmp);
					            
					        }
					        else if (strcmp(rel, ">="))
					        {
					             /* mov ax,a
								cmp ax,b
								jl L1
								mov a1,1
								jmp L2
								L1: mov a1,0
								L2:
								*/
								fprintf(fp2, "JL R%d\n", rel_jmp);
					        }
					        else
					        {
					             /* mov ax,a
								cmp ax,b
								jg L1
								mov a1,1
								jmp L2
								L1: mov a1,0
								L2:
								*/
								fprintf(fp2, "JG R%d\n", rel_jmp);
					        }
					    }
					    else
					    {
					        int operand3 = get_register_no();
					        fprintf(fp2, "MOV BX, %d\n" ,operand3);
					    	fprintf(fp2, "CMP AX, [BX]\n");
					        if(rel[0] == '=')
					        {
					            
					        }
					        else if (rel[0] == '>')
					        {
					             /* a == b
					        	mov ax,a
								cmp ax,b
								jnz L1
								mov a1,1
								jmp L2
								L1: mov a1,0
								L2:
								*/
								fprintf(fp2, "JLE R%d\n", rel_jmp);
					        }
					        else
					        {
					             /* mov ax,a
								cmp ax,b
								jle L1
								mov a1,1
								jmp L2
								L1: mov a1,0
								L2:
								*/
								fprintf(fp2, "JGE R%d\n", rel_jmp);
					        }
					        
					    }
					    rel_jmp++;
					    fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV [BX], 1\n");
						fprintf(fp2, "JMP R%d\n", rel_jmp);
						fprintf(fp2, "R%d:\n" ,rel_jmp-1);
						fprintf(fp2, "MOV BX, %d\n" ,operand1);
						fprintf(fp2, "MOV [BX], 0\n");
						fprintf(fp2, "R%d:\n", rel_jmp++);
					}
					else
					{
					    // print mov operand1 operand2
					    fprintf(fp2, "MOV BX, %d\n" ,operand2);
					    fprintf(fp2, "MOV AX, [BX]\n");
					    fprintf(fp2, "MOV BX, %d\n" ,operand1);
					    fprintf(fp2, "MOV [BX], AX\n");
					}
				}
			}
		}
		
	}
}

int main()
{
	fp = fopen("3addr.txt", "r");
	fp2 = fopen("8086.txt", "w");
	generate();
	fprintf(fp2, "HLT");
	fclose(fp);
	fclose(fp2);
}