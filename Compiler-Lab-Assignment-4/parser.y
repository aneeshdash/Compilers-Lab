%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>
    #include <fstream>
    #include <list>
    #include <string>
    #include "functions.cpp"
    using namespace std;
    int yylex(void);
    void yyerror(char *s)
    {

        fprintf(stderr,"Unknown errors detected.\n");
        extern int yylineno;
    }

    int flag = 0, level=0,sem_flag=0, offset, param_offset;
    int label = 0 ;
    vector<function> func_table;
    vector<variable> global_var_table;
    vector<int> dim_list;
    function *active_func_ptr;
    function call_name_ptr;
    vector<val_type> args_list;
    vector<variable> patch_list;
    val_type data_type;
    extern int  yylex();
    extern int yylineno;
    extern int lineno;
    FILE *fout = fopen("output.asm","w");
    // fclose(fout);
    // fout = fopen("output.asm","a");
%}

%union{
	node *Node;
}

%token <Node>   NUMCONST SCOPE_SPECIFIER TYPE_SPECIFIER RETURN PRINT IF ELSE WHILE BREAK BOOLCONST READ IDENTIFIER COMMA CHARVAL
%token <Node>   SEMICOLON OPENBRACES OPENPAREN CLOSEDBRACES CLOSEDPAREN OPENSQUAREBRACES CLOSEDSQUAREBRACES
%token <Node>   MINUS PLUS BY TIMES MOD NOT
%token <Node>   GREQ LEQ LESS GREATER NEQ EQUALS AND OR ASSIGN
%start   program
%expect 6

%type <Node> program declaration_list declaration empty variable_dec variable_dec_list function_dec parameters parameter_list parameter_type_list statement compound_stmt stmt_list expression_stmt conditional_stmt iteration_stmt return_stmt break_stmt expression expr_suffix simple_expr and_expr unary_rel_expr rel_expr sum_expr sumop term mulop factor call args arg_list constant openparen closedparen closedbraces comma semicolon id id_arr dimlist

%%

