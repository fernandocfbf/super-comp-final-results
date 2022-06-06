// libraries
#include <random>
#include <iostream>
#include <vector> //to use vector
#include <string> //to use string
#include <algorithm> // max
#include<bits/stdc++.h>
#include <cmath>
#include <omp.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/copy.h>
#include <thrust/host_vector.h>

using namespace std;

#define MATCH 2
#define MISS -1
#define GAP  -1
#define BATCHSIZE 500000
#define MEM 500000

//globals
struct item {
    int score;
    vector<char> s1;
    vector<char> s2;
};
struct s_temp
{
    thrust::device_ptr<char> d_s;
    char letter_t;
    thrust::device_ptr<int> store_list_0;
    s_temp(
        thrust::device_ptr<char> d_s_,
        char letter_t_,
        thrust::device_ptr<int> store_list_0_) : 
        d_s(d_s_),
        letter_t(letter_t_),
        store_list_0(store_list_0_) {};
    __host__ __device__
    int operator() (const int(&j))
    {
        int w;
        if (d_s[j] == letter_t) {
            w = MATCH;
        }
        else {
            w = MISS;
        }

        int diagonal = store_list_0[j-1] + w;
        int insertion = store_list_0[j];
        int max = 0;
        if (diagonal > max) max = diagonal;
        if (insertion > max) max = insertion;
        return max;
    }
};

//functions sign
void generate_subsequence(vector<char> input, vector<vector<char>> &all);
int get_max_score(vector<vector<char>> &a_all, vector<vector<char>> &b_all);
int compute_score(vector<item> &batch_vector);
vector<item> slicer(vector<item> &arr, int X, int Y);

// functions code
vector<item> slicer(vector<item> &v, int m, int n){
    std::vector<item> vec(n - m + 1);
    std::copy(v.begin() + m, v.begin() + n + 1, vec.begin());
    return vec;
}
void generate_subsequence(string input, vector<vector<char>> &all){
    /*
    vector<char> sub_char(input.begin(), input.end());
    all.push_back(sub_char);
    */
    
    for (int j=0; j<(int)input.size(); j++){
        for(int i=j; i<(int)input.size(); i++){
            string sub = input.substr(i,j);
            vector<char> sub_char(sub.begin(), sub.end());
            if(sub_char.size() > 0){
                all.push_back(sub_char);
            }
        }
    }
    
}
int compute_score(vector<item> &batch_vector){
    int maximum_batch = -1;
    int i = 0;
    for (auto& pair_sub : batch_vector){
        i+=1;
        vector<char> S = pair_sub.s1;
        vector<char> T = pair_sub.s2;
        int N = (int)S.size();
        int M = (int)T.size();

        thrust::device_vector<int> store_list[2]; //store previous and current
        store_list[0].resize(N+1); //previous line
        store_list[1].resize(N+1); //current line

        thrust::fill(store_list[0].begin(), store_list[0].end(), 0);
        thrust::device_vector<char> d_S(N);
        thrust::copy(S.begin(), S.begin()+N, d_S.begin());

        thrust::counting_iterator<int> c0(1);
        thrust::counting_iterator<int> c1(M+1);

        for (int j=0; j<M; j++){
            char t_letter = T[j];

            //up and diagonal
            thrust::transform(c0, c1, store_list[1].begin()+1, s_temp(d_S.data(), t_letter, store_list[0].data()));
            
            //left side
            thrust::inclusive_scan(store_list[1].begin()+1, store_list[1].end(), store_list[0].begin()+1, thrust::maximum<int>());
        }
        int result_batch = (int)store_list[1].data()[store_list[1].size()-1];
        if (result_batch > maximum_batch){
            maximum_batch = result_batch;
        }
    }
    return maximum_batch;
}
int get_max_score(vector<vector<char>> &a_all, vector<vector<char>> &b_all){
    vector<item> subseq_pairs;
    int best_result_batch = -1;
    int best_result = -1;
    int score_computed = -1;
    int filled_spaces = 0;
    int computed = 0;

    for (auto& sub_a: a_all){        
        for (auto& sub_b: b_all){
            subseq_pairs.push_back({0, sub_a, sub_b});
            filled_spaces+=1;
            if (filled_spaces == MEM){
                #pragma omp parallel for reduction(max:best_result_batch) shared(computed)
                for (int i=0; i < subseq_pairs.size(); i+=BATCHSIZE){

                    vector<item> batch;

                    if (i + BATCHSIZE > (int)subseq_pairs.size()){
                        batch = slicer(subseq_pairs, i, (int)subseq_pairs.size()-1);
                    }
                    else{
                        batch = slicer(subseq_pairs, i, i+BATCHSIZE);
                    }
                    score_computed = compute_score(batch);
                    if (score_computed > best_result_batch){
                        best_result_batch = score_computed;
                    }
                    computed += BATCHSIZE;
                    //cout << "Computed " << i << "/" << subseq_pairs.size() << endl;
                    
                }
                if (best_result_batch > best_result){
                        best_result = best_result_batch;
                }
                filled_spaces = 0;
                subseq_pairs.clear();
            }
        }
    }
    #pragma omp parallel for reduction(max:best_result_batch)
    for (int i=0; i < subseq_pairs.size(); i+=BATCHSIZE){
        vector<item> batch;
        if (i + BATCHSIZE > (int)subseq_pairs.size()){
            batch = slicer(subseq_pairs, i, (int)subseq_pairs.size()-1);
        }
        else{
            batch = slicer(subseq_pairs, i, i+BATCHSIZE);
        }
        score_computed = compute_score(batch);
        if (score_computed > best_result_batch){
            best_result_batch = score_computed;
        }
        computed += BATCHSIZE;
        //cout << "Computed " << i << "/" << subseq_pairs.size() << endl;
    }
    if (best_result_batch > best_result){
            best_result = best_result_batch;
    }
    
    return best_result;
}

int main(){
    string S, T;
    int N, M;
    vector<vector<char>> get_all1, get_all2;

    cin >> N;
    cin >> M;
    cin >> S;
    cin >> T;

    generate_subsequence(S, get_all1);
    generate_subsequence(T, get_all2);

    int score_max = get_max_score(get_all1, get_all2);
    cout << score_max << endl;
    return 0;
}