#include <cstdio>
#include <vector>
#include "define.h"
using namespace std;


//Create a new NFA
nfa create(int inp)
{
    nfa a;
    vector< vector<int> > tr;
    vector<int> t;
    for (size_t i = 0; i < 27; i++)
        tr.push_back(t);
    tr[inp].push_back(1);
    a.trans.push_back(tr);
    tr[inp].clear();
    a.trans.push_back(tr);
    return a;
}

//Concat two NFA a.b
nfa concat(nfa a, nfa b)
{
    nfa final;
    int offset = a.trans.size();
    a.trans.back()[26].push_back(offset);
    final.trans.insert(final.trans.end(),a.trans.begin(),a.trans.end());
    for (size_t i = 0; i < b.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            for (size_t k = 0; k < b.trans[i][j].size(); k++) {
                b.trans[i][j][k] += offset;
            }
        }
    }
    final.trans.insert(final.trans.end(),b.trans.begin(),b.trans.end());
    return final;
}

//Union of two NFA a+b
nfa unions(nfa a, nfa b)
{
    nfa final;
    vector< vector<int> > v;
    vector<int> vi;
    int offset = a.trans.size() + 1;
    for (size_t i = 0; i < 27; i++)
        v.push_back(vi);
    v[26].push_back(1);
    v[26].push_back(a.trans.size() + 1);
    final.trans.push_back(v);
    for (size_t i = 0; i < a.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            for (size_t k = 0; k < a.trans[i][j].size(); k++) {
                a.trans[i][j][k] += 1;
            }
        }
    }
    a.trans.back()[26].push_back(a.trans.size() + b.trans.size() + 1);
    final.trans.insert(final.trans.end(),a.trans.begin(),a.trans.end());
    for (size_t i = 0; i < b.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            for (size_t k = 0; k < b.trans[i][j].size(); k++) {
                b.trans[i][j][k] += offset;
            }
        }
    }
    b.trans.back()[26].push_back(a.trans.size() + b.trans.size() + 1);
    final.trans.insert(final.trans.end(),b.trans.begin(),b.trans.end());
    v[26].clear();
    final.trans.push_back(v);
    return final;
}

//Kleene star operation of NFA a*
nfa kleene(nfa a)
{
    nfa final;
    vector< vector<int> > v;
    vector<int> vi;
    for (size_t i = 0; i < 27; i++)
        v.push_back(vi);
    v[26].push_back(1);
    v[26].push_back(a.trans.size());
    final.trans.push_back(v);
    for (size_t i = 0; i < a.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            for (size_t k = 0; k < a.trans[i][j].size(); k++) {
                a.trans[i][j][k] += 1;
            }
        }
    }
    a.trans.back()[26].push_back(1);
    final.trans.insert(final.trans.end(),a.trans.begin(),a.trans.end());
    return final;
}

int main()
{
    nfa a = create(1);
    nfa b = create(2);
    nfa c = concat(a,b);
    for (size_t i = 0; i < c.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            printf("%d ", c.trans[i][j].size() ? c.trans[i][j][0] : 0);
        }
        printf("\n");
    }
}