program : declaration_list
{
     $$ = add_node("program",$1);
    $$->code = $1->code;
    $$->code = generate_final_code($$->code,global_var_table);
    if(flag==0&&sem_flag==0)
    {
        printf("Compilation successful.\n");
        ofstream output;
	    output.open("tree.txt");
        //print_nodes($$,output);
        //print_symbol(global_var_table);
        //print_func(func_table);
        output.close();
        fprintf(fout,$$->code.c_str());
        printf("ASM file generated.\n");
    }
    else if(flag==1&&sem_flag==0)
    {
        printf("Syntax errors found.\n");
    }
    else if(flag==0&&sem_flag==1)
    {
        printf("Semantic errors found.\n");
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
    $$->code = $1->code + $2->code;
}
                 | declaration
{
    $$ = add_node("declaration_list",$1);
    $$->code = $1->code;
}
                 ;

declaration : function_dec
{
   $$ = add_node("declaration",$1);
   $$->code = $1->code;
}
            | variable_dec
{
   $$ = add_node("declaration",$1);
   $$->code = $1->code;
}
            | error {printf("Error: Missing Type Specifier at line %d\n", lineno); flag = 1;$$ = NULL;}
            ;


variable_dec            : TYPE_SPECIFIER variable_dec_list semicolon
{
    string temp_code = "li $a0, 0\n";
    $$ = add_node("variable_dec",$1,$2,$3);
    if(level)
    {
        for(int i=0; i< patch_list.size();i++)
        {
            if(check_varlist(global_var_table, func_table, 0, patch_list[i].name) && (!level || check_func_varlist(active_func_ptr, level, patch_list[i].name)))
            {
                variable k = patch_list[i];
                k.ele_type = $1->type;
                for (size_t i = 0; i < k.offset/4; i++) {
                    temp_code += "sw $a0, -"+to_string(offset + i*4)+"($fp)\n";
                }
                offset += k.offset;
                k.offset = -(offset - k.offset);
                active_func_ptr->local_var.push_back(k);

            }
            else
            {
                printf("Error near line %d. Function or variable with same name already declared.\n",lineno);
                sem_flag = 1;
            }
        }
    }
    else
    {
        for(int i=0;i<patch_list.size();i++)
        {
            if(check_varlist(global_var_table, func_table, 0, patch_list[i].name) && (!level || check_func_varlist(active_func_ptr, level, patch_list[i].name)))
            {
                    variable k = patch_list[i];
                    k.ele_type = $1->type;
                    k.offset = -1;
                    global_var_table.push_back(k);
            }
            else
            {
                    printf("Error near line %d. Function or variable with same name already declared.\n",lineno);
                    sem_flag = 1;
            }
        }

    }
    $$->code = temp_code;
    patch_list.clear();
}
;


variable_dec_list       : variable_dec_list comma id_arr
{
    $$ = add_node("variable_dec_list",$1,$2,$3);
}
                        | id_arr

{
    $$ = add_node("variable_dec_list",$1);
}
;

id_arr : id OPENSQUAREBRACES dimlist CLOSEDSQUAREBRACES
{
    $$ = add_node( "id_arr" ,$1,$2,$3,$4);
    int temp_offset = 1;
    $$->name = $1->name;
    variable k($$->name,ARRAY,NONE,level);
    k.offset = temp_offset;
    for (size_t i = 0; i < dim_list.size(); i++) {
        k.dim_ptr.push_back(dim_list[i]);
        temp_offset *= dim_list[i];
    }
    k.offset = temp_offset*4;
    dim_list.clear();
    patch_list.push_back(k);
}
    |
        id
{
    $$ = add_node("id_arr" ,$1);
    $$->name = $1->name;
    variable k($$->name,SIMPLE,NONE,level);
    k.offset = 4;
    patch_list.push_back(k);

}
;

id                      : IDENTIFIER
{
    $$ = add_node("id",$1);
    $$->name = $1->name;
}
;


dimlist : NUMCONST comma dimlist
{
    $$ = add_node("dimlist",$1,$2,$3);
    dim_list.insert(dim_list.begin(), $1->val);
}

        | NUMCONST

{
    $$ = add_node("dimlist",$1);
    dim_list.push_back($1->val);
}

;


function_dec            : TYPE_SPECIFIER IDENTIFIER OPENPAREN
{
    level++;
    offset = 4;
    if($2->name == "main")
        param_offset = 4;
    else
        param_offset = 8;
    // offset = 8;
    // param_offset = 0;
    if(check_varlist(global_var_table, func_table, 0, $2->name))
    {
        function cur_func($2->name ,$1->type);
        func_table.push_back(cur_func);
        active_func_ptr = &func_table.back();
    }
    else
    {
        printf("Error near line %d. Function or variable with same name is already declared.\n",lineno);
        sem_flag=1;
        function cur_func($2->name ,$1->type);
        active_func_ptr = &cur_func;
    }

} parameters closedparen compound_stmt
{
    level--;
    string temp_code;
    if(active_func_ptr->name == "main")
        temp_code = "main:\n";
    else
        temp_code = "func_"+$2->name+":\n";
    temp_code += "move $fp, $sp\nsw $ra, 0($sp)\naddiu $sp, $sp, -4\n";
    temp_code += "addiu $sp, $sp, -"+to_string(offset - 4) + "\n";
    temp_code += $7->code;
    temp_code += "lw $ra, 0($fp)\n";
    temp_code += "lw $fp, 4($fp)\n";
    if(active_func_ptr->name == "main")
        temp_code += "addiu $sp, $sp, 4\n";
    else
        temp_code += "addiu $sp, $sp, "+to_string(offset + param_offset)+"\n";
    temp_code += "jr $ra\n";
    // if(active_func_ptr->name == "main")
    //     temp_code += "li $v0,10\nsyscall";
    // else
    //     temp_code += "jr $ra\n";
    $$ = add_node("function_dec",$1,$2,$3,$5,$6,$7);
    //print_local_var(active_func_ptr);
    $$->code = temp_code;

}


;


parameters              : parameter_list
{
    // param_offset = 0;
    $$ = add_node("parameters",$1);
}
| empty
{
    // param_offset = 0;
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
   variable k($2->name, SIMPLE, $1->type, 1);
   k.offset = param_offset;
   active_func_ptr->params.push_back(k);
   param_offset += 4;
}

;




statement               : expression_stmt
{
    $$ = add_node("statement",$1);
    $$->code = $1->code;
}
                        | compound_stmt
{
    $$ = add_node("statement",$1);
    $$->code = $1->code;
}

                        | conditional_stmt
{
    $$ = add_node("statement",$1);
    $$->code = $1->code;
}
                        | iteration_stmt
{
    $$ = add_node("statement",$1);
    $$->code = $1->code;
}
                        | return_stmt
{
    $$ = add_node("statement",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    if(active_func_ptr->return_type != $$->type)
    {
        printf("Return type mismatch at line %d.\n",lineno);
    }
    $$->code = $1->code;
}
                        | break_stmt
{
    $$ = add_node("statement",$1);
}
                        |{patch_list.clear();} variable_dec
{
    $$ = add_node("statement",$2);
    //patch_list.clear();
    $$->code = $2->code;
}
                        ;

compound_stmt           : OPENBRACES { level++; } stmt_list closedbraces
{
    clear_vars(active_func_ptr, level);
    level--;
    $$ = add_node("compound_stmt",$1,$3,$4);
    $$->code = $3->code;

}
;

stmt_list               : stmt_list statement
{
    $$ = add_node("stmt_list",$1, $2);
    $$->code = $1->code + $2->code;
}
                        | empty
{
    $$ = add_node("stmt_list",$1);
}
                        ;

expression_stmt         : expression semicolon
{
    $$ = add_node("expression_stmt",$1,$2);
    $$->code = $1->code;
}
                         ;

conditional_stmt        : IF  openparen expression closedparen statement
{
    $$ = add_node("conditional_stmt",$1,$2,$3,$4,$5);

    $$->code = $3->code + "li $t0,1" + "\n" + "blt	$a0,$t0," + "L_"+to_string(label)+"\n" + $5->code+"L_"+to_string(label)+":\n";
    label++;


}

                        | IF openparen expression closedparen statement ELSE statement
{
    $$ = add_node("conditional_stmt",$1,$2,$3,$4,$5,$6,$7);
    $$->code = $3->code + "li $t0,1" + "\n" + "blt $a0,$t0, " + "L_"+to_string(label)+"\n";
    label++;

    $$->code = $$->code + $5->code+"b L_" +to_string(label)+"\nL_"+to_string(label-1)+":"+$7->code+"L_"+to_string(label)+":";
    label++;
}
                        ;

iteration_stmt          : WHILE openparen expression closedparen statement
{
    $$ = add_node("iteration_stmt",$1,$2,$3,$4,$5);
    $$->code = "\nL_" + to_string(label)+":\n" + $3->code +"li $t0,1\n";
    label++;
    $$->code = $$->code + "blt $a0,$t0, " + "L_"+to_string(label)+"\n" + $5->code+"j "+"L_"+to_string(label-1)+"\nL_"+to_string(label)+":\n";
    label++;
}
;

return_stmt             : RETURN  semicolon
{
    $$ = add_node("return_stmt",$1,$2);
    $$->code = "lw $ra, 0($fp)\nlw $fp, 4($fp)\naddiu $sp, $sp, "+to_string(offset + param_offset)+"\njr $ra\n";
}
                        | RETURN  expression semicolon
{
    $$ = add_node("return_stmt",$1,$2,$3);
    // see if need to update $a
    $$->code= $2->code + "lw $ra, 0($fp)\nlw $fp, 4($fp)\naddiu $sp, $sp, "+to_string(offset + param_offset)+"\njr $ra\n";
    $$->type = $2->type;
};

break_stmt              : BREAK semicolon
{
    $$ = add_node("break_stmt",$1,$2);
}
;

expression              : id_arr ASSIGN simple_expr
{
    $$ = add_node("expression",$1,$2,$3);
    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);

         if(check_dimension(var1,patch_list, lineno))
         {
            int arr_offset = 0;

            arr_offset = cal_arr_offset(patch_list[patch_list.size()-1],var1);
            //printf("%d\n" ,cal_arr_offset(patch_list[patch_list.size()-1],var1));


            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types. Assignment failed.\n",lineno);
                sem_flag = 1;
            }
            else
            {
                if(var1.offset == -1)
                {
                     $1->val = $3->val;
                    $$->type = BOOL;
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nsw $a0,($t1)\n";
                }
                else
                {
                    $1->val = $3->val;
                    $$->type = BOOL;
                    $$->val = 1;
                    $$->code= $3->code + "sw $a0,"+to_string(var1.offset+arr_offset)+"($fp)"+"\n"+ "li $a0,1\n";
                    //var1.offset($fp)
                }
            }

        }

    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        $$->type = ERROR;
        sem_flag = 1;
    }

}

                        | id_arr PLUS expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
     if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);


        if(check_dimension(var1,patch_list, lineno))
        {
            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types cannot be added.\n",lineno);
                $$->type = ERROR;
                sem_flag = 1;
            }
            else
            {
                if(var1.offset == -1)
                {
                    $1->val = $3->val;
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nlw $t2,($t1)\nadd $a0,$a0,$t2\n";
                }
                else
                {
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->code= $3->code + "move $t1,$a0\nlw $a0,"+to_string(var1.offset)+"($fp)"+"\n"+ "add $a0,$a0,$t1\n";
                }

            }

        }
    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        sem_flag = 1;
    }
}
                        | id_arr MINUS expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);
        if(check_dimension(var1,patch_list, lineno))
        {

            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types cannot be added.\n",lineno);
                $$->type = ERROR;
                sem_flag = 1;
            }
            else
            {
                printf("%d\n", var1.offset);
                if(var1.offset == -1)
                {
                    $1->val = $3->val;
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nlw $t2,($t1)\nsub $a0,$a0,$t2\n";
                }
                else
                {
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->code= $3->code + "move $t1,$a0\nlw $a0,"+to_string(var1.offset)+"($fp)"+"\n"+ "sub $a0,$a0,$t1\n";
                }
            }
        }
    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        sem_flag = 1;
    }

}
                        | id_arr TIMES expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
         variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);
        if(check_dimension(var1,patch_list, lineno))
        {

            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types cannot be added.\n",lineno);
                $$->type = ERROR;
                sem_flag = 1;
            }
            else
            {
                if(var1.offset == -1)
                {
                    $1->val = $3->val;
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nlw $t2,($t1)\nmult $a0,$t2\nmflo $a0\n";
                }
                else
                {
                     $$->type = compare_types(var1.ele_type,$3->type);
                    $$->code= $3->code + "move $t1,$a0\nlw $a0,"+to_string(var1.offset)+"($fp)"+"\nmult $a0,$t1\nmflo $a0\n";
                }

        }
    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        sem_flag = 1;
    }
}
}
                        | id_arr BY expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);
        if(check_dimension(var1,patch_list, lineno))
        {

            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types cannot be added.\n",lineno);
                $$->type = ERROR;
                sem_flag = 1;
            }
            else
            {
                if(var1.offset == -1)
                {
                    $1->val = $3->val;
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nlw $t2,($t1)\ndiv $a0,$t2\nmflo $a0\n";
                }
                else
                {
                     $$->type = compare_types(var1.ele_type,$3->type);
                    $$->code= $3->code + "move $t1,$a0\nlw $a0,"+to_string(var1.offset)+"($fp)"+"\ndiv $a0,$t1\nmflo $a0\n";
                }
            }
        }
    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        sem_flag = 1;
    }

}
                        | id_arr MOD expr_suffix
{
    $$ = add_node("expression",$1,$2,$3);
    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);
        if(check_dimension(var1,patch_list, lineno))
        {

            if(!coercible(var1.ele_type,$3->type))
            {
                printf("Error at line %d: Non-coercible types cannot be added.\n",lineno);
                $$->type = ERROR;
                sem_flag = 1;
            }
            else
            {
                if(var1.offset == -1)
                {
                    $1->val = $3->val;
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->val = 1;
                    $$->code = $3->code + "la $t1," + var1.name + "\nlw $t2,($t1)\ndiv $a0,$t2\nmfhi $a0\n";
                }
                else
                {
                    $$->type = compare_types(var1.ele_type,$3->type);
                    $$->code= $3->code + "move $t1,$a0\nlw $a0,"+to_string(var1.offset)+"($fp)"+"\ndiv $a0,$t1\nmfhi $a0\n";
                }
            }
        }
    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        sem_flag = 1;
    }

}
                        | simple_expr
{
    $$ = add_node("expression",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        ;

expr_suffix             : simple_expr
{
    $$ = add_node("expr_suffix",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        | ASSIGN simple_expr
{
    $$ = add_node("expr_suffix",$1,$2);
}


simple_expr             : OPENPAREN simple_expr OR and_expr closedparen
{
    $$ = add_node("simple_expr",$1,$2,$3,$4,$5);
     $$->type = BOOL;
    $$->val = $2->val || $4->val;
    int label1 = label++;
    int label2 = label++;
    $$->code = $2->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $4->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nadd $t1,$t1,$t2\nli $t2,0\nbeq $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | and_expr
{
    $$ = add_node("simple_expr",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        ;

and_expr                : and_expr AND unary_rel_expr
{
    $$ = add_node("and_expr",$1,$2,$3);
     $$->type = BOOL;
    $$->val = $1->val && $3->val;
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nmult $t1,$t2\nmflo $t1\nli $t2,0\nbeq $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | unary_rel_expr
{
    $$ = add_node("and_expr",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        ;

unary_rel_expr          : NOT unary_rel_expr
{
    $$ = add_node("unary_rel_expr",$1,$2);
     $$->type = BOOL;
    $$->val = ($1->val+1)%2;
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "move $t1,$a0\nli $t2,0\nbeq $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +
        "\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | rel_expr
{
    $$ = add_node("unary_rel_expr",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        ;

rel_expr                : sum_expr GREATER sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);

     $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val > $3->val)
        {
            $$->val == 1;
        }
        else
        {
            $$->val == 0;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nble $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | sum_expr LESS sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
     $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val < $3->val)
        {
            $$->val == 1;
        }
        else
        {
            $$->val == 0;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nbge $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | sum_expr EQUALS sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
     $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val == $3->val)
        {
            $$->val == 1;
        }
        else
        {
            $$->val == 0;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nbeq $t1,$t2,L_" + to_string(label1) + "\nli $a0,0\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,1\nL_" + to_string(label2) + ":\n";
}
                        | sum_expr GREQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
     $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val >= $3->val)
        {
            $$->val == 1;
        }
        else
        {
            $$->val == 0;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nblt $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | sum_expr LEQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
     $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val <= $3->val)
        {
            $$->val == 1;
        }
        else
        {
            $$->val == 0;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nbgt $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}
                        | sum_expr NEQ sum_expr
{
    $$ = add_node("rel_expr",$1,$2,$3);
    $$->type = BOOL;
    if(coercible($1->type,$3->type))
    {
        if($1->val == $3->val)
        {
            $$->val == 0;
        }
        else
        {
            $$->val == 1;
        }
    }
    else
    {
        printf("Error at line %d: Non-coercible types. Comparison failed.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    int label1 = label++;
    int label2 = label++;
    $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\naddiu $sp,$sp,4\nmove $t2,$a0\nbeq $t1,$t2,L_" + to_string(label1) + "\nli $a0,1\nb L_" + to_string(label2) +"\nL_" + to_string(label1) + ": li $a0,0\nL_" + to_string(label2) + ":\n";
}

                        | sum_expr
{
    $$ = add_node("rel_expr",$1);

    $$->type = $1->type;
    if($1->type == BOOL)
    {
        $$->val = $1->val;
    }
    else
    {
        if($1->val > 0)
            $$->val = 1;
        else
            $$->val = 0;
    }
    $$->code = $1->code;
}
                        ;

sum_expr                : sum_expr sumop term
{
    $$ = add_node("sum_expr",$1,$2,$3);
    if(coercible($1->type,$3->type)==0)
    {
        printf("Error at line %d: Operation on non-coercible types %d %d.\n",lineno,$1->type,$3->type);
        sem_flag = 1;
        $$->type = ERROR;
    }
    else
    {
        $$->type = compare_types($1->type,$3->type);
        if($2->val == 0)
        {
            $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\nadd $a0,$a0,$t1\naddiu $sp,$sp,4\n";
            $$->val = $1->val + $3->val;
        }
        else if($2->val == 1)
        {
            $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\nsub $a0,$t1,$a0\naddiu $sp,$sp,4\n";
            $$->val = $1->val - $3->val;
        }
    }
}
                        | term
{
    $$ = add_node("sum_expr",$1);
    $$->type = $1->type;
    $$->val = $$->val;
    $$->code = $1->code;
}
                        ;

sumop                   : PLUS
{
    $$ = add_node("sumop",$1);
    $$->val = 0;
}
                        | MINUS
{
    $$ = add_node("sumop",$1);
    $$->val = 1;
}
                        ;

term                    : term mulop factor
{
    $$ = add_node("term",$1,$2,$3);
    if(coercible($1->type,$3->type)==0)
    {
        printf("Error at line %d: Operation on non-coercible types.\n",lineno);
        sem_flag = 1;
        $$->type = ERROR;
    }
    else
    {
        $$->type = compare_types($1->type,$3->type);
        if($2->val == 0)
        {
            $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\nmult $a0,$t1\nmflo $a0\naddiu $sp,$sp,4\n";
            $$->val = $1->val * $3->val;
        }
        else if($2->val == 1)
        {
            $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\ndiv $t1,$a0\nmflo $a0\naddiu $sp,$sp,4\n";
            $$->val = $1->val / $3->val;
        }
        else if($2->val==2)
        {
            $$->code = $1->code + "sw $a0,0($sp)\naddiu $sp,$sp,-4\n" + $3->code + "lw $t1,4($sp)\ndiv $t1,$a0\nmfhi $a0\naddiu $sp,$sp,4\n";
            $$->val = $1->val % $3->val;
        }
    }
}
                        | factor
{
    $$ = add_node("term",$1);
    $$->type = $1->type;
    $$->val = $$ ->val;
    $$->code = $1->code;
}
                        ;

mulop                   : TIMES
{
    $$ = add_node("mulop",$1);
    $$->val = 0;
}

                        | BY
{
    $$ = add_node("mulop",$1);
    $$->val = 1;
}
                        | MOD
{
    $$ = add_node("mulop",$1);
    $$->val = 2;
}
;

factor                  : id_arr
{
    $$ = add_node("factor",$1);

    if(check_declared(active_func_ptr,global_var_table,level,$1->name))
    {
        variable var1 = retrieve_declared(active_func_ptr,global_var_table,level,$1->name);

       // printf("%d %d\n" ,patch_list.size() , var1.dim_ptr.size());


        //printf("%s\n",patch_list[patch_list.size()].name.c_str());
        if(patch_list[patch_list.size()-1].dim_ptr.size() == var1.dim_ptr.size() )
        {
            int res = 1;
            for(int i=0;i< var1.dim_ptr.size() ;i++)
            {
                if(patch_list[patch_list.size()-1].dim_ptr[i] >= var1.dim_ptr[i])
                {
                    res=0;
                }
            }
            if(res == 1)
            {
                if(var1.offset == -1)
                {
                    $$->val = $1->val;
                    $$->type = var1.ele_type;
                    $$->code = "la $t1," + var1.name + "\nlw $a0,($t1)\n";
                }
                else
                {
                    $$->type = var1.ele_type;
                    $$->code= "lw $a0,"+to_string(var1.offset)+"($fp)\n";
                }
            }
            else
            {
                 printf("indexes are out of bound\n");
            }
        }
        else
        {
            printf("dimension mismatch\n");
        }

    }
    else
    {
        printf("Error at line %d: Variable not declared.\n",lineno);
        $$->type = ERROR;
        sem_flag = 1;
    }
}
                        | OPENPAREN expression closedparen
{
    $$ = add_node("factor",$1,$2,$3);
    $$->type = $2->type;
    $$->val  =$2->val;
    $$->code = $2->code;
}

                        |  call
{
    $$ = add_node("factor",$1);
    $$->type = $1->type;
    $$->val = $$->val;
    $$->code = $1->code;
}
                        | constant
{
    $$ = add_node("factor",$1);
    $$->type = $1->type;
    $$->val = $1->val;
    $$->code = $1->code;
}
                        ;

call                    : IDENTIFIER OPENPAREN args closedparen
{
    $$ = add_node("call",$1,$2,$3,$4);
    if(check_func_name(func_table, $1->name)) {
        printf("Error at line %d: Function not declared.\n",lineno);
        sem_flag = 1;
    }
    else {
        call_name_ptr = get_func(func_table, $1->name);
        if(match_args(call_name_ptr, args_list)) {
            $$ ->type = call_name_ptr.return_type;
            $$->code = $3->code + "jal func_" + call_name_ptr.name+"\n";
            // printf("hello %s\n", $$->code.c_str());
        }
        else
        {
            printf("Argument mismatch in function call at line %d.\n",lineno);
            sem_flag = 1;
            $$->type = ERROR;
        }
        args_list.clear();
    }
}
;

args                    : arg_list
{
    $$ = add_node("args",$1);
    $$->code = $1->code + "sw $fp, 0($sp)\naddiu $sp, $sp, -4\n";
}

                        | empty
{
    $$ = add_node("args",$1);
    $$->code = "sw $fp, 0($sp)\naddiu $sp, $sp, -4\n";
}
                        ;

arg_list                : arg_list comma expression
{
    $$ = add_node("arg_list",$1,$2,$3);
    args_list.push_back($3->type);
    string temp_code = $3->code + "sw $a0, 0($sp)\naddiu $sp, $sp, -4\n";
    $$->code = temp_code + $1->code;


}
                        | expression
{
    $$ = add_node("arg_list",$1);
    args_list.push_back($1->type);
    $$->type = $1->type;
    $$->code = $1->code + "sw $a0, 0($sp) \naddiu $sp, $sp, -4\n";
}
                        ;

constant                : NUMCONST
{
    $$ = add_node("constant",$1);
    $$->type = INT;
    $$->val = $1->val;
    $$->code = "li $a0,"+to_string($1->val)+"\n";
}
                        | BOOLCONST
{
    $$ = add_node("constant",$1);
    $$->type = BOOL;
    $$->val = $1->val;
    $$->code = "li $a0,"+to_string($1->val)+"\n";
}
                        | CHARVAL
{
    $$ = add_node("constant" ,$1);
    $$->type = CHAR;
    $$->val = $1->val;
    $$->code = "li $a0,"+to_string($1->val)+"\n";
}
                        ;

openparen               : OPENPAREN

{
    $$ = add_node("openparen",$1);
}
                        | error
{
printf("Error: '(' expected after IF or WHILE at line %d\n", lineno);
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
                        | error {printf("Error: Missing ';' or ',' at line %d\n", lineno - 1); flag = 1; $$ = NULL;
};

%%
int main(){
   //yydebug = 1;
    yyparse();
    return 0 ;
}
