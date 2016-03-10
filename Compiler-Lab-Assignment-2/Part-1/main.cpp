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


    printf("NFA:\n");
    for (size_t i = 0; i < c.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            printf("%d ", c.trans[i][j].size() ? c.trans[i][j][0] : 0);
        }
        printf("\n");
    }
    write_nfa(c, input);


    vector<nfa_dfa_state> ans = nfa_to_dfa(c);
    write_dfa(ans, "dfa.txt", input);
    printf("DFA:\n");
    printf("States: %zu\n", ans.size());
    for (size_t i = 0; i < ans.size(); i++) {
        for (size_t j = 0; j < 26; j++) {
            printf("%d ", ans[i].trans[j] >= 0 ? ans[i].trans[j] : 0);
        }
        if (ans[i].isStart) {
            printf(" Start ");
        }
        if (ans[i].isFinal) {
            printf(" Final ");
        }
        printf("\n");
    }


    vector<nfa_dfa_state> final = optimize_dfa(ans);
    printf("Minimized DFA:\n");
    printf("No. of states: %zu\n", final.size());
    for (size_t i = 0; i < final.size(); i++) {
        for (size_t j = 0; j < 26; j++) {
            printf("%d ", final[i].trans[j] >= 0 ? final[i].trans[j] : 0);
        }
        if (final[i].isStart) {
            printf(" Start ");
        }
        if (final[i].isFinal) {
            printf(" Final ");
        }
        printf("\n");
    }
    write_dfa(final, "min_dfa.txt", input);

}
