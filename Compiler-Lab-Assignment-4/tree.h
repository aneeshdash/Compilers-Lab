#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;

enum val_type { INT, BOOL, CHAR, NONE, ERROR };
enum var_type { SIMPLE , ARRAY};


struct variable
{

   string name;
   var_type type;
   val_type ele_type;
   vector<int> dim_ptr;
   int level;
   int offset;

   variable() {}

   variable(string name1 ,var_type type1 ,val_type ele_type1 ,int level1 )
   {
       name = name1;
       type = type1;
       ele_type = ele_type1;
       level = level1;


   }
};

struct function
{
    string name;
    val_type return_type;
    vector<variable> params;
    vector<variable> local_var;

    function(){}
    function(string name1 ,val_type return_type1)
    {
        name = name1;
        return_type = return_type1;
    }

};

struct node{
    node* children[10];
    string node_name;
    string name;
    int line_no;
    int node_no;
    int val;
    val_type type;
    string code;

};
