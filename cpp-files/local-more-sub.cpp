// libraries
#include <iostream>
#include <vector> //to use vector
#include <fstream>
#include <algorithm>
#include <string> //to use string
#include <random> //to use generator
#include <stdio.h>
#include <string.h>

using namespace std;

// --------------------------------------- global variables --------------------------------------
default_random_engine generator(10);

struct item {
    string sa;
    string sb;
    int score;
};

int MATCH = 2;
int MISSMATCH = -1;

// -----------------------------------------------------------------------------------------------

// --------------------------------------- functions signs ---------------------------------------
void read_file(int &len_sequence1, int &len_sequence2, string &sequence1, string &sequence2);
int generate_random_numbers(int min, int max);
string generate_subsequence(string b, int j, int k);
item run(string a, string b, int n, int m);
string step1(int n, string b, int m, int &k);
int step2(int k);
vector<string> step3(string a, int n, int p, int k);
vector<item> step4(vector<string> subsequences, int vector_size, string sb);
item step5(vector<item> results, int results_size);
int calculate_score(string sa, string sb);
// -----------------------------------------------------------------------------------------------

int main(){
    int len_sequence1 = 0; 
    int len_sequence2 = 0;
    string sequence1;
    string sequence2;
    read_file(len_sequence1, len_sequence2, sequence1, sequence2);
    item best;
    best.score = 0; //avoid memory trash
    for(int i=0; i<8; i++){
        item random = run(sequence1, sequence2, len_sequence1, len_sequence2);
        if (random.score > best.score) {
            best.sa = random.sa;
            best.sb = random.sb;
            best.score = random.score;
        };
    }
    cout << best.score;

    return 0;
}

item run(string a, string b, int n, int m){
    int k;
    string sb = step1(a.size(), b, m, k);
    int p = step2(k);
    vector<string> p_sa = step3(a, n, p, sb.size());
    vector<item> score_subsequences = step4(p_sa, p_sa.size(), sb);
    item best_alingment = step5(score_subsequences, score_subsequences.size());
    return best_alingment;
}

string step1(int n, string b, int m, int &k){
    k = generate_random_numbers((int)n/2, n);
    int j = generate_random_numbers(0, m-k);
    return generate_subsequence(b, j, k);
}

int step2(int k){
    return k*k*k;
}

vector<string> step3(string a, int n, int p, int k){
    vector<string> subsequences;
    for (int sub = 0; sub < p; sub++){
        int i = generate_random_numbers(0, n-k);
        string sa = generate_subsequence(a, i, k);
        subsequences.push_back(sa);
    }
    return subsequences;
}

vector<item> step4(vector<string> subsequences, int vector_size, string sb){
    vector<item> result;
    for (int i=0; i<vector_size; i++){
        int score = calculate_score(subsequences[i], sb);
        item sub_result = {sa: subsequences[i], sb: sb, score: score};
        result.push_back(sub_result);
    }
    return result;
}

item step5(vector<item> results, int results_size){
    item best;
    best.score = 0; //avoid memory trash
    for (int i=0; i<results_size; i++){
        item element = results[i];
        if(element.score > best.score){
            best.score = element.score;
            best.sa = element.sa;
            best.sb = element.sb;
        } 
    }
    return best;
}

int calculate_score(string sa, string sb){
    int score = 0;
    int k = sa.size();
    for(int i=0; i<k; i++){
        if(sa[i] == sb[i]) score += MATCH;
        else score += MISSMATCH;
    }
    return score;
}

void read_file(int &len_sequence1, int &len_sequence2, string &sequence1, string &sequence2){
    // description: read the input file, and change the variables values
    cin >> len_sequence1;
    cin >> len_sequence2;
    cin >> sequence1;
    cin >> sequence2;
}

string generate_subsequence(string b, int j, int k){
    // description: generates the subsequence of a given sequence b
    string result;
    for (int i=0; i<k; i++){
        result.push_back(b[j+i]);
    }
    return result;
}

int generate_random_numbers(int min, int max){
    // description: generate a random int number between min and max
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator); 
}