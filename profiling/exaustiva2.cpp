// libraries
#include <random>
#include <iostream>
#include <vector> //to use vector
#include <string> //to use string
#include <algorithm> // max
#include<bits/stdc++.h>
#include <cmath>

#define MATCH 2
#define MISS -1
#define GAP -1

struct item {
    int value;
    int x;
    int y;
};

// typedefs
typedef std::vector<std::vector<item>> matriz;

using namespace std;

struct result {
    int score;
    string s1;
    string s2;
};

//functions
matriz initialize_matriz(int n, int m);
void calculate_score(matriz &sequencies, string &s1, string &s2, int i, int j);
int compare_letters(matriz &sequencies, string &s1, string &s2, int i, int j);
item compute_score(matriz &filled_matriz, int n, int m);
void gps(item &i, int x, int y, int up, int left, int diagonal);
void traceback(item init, matriz &m, string &seq1, string &seq2, string &seq1_aligned, string &seq2_aligned);
void run_p1(string &sequence1, string &sequence2, string &sequence1_aligned, string &sequence2_aligned, int len_sequence1, int len_sequence2, item &score);
void generate_subsequence(string input, string sub_sequence, vector<string> &all);
result compute_score_for_all(vector<string> a_all, vector<string> b_all);
item calculate_score_trucate(string sa, string sb);

void run_p1(string &sequence1, string &sequence2, string &sequence1_aligned, string &sequence2_aligned, int len_sequence1, int len_sequence2, item &score){
    matriz dna = initialize_matriz(len_sequence1, len_sequence2);
    for (int i=1; i <= len_sequence1; i++){
        for (int j=1; j<= len_sequence2; j++){
            calculate_score(dna, sequence1, sequence2, i, j);
        }
    }
    score = compute_score(dna, len_sequence1, len_sequence2);
    traceback(score, dna, sequence1, sequence2, sequence1_aligned, sequence2_aligned);
    return;
}

matriz initialize_matriz(int n, int m){
    /*
    input: number of lines and columns (int)
    output: matriz variables [n, m]
    description: initialize a zero matriz variable
    */
    matriz init;
    for (int i=0; i<=n; i++){
        std::vector<item> line;
        for(int j=0; j<=m; j++){
            item e = {value: 0, x: -1, y: -1};
            line.push_back(e);
        }
        init.push_back(line);
    }
    return init;
}

item calculate_score_trucate(string sa, string sb){
    item score;
    score.value = 0;
    int k = sa.size();
    for(int i=0; i<k; i++){
        if(sa[i] == sb[i]) score.value += MATCH;
        else score.value += MISS;
    }
    return score;
}

void calculate_score(matriz &sequencies, string &s1, string &s2, int i, int j){
    /*
    input: matriz [n,m], sequence1 and sequence2 (string) ,line (int), column (int)
    output: 
    description: calculates the score for a specific line and column
    */
    int diagonal = compare_letters(sequencies, s1, s2, i, j);
    int delection = sequencies[i-1][j].value-1;
    int inserction = sequencies[i][j-1].value-1;
    item new_item;
    new_item.value = max({0, diagonal, delection, inserction});
    gps(new_item, j, i, delection, inserction, diagonal);
    sequencies[i][j] = new_item;
    return;
}

int compare_letters(matriz &sequencies, string &s1, string &s2, int i, int j){
    /*
    input: matriz [n,m], sequence1 and sequence2 (string) ,line (int), column (int)
    output: result (int)
    decription: calculates the diagonal for a specific line (i) and column (j) on the matriz (sequencies)
    */
    int diagonal1 = sequencies[i-1][j-1].value+MATCH; //match
    int diagonal2 = sequencies[i-1][j-1].value+MISS; //mismatch
    int diagonal3 = sequencies[i-1][j-1].value+GAP; //gap
    
    if((s1[i-1] == s2[j-1]) && (i-1 != (int)s1.size() && j-1 != (int)s2.size())){
        return max({diagonal1, diagonal2, diagonal3});
    }
    return max({diagonal2, diagonal3});
}

