#include "tree.h"
#include <iostream>
node* add_node(string name, node* a=NULL, node* b=NULL, node* c=NULL, node* d=NULL, node* e=NULL, node* f=NULL, node* g=NULL, node* h=NULL, node* i=NULL) {
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
        no++;
        // new_node->line_no=line;

        return new_node;
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
                        cout << root->node_val;
                        myfile << root->node_val;
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
