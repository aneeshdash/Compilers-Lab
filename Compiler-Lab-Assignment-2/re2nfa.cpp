#include <cstdio>
#include <vector>
#include <cstring>
#include <stack>
#include <iostream>
#include <fstream>
// #include "define.h"
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
//     nfa final;
//     vector< vector<int> > v;
//     vector<int> vi;
//     for (size_t i = 0; i < 27; i++)
//         v.push_back(vi);
//     v[26].push_back(1);
//     v[26].push_back(a.trans.size());
//     final.trans.push_back(v);
//     for (size_t i = 0; i < a.trans.size(); i++) {
//         for (size_t j = 0; j < 27; j++) {
//             for (size_t k = 0; k < a.trans[i][j].size(); k++) {
//                 a.trans[i][j][k] += 1;
//             }
//         }
//     }
    a.trans.front()[26].push_back(a.trans.size() - 1);
    a.trans.back()[26].push_back(0);
    // final.trans.insert(final.trans.end(),a.trans.begin(),a.trans.end());
    return a;
}

nfa re_to_nfa(char* re)
{
    nfa* d;
    //printf("%d\n",strlen(re));
    stack<char> operators;
    stack<nfa> operands;
    int i;
    for(i=0; i< strlen(re) ;i++)
    {
        switch(re[i])
        {
            case '(':
                operators.push('(');
                break;
            case ')':
                while(operators.top() != '(')
                {
                    if(operators.top() == '|')
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        nfa operand2 = operands.top();
                        operands.pop();
                        operands.push(unions(operand2,operand1));
                        operators.pop();
                    }
                    else if(operators.top() == '*')
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        operands.push(kleene(operand1));
                        operators.pop();

                    }
                    else
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        nfa operand2 = operands.top();
                        operands.pop();
                        operands.push(concat(operand2,operand1));
                        operators.pop();
                    }

                }
                operators.pop();
                break;
            case '*':
                operators.push('*');
                break;
            case '.':
                if(!operators.empty())
                {
                    while(operators.top() == '*')
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        operands.push(kleene(operand1));
                        operators.pop();
                        if(operators.empty())
                        {
                            break;
                        }
                    }
                }
                operators.push('.');

                break;
            case '|':
                if(!operators.empty())
                {
                    while(operators.top() == '*')
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        operands.push(kleene(operand1));
                        operators.pop();
                        if(operators.empty())
                        {
                            break;
                        }
                    }
                }
                if(!operators.empty())
                {
                        while(operators.top() == '.')
                    {
                        nfa operand1 = operands.top();
                        operands.pop();
                        nfa operand2 = operands.top();
                        operands.pop();
                        operands.push(concat(operand2,operand1));
                        operators.pop();
                        if(operators.empty())
                        {
                            break;
                        }
                    }
                }

                operators.push('|');
                break;
            default:
                operands.push(create(int(re[i])-97));
                break;
        }
    }

    while(!operators.empty())
    {
        if(operators.top() == '|')
        {
            nfa operand1 = operands.top();
            operands.pop();
            nfa operand2 = operands.top();
            operands.pop();
            operands.push(unions(operand2,operand1));
            operators.pop();
        }
        else if(operators.top() == '*')
        {
            nfa operand1 = operands.top();
            operands.pop();
            operands.push(kleene(operand1));
            operators.pop();

        }
        else
        {
            nfa operand1 = operands.top();
            operands.pop();
            nfa operand2 = operands.top();
            operands.pop();
            operands.push(concat(operand2,operand1));
            operators.pop();
        }

    }
    //nfa e;
    //return e;
    return operands.top();
}

// void write_nfa(nfa input)
// {
//     ofstream outfile;
//     outfile.open("nfa.txt");
//     outfile << input.trans.size() << endl;
//     for (size_t i = 0; i < input.trans.size(); i++) {
//         for (size_t j = 0; j < 27; j++) {
//             for (size_t k = 0; k < input[i][j].size(); k++) {
//                 outfile << i << "   " << j << "   " << input[i][j][k] << endl;
//             }
//         }
//     }
//     outfile.close();
// }

// int main()
// {
//     char input[1000];
//
//     scanf("%s",&input);
//     nfa c =re_to_nfa(input);
//     nfa a = create(1);
//     nfa b = create(2);
//     nfa c = concat(a,b);
//     for (size_t i = 0; i < c.trans.size(); i++) {
//         for (size_t j = 0; j < 27; j++) {
//             printf("%d ", c.trans[i][j].size() ? c.trans[i][j][0] : 0);
//         }
//         printf("\n");
//     }
// }
