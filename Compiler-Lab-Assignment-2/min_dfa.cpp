#include <vector>

vector<nfa_dfa_state> optimize_dfa(vector<nfa_dfa_state> input)
{
    int s_state = input.size(), f_state;
    do {
        /* code */
    } while(s_state == f_state);
}

vector<nfa_dfa_state> initialize_dfa(vector<nfa_dfa_state> input)
{
    vector<nfa_dfa_state> output;
    nfa_dfa_state non_final, final;
    final.isFinal = 1;
    int states = input.size();
    vector<bool> nf(states,0), f(states,0);
    for (size_t i = 0; i < states; i++) {
        if(input.isFinal)
            f[i] = 1;
        else
            nf[i] = 1;
    }
    non_final.state = nf;
    final.state = f;
}

vector<nfa_dfa_state> split_dfa(vector<nfa_dfa_state> newDFA, vector<nfa_dfa_state> oldDFA, int state, int symbol)
{
    vector<int> state_trans;
    for(size_t i = 0;i < oldDFA.size(); i++)
    {
        if(newDFA[state].state[i]) {
            int n_state = oldDFA[i].trans[symbol];
            int state_2 = getSubset(newDFA, n_state);
            for (size_t j = 0; j < state_trans.size(); j++) {
                /* code */
            }
        }
    }
}

int getSubset (vector<nfa_dfa_state> input, int state)
{
    for (size_t i = 0; i < input.size(); i++) {
        if(input[i].state[state])
            return i;
    }
}
