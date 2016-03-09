#include <cstdio>
#include <vector>
#include <string>
using namespace std;

void epsilon_closure(nfa a, int state, vector<bool>& closure)
{
    if (! closure[state]) {
        closure[state] = 1;
        for (size_t i = 0; i < a.trans[state][26].size(); i++) {
            epsilon_closure(a, a.trans[state][26][i], closure);
        }
    }
    return;
}

void epsilon_closure(nfa a, vector<bool>& closure) {
    int states = closure.size();
    vector<bool> closure_1(states);
    for (size_t i = 0; i < states; i++) {
        if(closure[i])
            epsilon_closure(a, i, closure_1);
    }

    for (size_t i = 0; i < states; i++) {
        closure[i] = closure_1[i];
    }
    return;
}

vector<nfa_dfa_state> nfa_to_dfa(nfa input)
{
    vector<nfa_dfa_state> final;
    int states = input.trans.size();
    vector<bool> init_closure(states);
    vector<bool> zeroes(states,0);
    init_closure[0] = 1;
    epsilon_closure(input, init_closure);
    for (size_t i = 0; i < states; i++) {
        printf("%d ", static_cast<int>(init_closure[i]));
    }
    printf("\n");
    nfa_dfa_state initial;
    initial.state = init_closure;
    initial.isStart = 1;
    if (init_closure[states - 1] == 1)
        initial.isFinal = 1;
    final.push_back(initial);
    // vector<nfa_dfa_state>::iterator it = final.begin();
    // while (it != final.end())
    for (size_t it = 0; it < final.size(); it++)
    {
        // iterate for all input characters
        for (size_t j = 0; j < 26; j++) {
            vector<bool> closure(states,0);
            //iterate over all states for input transition
            for (size_t i = 0; i < states; i++) {
                if((final[it]).state[i] == 1 && input.trans[i][j].size()) {
                    //add states to closure
                    for (size_t k = 0; k < input.trans[i][j].size(); k++) {
                        closure[input.trans[i][j][k]] = 1;
                    }
                }
            }
            epsilon_closure(input, closure);
            for (size_t i = 0; i < final.size(); i++) {
                if(final[i].state == closure) {
                    (final[it]).trans[j] = i;
                    break;
                }
            }
            if( (final[it]).trans[j] < 0 && closure != zeroes) {
                nfa_dfa_state a;
                a.state = closure;
                if (closure[states - 1] == 1)
                    a.isFinal = 1;
                final[it].trans[j] = final.size();
                final.push_back(a);
                for (size_t i = 0; i < states; i++) {
                    printf("%d ", static_cast<int>(closure[i]));
                }
                printf("\n");
            }
        }
    }
    return final;
}
