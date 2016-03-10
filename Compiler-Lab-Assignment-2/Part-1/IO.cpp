#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

void write_nfa(nfa input, char regex[])
{
    ofstream outfile;
    outfile.open("nfa.txt");
    outfile << "Regular Expression: " << regex << endl;
    outfile << "No. of states: " << input.trans.size() << endl;
    outfile << "Initial State: " << 0 << endl;
    outfile << "Final State: " << input.trans.size() - 1 << endl;
    for (size_t i = 0; i < input.trans.size(); i++) {
        for (size_t j = 0; j < 27; j++) {
            for (size_t k = 0; k < input.trans[i][j].size(); k++) {
                char c = j<26 ? (j+97) : 69;
                outfile << i << "   " << c << "   " << input.trans[i][j][k] << endl;
            }
        }
    }
    outfile.close();
}


void write_dfa(vector<nfa_dfa_state> input, string name, char regex[])
{
    ofstream outfile;
    outfile.open(name.c_str());
    outfile << "Regular Expression: " << regex << endl;
    outfile << "No. of states: " << input.size() << endl;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i].isStart) {
            outfile << "Initial State: " << i << endl;
        }
    }
    outfile << "Final States: ";
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i].isFinal) {
            outfile << i << ",";
        }
    }
    outfile << endl;
    for (size_t i = 0; i < input.size(); i++) {
        for (size_t j = 0; j < 26; j++) {
            if(input[i].trans[j] != -1) {
                char c = j+97;
                outfile << i << "   " << c << "   " << input[i].trans[j] << endl;
            }
        }
    }
    outfile.close();
}

vector<nfa_dfa_state> read_dfa()
{
    fstream infile;
    string input;
    int states, initial_state,i,offset,state1, state2;
    vector<int> final_states;
    vector<nfa_dfa_state> final;
    infile.open("min_dfa.txt");
    getline(infile, input);
    printf("%s\n", input.c_str());
    getline(infile, input);
    input = input.substr(input.find(":")+2);
    states = stoi(input);
    printf("No. of States: %d\n", states);
    getline(infile, input);
    input = input.substr(input.find(":")+2);
    initial_state = stoi(input);
    getline(infile, input);
    input = input.substr(input.find(":")+2);
    stringstream ss(input);
    while (ss >> i)
    {
        final_states.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
    for (size_t i = 0; i < states; i++) {
        nfa_dfa_state a;
        for (size_t j = 0; j < 26; j++) {
            getline(infile, input);
            offset = input.find("   ");
            state1 = stoi(input.substr(0,offset));
            state2 = stoi(input.substr(offset + 7));
            a.trans[j] = state2;
        }
        final.push_back(a);
    }
    final[initial_state].isStart = 1;
    for (size_t i = 0; i < final_states.size(); i++) {
        final[final_states[i]].isFinal = 1;
    }
    return final;
}
