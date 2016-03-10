#include <vector>
#include <cstring>
void add_dummy(vector<nfa_dfa_state>& oldDFA)
{
    nfa_dfa_state dummy;
    int dummy_state = oldDFA.size();
    for (size_t i = 0; i < 26; i++) {
        dummy.trans[i] = dummy_state;
    }
    for (size_t i = 0; i < oldDFA.size(); i++) {
        for (size_t j = 0; j < 26; j++) {
            if(oldDFA[i].trans[j] == -1)
                oldDFA[i].trans[j] = dummy_state;
        }
    }
    oldDFA.push_back(dummy);
}

vector<nfa_dfa_state> initialize_dfa(vector<nfa_dfa_state> input)
{
    vector<nfa_dfa_state> output;
    nfa_dfa_state non_final, final;
    final.isFinal = 1;
    int states = input.size();
    vector<bool> nf(states,0), f(states,0);
    for (size_t i = 0; i < states; i++) {
        if(input[i].isFinal)
            f[i] = 1;
        else
            nf[i] = 1;
    }
    non_final.state = nf;
    final.state = f;
    output.push_back(non_final);
    output.push_back(final);
    return output;
}

int getSubset (vector<nfa_dfa_state>& input, int state)
{
    for (size_t i = 0; i < input.size(); i++) {
        if(input[i].state[state])
            return i;
    }
}

void split_dfa(vector<nfa_dfa_state>& newDFA, vector<nfa_dfa_state>& oldDFA, int state, int symbol)
{
    vector<int> state_list;
    vector<int> trans;
    for(size_t i = 0;i < oldDFA.size(); i++)
    {
        if(newDFA[state].state[i]) {
            int n_state = oldDFA[i].trans[symbol];
            int trans_2 = getSubset(newDFA, n_state);
            for (size_t j = 0; j < trans.size(); j++) {
                if(trans_2 == trans[j]) {
                    newDFA[state_list[j]].state[i] = 1;
                    newDFA[state].state[i] = 0;
                    break;
                }
            }
            if(newDFA[state].state[i]) {
                nfa_dfa_state a;
                for (size_t k = 0; k < oldDFA.size(); k++) {
                    a.state.push_back(0);
                }
                a.state[i] = 1;
                state_list.push_back(newDFA.size());
                trans.push_back(trans_2);
                newDFA.push_back(a);
                newDFA[state].state[i] = 0;
            }
        }
    }
    newDFA.erase(newDFA.begin()+state);
}

void add_transitions(vector<nfa_dfa_state>& newDFA, vector<nfa_dfa_state>& oldDFA)
{
	for (size_t i = 0; i < newDFA.size(); ++i)
	{
		int k = -1;
		for(size_t l=0 ; l < oldDFA.size() ; l++)
		{
			if(newDFA[i].state[l]){
				k=l;
				break;
			}
		}

		for (int j = 0; j < 26; ++j)
		{
			newDFA[i].trans[j] = getSubset(newDFA,oldDFA[k].trans[j]);
		}
	}
}

void add_final(vector<nfa_dfa_state>& newDFA, vector<nfa_dfa_state>& oldDFA)
{
	for(size_t i =0 ;i< oldDFA.size() ;i++)
	{
		if(oldDFA[i].isFinal)
		{
			int k = getSubset(newDFA,i);
			newDFA[k].isFinal = 1;
			//printf("%d\n",k);
		}
	}
}


vector<nfa_dfa_state> optimize_dfa(vector<nfa_dfa_state> oldDFA)
{
    add_dummy(oldDFA);
    vector<nfa_dfa_state> newDFA = initialize_dfa(oldDFA);
    int s_state, flag;
    do {
        flag = 0;
        s_state = newDFA.size();
        for (size_t i = 0; i < newDFA.size(); i++) {
            for (size_t j = 0; j < 26; j++) {
                int check = -1;
                for (size_t k = 0; k < oldDFA.size(); k++) {
                    if(newDFA[i].state[k]) {
                        // printf("%zu %zu %d\n",k, j, oldDFA[k].trans[j] );
                        int sub = getSubset(newDFA, oldDFA[k].trans[j]);
                        if(check == -1) {
                            check = sub;
                        }
                        else if(check != sub ) {
                            split_dfa(newDFA, oldDFA, i, j);
                            flag = 1;
                            break;
                        }
                    }
                }
                if(flag)
                    break;
            }
            if(flag)
                break;
        }
    } while(s_state != newDFA.size());
    add_transitions(newDFA,oldDFA);
    add_final(newDFA,oldDFA);
    int start = getSubset(newDFA ,0);
    newDFA[start].isStart = 1 ;
    return newDFA;
}
