#include <cstdio>
#include "define.h"
#include "min_dfa.cpp"
#include "re2nfa.cpp"
#include "nfa2dfa.cpp"
#include "IO.cpp"
using namespace std;

int main()
{
    char input[1000];
    scanf("%s",input);
    nfa c =re_to_nfa(input);
    // nfa a = create(1);
    // nfa b = create(2);
    // nfa c = kleene(a);
    for (size_t i = 0; i < c.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            printf("%d ", c.trans[i][j].size() ? c.trans[i][j][0] : 0);
        }
        printf("\n");
    }
    write_nfa(c, input);
    vector<nfa_dfa_state> ans = nfa_to_dfa(c);
    write_dfa(ans, "dfa.txt", input);
    printf("States: %zu\n", ans.size());
    for (size_t i = 0; i < ans.size(); i++) {
        for (size_t j = 0; j < 26; j++) {
            printf("%d ", ans[i].trans[j] >= 0 ? ans[i].trans[j] : 0);
        }
        printf("\n");
    }
    vector<nfa_dfa_state> final = optimize_dfa(ans);
    write_dfa(final, "min_dfa.txt", input);
    // read_dfa();
    // scanf("%s", input);
    // simulate_DFA(final, input);
}
