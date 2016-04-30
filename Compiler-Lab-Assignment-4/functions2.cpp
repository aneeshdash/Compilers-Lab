#include "tree.h"

node *add_node_leaf(string name, string value, val_type type=NONE)
{
    static int no = 1;
    node *new_node;
    new_node = new node();
    new_node->children[0] = NULL;
    new_node->children[1] = NULL;
    new_node->children[2] = NULL;
    new_node->children[3] = NULL;
    new_node->children[4] = NULL;
    new_node->children[5] = NULL;
    new_node->children[6] = NULL;
    new_node->children[7] = NULL;
    new_node->children[8] = NULL;
    new_node->children[9] = NULL;
    new_node->node_name=name;
    new_node->name=value;
    new_node->type=type;

    if(name.compare("NUMBER") == 0) {
        new_node->val = atoi(value.c_str());
    }

    else if(name.compare("BOOLCONST") == 0) {
        if(value.compare("true") == 0)
            new_node->val = 1;
        else
            new_node->val = 0;
    }

    else if(name.compare("CHARVAL") == 0) {
        new_node->val = value[1];
    }


    new_node->node_no = no * 10 + 1;
    no++;
    return new_node;
}
