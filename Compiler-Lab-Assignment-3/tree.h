#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;

struct node{
    node* children[10];
    string node_name;
    string node_val;
    int line_no;
    int node_no;
    bool is_int;
    bool is_bool;

    node() {
        is_int=false;
        is_bool=false;
    }
};