item compute_score(matriz &filled_matriz, int n, int m){
    /*
    input: filled matriz [n, m]
    output: result (int)
    description: get the max score from the filled matriz 
    */
   item max_score = {value: 0, x: 0, y: 0};
   for(int i=1; i<=n; i++){
       for (int j=1; j<=m; j++){
           if (filled_matriz[i][j].value > max_score.value){
               max_score.value = filled_matriz[i][j].value;
               max_score.x = j; 
               max_score.y = i; 
           }
       }
   }
   return max_score;
}

void gps(item &i, int x, int y, int up, int left, int diagonal){
    /*
    input: struct (item), current position x and y, diagonal, up and left values
    output:
    description: gets the best way to traceback
    */
    if(diagonal >= up && diagonal >= left ){
        i.x = x-1;
        i.y = y-1;
    }
    else if (up > diagonal && up > left){
        i.x = x;
        i.y = y-1;
    }
    else{
        i.x = x-1;
        i.y = y;
    }
}

void traceback(item init, matriz &m, string &seq1, string &seq2, string &seq1_aligned, string &seq2_aligned){
    /*
    
    input: initial position (init), filled matriz [n,m] (m), both sequencies
    output:
    description: calculates the traceback for the given matriz. Print the sequencies alignment
    */
    item current_item = init;
    item next_item = m[current_item.y][current_item.x];
        
    while((current_item.x> 0 && current_item.y> 0) && next_item.value != 0){
        //jump diagonal
        if(next_item.y < current_item.y && next_item.x < current_item.x){
            seq1_aligned.push_back(seq1[current_item.y-1]);
            seq2_aligned.push_back(seq2[current_item.x-1]);
        }
        //jump vertical
        else if(next_item.y < current_item.y){     
            seq1_aligned.push_back(seq1[current_item.y-1]);
            seq2_aligned.push_back('-');
        }
        //jump horizontal
        else if(next_item.x < current_item.x){
            seq1_aligned.push_back('-');
            seq2_aligned.push_back(seq2[current_item.x-1]);
        }
        
        current_item = next_item;
        next_item = m[current_item.y][current_item.x];
    }

    reverse(seq1_aligned.begin(),seq1_aligned.end());
    reverse(seq2_aligned.begin(),seq2_aligned.end());      
}

void generate_subsequence(string input, string sub_sequence, vector<string> &all){
    for (int j=0; j<(int)input.size(); j++){
        for(int i=0; i<(int)input.size(); i++){
            string sub = input.substr(i,j);
            all.push_back(sub);
        }
    }
}

result compute_score_for_all(vector<string> a_all, vector<string> b_all){
    result best_alignment;
    string s1_aligned, s2_aligned;
    item score;

    score.value = 0;
    best_alignment.score = 0;

    for (auto& sub_a: a_all){
        for (auto& sub_b: b_all){

            if (sub_a.size() == sub_b.size()) score = calculate_score_trucate(sub_a, sub_b);
            else run_p1(sub_a, sub_b, s1_aligned, s2_aligned, (int)sub_a.size(), (int)sub_b.size(), score);

            if (score.value > best_alignment.score){
                best_alignment.score = score.value;
                best_alignment.s1 = sub_a;
                best_alignment.s2 = sub_b;
            }
        }
    }
    return best_alignment;
}

int main(){
    int s1_len, s2_len;
    string s1, s2, out1, out2;
    vector<string> get_all1, get_all2;

    cin >> s1_len;
    cin >> s2_len;
    cin >> s1; 
    cin >> s2;

    generate_subsequence(s1, out1, get_all1);
    generate_subsequence(s2, out2, get_all2);
    result best = compute_score_for_all(get_all1, get_all2);

    cout << best.score << endl;
    return 0;
}