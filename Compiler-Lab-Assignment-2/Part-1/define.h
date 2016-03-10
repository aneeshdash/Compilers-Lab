/*
This file contains all constants and structure used in the code
*/
#include <cstdio>
#include <vector>
using namespace std;

#define OB 26
#define CB 27
#define AST 28
#define OR 29
#define AND 30
#define EPSILON 31


struct nfa {
    vector< vector< vector<int> > > trans;
};

struct nfa_dfa_state {
    vector<bool> state;
    vector<int> trans;
    bool isFinal;
    bool isStart;

    nfa_dfa_state() : trans(26,-1), isFinal(0), isStart(0) {}
};
