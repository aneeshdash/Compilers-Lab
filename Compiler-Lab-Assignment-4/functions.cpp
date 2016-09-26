
        #include "tree.h"
#include <iostream>
#include <list>
using namespace std;

node* add_node(string name, node* a=NULL, node* b=NULL, node* c=NULL, node* d=NULL, node* e=NULL, node* f=NULL, node* g=NULL, node* h=NULL, node* i=NULL, val_type type= NONE, int val=0) {
        static int no = 1;
        node *new_node;
        new_node = new node();

        new_node->children[0] = a;
        new_node->children[1] = b;
        new_node->children[2] = c;
        new_node->children[3] = d;
        new_node->children[4] = e;
        new_node->children[5] = f;
        new_node->children[6] = g;
        new_node->children[7] = h;
        new_node->children[8] = i;
        new_node->children[9] = NULL;
        new_node->node_name=name;
        new_node->node_no = no * 10;
        new_node->type=type;
        new_node->val=val;

        no++;
        // new_node->line_no=line;

        return new_node;
    }

void patch_type(list<int> patch_list, val_type type, vector<variable>& global_var_table)
{
        list <int>::iterator it;
        for (it = patch_list.begin(); it != patch_list.end(); ++it) {
            // (*it)->ele_type = type;
            global_var_table[(*it)].ele_type = type;
        }
}

void patch_type(list<int> patch_list, val_type type, function *active_func_ptr)
{
        list <int>::iterator it;
        for (it = patch_list.begin(); it != patch_list.end(); ++it) {
            // (*it)->ele_type = type;
            active_func_ptr->local_var[(*it)].ele_type = type;
        }
}

int coercible(val_type expr1,val_type expr2)
{
        if(expr1==expr2 && expr1!=NONE && expr1!=ERROR)
                return 1;
        if((expr1==INT&&expr2==BOOL)||(expr1==BOOL&&expr2==INT)||(expr1==INT&&expr2==CHAR)||(expr1==CHAR&&expr2==INT))
                return 1;
        return 0;
}

val_type compare_types(val_type expr1,val_type expr2)
{
        if(expr1==INT||expr2==INT)
                return INT;
        if(expr1==BOOL)
                return BOOL;
        if(expr1==CHAR)
                return CHAR;
        return ERROR;
}

void print_local_var(function *active_func_ptr)
{
        vector<variable>:: iterator it;
        for(it = active_func_ptr->local_var.begin() ; it != active_func_ptr->local_var.end(); ++it)
        {
                cout << it->name << " " << it->type << "  " << it->ele_type  <<"\n";
        }
}

void print_nodes(struct node* root, std::ofstream& myfile)
{
        if(root == NULL)
                return;
        else
        {
                cout << "Parent node " << root->node_no << "( " << root->node_name << " ) : ";
                 myfile << "Parent node " << root->node_no << "( " << root->node_name << " ) : ";
                if(root->children[0] == NULL)
                {
                        cout << root->name;
                        myfile << root->name;
                }
                for(int i=0 ; i<10 ; i++)
                {
                        if(root->children[i] != NULL)
                        {
                                cout << root->children[i]->node_no << " (" << root->children[i]->node_name << ") ";
                                 myfile << root->children[i]->node_no << " (" << root->children[i]->node_name << ") ";
                        }
                        else
                        {
                                cout << "\n";
                                myfile << "\n";
                                break;
                        }
                }
                for(int i=0;i<10; i++)
                {
                        if(root->children[i] != NULL)
                        {
                                print_nodes(root->children[i],myfile);
                                //break;
                        }
                }
        }

}





int check_varlist(vector<variable> var_list, vector<function> func_table, int level,string name1)
{

        vector<variable>:: iterator it;
        for(it = var_list.begin() ; it != var_list.end(); ++it)
        {
                if(it->name == name1)
                {
                        return 0;
                }
        }
        for (size_t i = 0; i < func_table.size(); i++) {
            if(func_table[i].name == name1)
                    return 0;
        }
        return 1;
}

