#include <string>
#include <iostream>
#include <fstream>
#include <vector>

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
            outfile << i << ", ";
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

// void read_dfa()
// {
//     fstream infile;
//     infile.open("min_dfa.txt");
//     string input;
//     cin.getline(input);
//     printf("%s\n", input.c_str());
// }
