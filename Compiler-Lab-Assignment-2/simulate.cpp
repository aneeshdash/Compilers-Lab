#include <vector>
#include <cstring>
#include <cstdio>
#include "define.h"
#include "IO.cpp"
using namespace std;


void simulate_DFA(vector<nfa_dfa_state>& newDFA,char* input)
{
	int start,number;
	for (size_t i = 0; i < newDFA.size(); ++i)
	{
		if (newDFA[i].isStart)
		{
			start = i;
		}
	}
	//printf("%d\n",start );
	for(int i=0; i < strlen(input);i++)
	{


		number =int(input[i])-97;
		if(number < 0 || number > 25)
		{
			printf("alphabet error\n");
			exit(0);
		}

		int old = start;
		start = newDFA[start].trans[number];
		printf("Dfa moved from %d to %d on symbol %c\n", old,start,input[i]);
	}

	if(newDFA[start].isFinal)
	{
		printf("string accepted by DFA\n");
	}
	else
	{
		printf("string not accepted by DFA\n");
	}
}

int main()
{
    char input[1000];
    vector<nfa_dfa_state> sim_dfa = read_dfa();
    scanf("%s", input);
    simulate_DFA(sim_dfa, input);
}