int check_func_varlist(function *current,int level1,string var_name)
{
         vector<variable> var_list = current->local_var;
         vector<variable> param_list = current->params;
         for( int i=0;i< var_list.size(); i++)
         {
                 if(var_list[i].name == var_name && var_list[i].level == level1){
                        return 0;
                 }
        }
        if(level1 == 2)
        {
                for(int i=0; i < param_list.size();i++)
                {
                        if(param_list[i].name == var_name)
                                return 0;
                }

        }
        return 1 ;
}


int check_all_varlist(function *current, int level, string var_name, vector<variable> global_var_table)
{

}

int check_func_name(vector<function> func_list, string name)
{
    for(int i=0;i< func_list.size() ; i++)
    {
            if(func_list[i].name == name)
                    return 0;
    }
    return 1;
}

function get_func(vector<function> func_table, string name)
{
    for(int i=0;i< func_table.size() ; i++)
    {
            if(func_table[i].name == name)
                    return func_table[i];
    }
}

int match_args(function callee, vector<val_type> args_list)
{
    int res = 1;
    if(callee.params.size() == args_list.size())
    {
            for(int i=0; i< args_list.size();i++)
            {
                    if(callee.params[i].ele_type != args_list[i])
                    {
                            res = 0;
                    }

            }
    }
    else
    {
            res = 0;
    }
    return res;
}

// int search_func
int check_declared(function *active_func_ptr ,vector<variable> var_list ,int level,string name1)
{
        for(int i=level; i >=0 ;i--)
        {
                if(!check_func_varlist(active_func_ptr,i,name1))
                        return 1;
        }

         vector<variable>:: iterator it;
        for(it = var_list.begin() ; it != var_list.end(); ++it)
        {
                if(it->name == name1)
                {
                        return 1;
                }
        }
        return 0;
}

variable retrieve_declared(function *active_func_ptr ,vector<variable> var_list ,int level,string name1)
{
        vector<variable> func_var_list = active_func_ptr->local_var;
        vector<variable> param_list = active_func_ptr->params;
        for(int i=level; i > 1 ;i--)
        {
            for( int j=0;j< func_var_list.size(); j++)
            {
                if(func_var_list[j].name == name1 && func_var_list[j].level == i){
                       return func_var_list[j];
                }
           }
        }
        for(int i=0; i < param_list.size();i++)
        {
                if(param_list[i].name == name1)
                        return param_list[i];
        }

         vector<variable>:: iterator it;
        for(it = var_list.begin() ; it != var_list.end(); ++it)
        {
                if(it->name == name1)
                {
                        return *it;
                }
        }
}

void clear_vars(function *active_func_ptr, int level)
{
        for( int j=0;j< active_func_ptr->local_var.size(); j++)
        {
            if(active_func_ptr->local_var[j].level == level){
                   (active_func_ptr->local_var).erase(active_func_ptr->local_var.begin() + j);
            }
       }
}



void print_symbol(vector<variable> var_list)
{
        vector<variable>:: iterator it;
        for(it = var_list.begin() ; it != var_list.end(); ++it)
        {
                cout << it->name << " " << it->type << "  " << it->ele_type  <<"\n";
        }

}

void print_func(vector<function> func_list)
{
        vector<function>:: iterator it;
        for(it = func_list.begin() ; it != func_list.end(); ++it)
        {
                cout << it->name << " " << it->return_type << "\n";
        }

}

string generate_final_code(string text,vector<variable> global_vars)
{
        string code = ".data\n";
        vector<variable>:: iterator it;
        for(it = global_vars.begin() ; it != global_vars.end(); ++it)
        {
                code = code + (*it).name + ": .word 0\n";
        }
        code = code + ".text\n" + text;
        return code;
}


int check_dimension(variable var1,vector<variable> patch_list, int lineno)
{
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
                return 1;
            }
            else
            {
                 printf("Indices are out of bound at line %d.\n",lineno);
                 return 0;
            }
        }
        else
        {
            printf("Dimension mismatch at line %d.\n",lineno);
            return 0;
        }

}

int cal_arr_offset(variable patch_k,variable m)
{
        int offset = 0;
        if(m.dim_ptr.size() > 0)
        {
                int i;
                for(i=0; i < m.dim_ptr.size()-1; i++)
                {
                        offset = offset + patch_k.dim_ptr[i]*m.dim_ptr[i+1];
                }

                offset += patch_k.dim_ptr[i];
        }
        return -(offset*4);

}
