#define MATCH 2
#define MISMATCH -1
#define GAP 1
#define NUMBER 100
#define LENGTH 128

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
//#include <windows.h>
#include <fstream>
#include "../lib/tm_usage.h"
using namespace std;

// struct SetColor {
//     int n;
//     SetColor(int n) : n(n) {}
// };

// std::ostream& operator<<(std::ostream& os, const SetColor& sc) {
//     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), sc.n);
//     return os;
// }

int max4(int a, int b, int c, int d){
    if(d > a && d > b && d > c)
        return d;
    else if(a > b && a > c)
        return a;
    else if(b > c)
        return b;
    else
        return c;
}

int min(int a, size_t b){
    return a < b ? a : b;
}

struct arrangement{
    int score;
    vector<char> trackback_R;
    vector<char> trackback_Q;
};


arrangement SmithWaterman(vector<char> R, vector<char> Q){

    arrangement answer;
    vector<vector <int> > DP(Q.size());
    int leftup, left, up;
    for(int i=0;i<Q.size();i++)
        for(int j=0;j<R.size();j++)
            DP[i].push_back(0);

    for(int i=1;i<Q.size();i++)
        for(int j=1;j<DP[i].size();j++){
            if(Q[i] == R[j])
                leftup = DP[i-1][j-1] + MATCH;
            else
                leftup = DP[i-1][j-1] + MISMATCH;
            
            left = DP[i][j-1] - GAP;
            up = DP[i-1][j] - GAP;
            DP[i][j] = max4(0, leftup, left, up);
        }

    // cout << "--------------------" << endl;
    // for(int i=1;i<Q.size();i++){
    //     for(int j=1;j<DP[i].size();j++){
    //         if(DP[i][j])
    //             cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
    //         else
    //             cout << setw(3) << DP[i][j];
    //     }
    //     cout << endl;
    // }
    // cout << "--------------------" << endl;

    // trackback
    int i = Q.size()-1;
    int j = R.size()-1;
    while(i >= 1 && j >= 1){
        if(Q[i] == R[j]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back(R[j]);
            i--;
            j--;
        }
        else if(DP[i-1][j] >= DP[i][j-1]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back('-');
            i--;
        }
        else{
            answer.trackback_Q.push_back('-');
            answer.trackback_R.push_back(R[j]);
            j--;
        }
    }
    while(i>0){
        answer.trackback_Q.push_back(Q[i]);
        i--;
    }
    while(j>0){
        answer.trackback_R.push_back(R[j]);
        j--;
    }   

    reverse(answer.trackback_Q.begin(), answer.trackback_Q.end());
    reverse(answer.trackback_R.begin(), answer.trackback_R.end());

    int R_large = 0;
    int Q_large = 0;
    if(answer.trackback_Q.size() - answer.trackback_R.size() > 0)
        Q_large = answer.trackback_Q.size() - answer.trackback_R.size();
    for(int i=0;i<Q_large;i++)
        answer.trackback_R.push_back('-');

    if(answer.trackback_R.size() - answer.trackback_Q.size() > 0)
        R_large = answer.trackback_R.size() - answer.trackback_Q.size();
    for(int i=0;i<R_large;i++)
        answer.trackback_Q.push_back('-');

    answer.score = DP[Q.size()-1][R.size()-1];

    return answer;
}

arrangement BSW(vector<char> R, vector<char> Q, int bandwidth){

    arrangement answer;
    vector<vector <int> > DP(Q.size());
    int leftup, left, up;
    for(int i=0;i<Q.size();i++)
        for(int j=0;j<R.size();j++)
            DP[i].push_back(0);

    for(int i=1;i<Q.size();i++){
        int start = max(1 , i - bandwidth/2 + 1);
        int end = min(i + (bandwidth+1)/2  + 1, DP[i].size());
        for(int j=start;j<end;j++){
            if(Q[i] == R[j])
                leftup = DP[i-1][j-1] + MATCH;
            else
                leftup = DP[i-1][j-1] + MISMATCH;
            
            left = DP[i][j-1] - GAP;
            up = DP[i-1][j] - GAP;
            DP[i][j] = max4(0, leftup, left, up);
        }
    }
        

    // cout << "--------------------" << endl;
    // for(int i=1;i<Q.size();i++){
    //     for(int j=1;j<DP[i].size();j++){
    //         if(DP[i][j])
    //             cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
    //         else
    //             cout << setw(3) << DP[i][j];
    //     }
    //     cout << endl;
    // }
    // ;
    // cout << "-------------------" << endl;

    // trackback
    int i = Q.size()-1;
    int j = R.size()-1;
    while(i >= 1 && j >= 1){
        if(Q[i] == R[j]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back(R[j]);
            i--;
            j--;
        }
        else if(DP[i-1][j] >= DP[i][j-1]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back('-');
            i--;
        }
        else{
            answer.trackback_Q.push_back('-');
            answer.trackback_R.push_back(R[j]);
            j--;
        }
    }
    while(i>0){
        answer.trackback_Q.push_back(Q[i]);
        i--;
    }
    while(j>0){
        answer.trackback_R.push_back(R[j]);
        j--;
    }   

    reverse(answer.trackback_Q.begin(), answer.trackback_Q.end());
    reverse(answer.trackback_R.begin(), answer.trackback_R.end());

    int R_large = 0;
    int Q_large = 0;
    if(answer.trackback_Q.size() - answer.trackback_R.size() > 0)
        Q_large = answer.trackback_Q.size() - answer.trackback_R.size();
    for(int i=0;i<Q_large;i++)
        answer.trackback_R.push_back('-');

    if(answer.trackback_R.size() - answer.trackback_Q.size() > 0)
        R_large = answer.trackback_R.size() - answer.trackback_Q.size();
    for(int i=0;i<R_large;i++)
        answer.trackback_Q.push_back('-');

    answer.score = DP[Q.size()-1][R.size()-1];

    return answer;
}

arrangement GACT_X(vector<char> R, vector<char> Q, int X){

    int current_max = 0;
    arrangement answer;
    vector<vector <int> > DP(Q.size());
    int leftup, left, up;
    for(int i=0;i<Q.size();i++)
        for(int j=0;j<R.size();j++)
            DP[i].push_back(0);

    for(int i=1;i<Q.size();i++){
        bool flag = 0;
        for(int j=1;j<DP[i].size();j++){
            if(Q[i] == R[j])
                leftup = DP[i-1][j-1] + MATCH;
            else
                leftup = DP[i-1][j-1] + MISMATCH;
            
            left = DP[i][j-1] - GAP;
            up = DP[i-1][j] - GAP;
            DP[i][j] = max4(leftup, leftup, left, up);

            if(DP[i][j] >= current_max - X && !flag){
                //cout << "i = " << i << " flag = " << 1 << " at " << j <<endl;
                flag = 1;
            }
                
            if(DP[i][j] < current_max - X && flag){
                //cout << "i = " << i << " break at " << j <<endl;
                break;  
            }
            
            current_max = max(current_max , DP[i][j]);
        }
        
    }
        

    // cout << "--------------------" << endl;
    // for(int i=1;i<Q.size();i++){
    //     for(int j=1;j<DP[i].size();j++){
    //         if(DP[i][j] > 0)
    //             cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
    //         else if(DP[i][j] < 0)
    //             cout << setw(3) << SetColor(12) << DP[i][j] << SetColor(7);
    //         else
    //             cout << setw(3) << DP[i][j];
    //     }
    //     cout << endl;
    // }
    // cout << "--------------------" << endl;

    //trackback
    int i = Q.size()-1;
    int j = R.size()-1;
    while(i >= 1 && j >= 1){
        if(Q[i] == R[j]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back(R[j]);
            i--;
            j--;
        }
        else if(DP[i-1][j] >= DP[i][j-1]){
            answer.trackback_Q.push_back(Q[i]);
            answer.trackback_R.push_back('-');
            i--;
        }
        else{
            answer.trackback_Q.push_back('-');
            answer.trackback_R.push_back(R[j]);
            j--;
        }
    }
    while(i>0){
        answer.trackback_Q.push_back(Q[i]);
        i--;
    }
    while(j>0){
        answer.trackback_R.push_back(R[j]);
        j--;
    }   

    reverse(answer.trackback_Q.begin(), answer.trackback_Q.end());
    reverse(answer.trackback_R.begin(), answer.trackback_R.end());

    int R_large = 0;
    int Q_large = 0;
    if(answer.trackback_Q.size() - answer.trackback_R.size() > 0)
        Q_large = answer.trackback_Q.size() - answer.trackback_R.size();
    for(int i=0;i<Q_large;i++)
        answer.trackback_R.push_back('-');

    if(answer.trackback_R.size() - answer.trackback_Q.size() > 0)
        R_large = answer.trackback_R.size() - answer.trackback_Q.size();
    for(int i=0;i<R_large;i++)
        answer.trackback_Q.push_back('-');

    answer.score = DP[Q.size()-1][R.size()-1];

    return answer;
}

int main(int argc, char* argv[]){

    CommonNs::TmUsage tmusg;
	CommonNs::TmStat SW_time, BSW_time, GACT_X_time;
	fstream fin(argv[1]);
    fstream fout;
    fout.open(argv[2],ios::out);

    //int match, mismatch, gap;
    string inputR[NUMBER];
    string inputQ[NUMBER];
    int bandwidth,X;

    //fin >> match >> mismatch >> gap;
    // fin >> bandwidth;
    // fin >> X;
    bandwidth = atoi(argv[3]);
    X = atoi(argv[4]);

    for(int i=0;i<NUMBER;i++){
        fin >> inputR[i];
        fin >> inputQ[i];
    }
    
    // cout << " R[0]: " << inputR[0] << endl;
    // cout << " Q[0]: " << inputQ[0] << endl;
    // cout << " R[1]: " << inputR[1] << endl;
    // cout << " Q[99]: " << inputQ[99] << endl;

    vector<vector <char> > R(NUMBER);
    vector<vector <char> > Q(NUMBER);
    
    for(int i=0;i<NUMBER;i++){
        for(int j=0;j<LENGTH;j++){
            R[i].push_back(inputR[i][j]);
            Q[i].push_back(inputQ[i][j]);
        }
        R[i].insert(R[i].begin(), '*');
        Q[i].insert(Q[i].begin(), '*');
    }
    
    // cout << "check" << endl;
    // for(int i=0;i<LENGTH+1;i++)
    //     cout << " R[0]: " << R[0][i] << endl;

    
    // for(int i=0;i<NUMBER;i++){

    //     cout << "size: " << Q[i].size() << " " << R[i].size() << endl;
    //     for(int j=0;j<LENGTH+1;j++){
    //         cout << R[i][j];   
    //     }
    //     cout << endl;
    //     for(int j=0;j<LENGTH+1;j++){
    //         cout << Q[i][j];
    //     }
    //     cout << endl;
    // }
    

    arrangement answer[NUMBER];
    arrangement answer_BSW[NUMBER];
    arrangement answer_GACT_X[NUMBER];
    
    tmusg.periodStart();
    for(int i=0;i<NUMBER;i++)
        answer[i] = SmithWaterman(R[i], Q[i]);
    tmusg.getPeriodUsage(SW_time);

    tmusg.periodStart();
    for(int i=0;i<NUMBER;i++)
        answer_BSW[i] = BSW(R[i], Q[i], bandwidth);
    tmusg.getPeriodUsage(BSW_time);

    tmusg.periodStart();
    for(int i=0;i<NUMBER;i++)
        answer_GACT_X[i] = GACT_X(R[i], Q[i], X);
    tmusg.getPeriodUsage(GACT_X_time);


    for(int j=0;j<NUMBER;j++){

        fout << "----------start----------" << endl;
        fout << "data " << j + 1 << ": " << endl;
        fout << "score of SW: " << answer[j].score << endl;
        // cout << "R: ";
        // for(int i=0;i<answer[j].trackback_R.size();i++)
        //     cout << answer[j].trackback_R[i];
        // cout << endl;
        // cout << "Q: ";
        // for(int i=0;i<answer[j].trackback_Q.size();i++)
        //     cout << answer[j].trackback_Q[i];
        // cout << endl;

        //cout << "-------------------------" << endl;

        fout << "score of BSW: " << answer_BSW[j].score << endl;
        // cout << "R: ";
        // for(int i=0;i<answer_BSW[j].trackback_R.size();i++)
        //     cout << answer_BSW[j].trackback_R[i];
        // cout << endl;
        // cout << "Q: ";
        // for(int i=0;i<answer_BSW[j].trackback_Q.size();i++)
        //     cout << answer_BSW[j].trackback_Q[i];
        // cout << endl;

        //cout << "-------------------------" << endl;

        fout << "score of GACT-X: "<< answer_GACT_X[j].score << endl;
        // cout << "R: ";
        // for(int i=0;i<answer_GACT_X[j].trackback_R.size();i++)
        //     cout << answer_GACT_X[j].trackback_R[i];
        // cout << endl;
        // cout << "Q: ";
        // for(int i=0;i<answer_GACT_X[j].trackback_Q.size();i++)
        //     cout << answer_GACT_X[j].trackback_Q[i];
        // cout << endl;
        fout << "-----------end-----------" << endl;

    }
    
    int BSW_good = 0, BSW_perfect = 0, GACT_X_good = 0, GACT_X_perfect = 0;
    
    for(int j=0;j<NUMBER;j++){
        if(answer[j].score == answer_BSW[j].score)
            BSW_perfect++;
        if(answer_BSW[j].score >= answer[j].score - (answer[j].score / 100.0 * 5))
            BSW_good++;
        if(answer[j].score == answer_GACT_X[j].score)
            GACT_X_perfect++;
        if(answer_GACT_X[j].score >= answer[j].score - (answer[j].score / 100.0 * 5))
            GACT_X_good++;
    }

    // result
    cout << endl;
    cout << "-----------SUMMARY------------" << endl;
    cout << "BSW bandwidth:       " << setw(3) << bandwidth << endl;
    cout << "BSW perfect:         " << setw(3) << BSW_perfect << "/" << NUMBER << endl;
    cout << "BSW good:            " << setw(3) << BSW_good << "/" << NUMBER << endl;
    cout << "GACT-X parameter:    " << setw(3) << X << endl;
    cout << "GACT-X perfect:      " << setw(3) << GACT_X_perfect << "/" << NUMBER << endl;
    cout << "GACT-X good:         " << setw(3) << GACT_X_good << "/" << NUMBER << endl;
    cout << "time cost by SW:     " << setw(7) << ((SW_time.uTime + SW_time.sTime) /1000.0) << "ms" << endl;
    cout << "time cost by BSW:    " << setw(7) << ((BSW_time.uTime + BSW_time.sTime) /1000.0) << "ms" << endl;
    cout << "time cost by GACT-X: " << setw(7) << ((GACT_X_time.uTime + GACT_X_time.sTime) /1000.0) << "ms" << endl;

    fout << endl;
    fout << "-----------SUMMARY------------" << endl;
    fout << "BSW bandwidth:       " << setw(3) << bandwidth << endl;
    fout << "BSW perfect:         " << setw(3) << BSW_perfect << "/" << NUMBER << endl;
    fout << "BSW good:            " << setw(3) << BSW_good << "/" << NUMBER << endl;
    fout << "GACT-X parameter:    " << setw(3) << X << endl;
    fout << "GACT-X perfect:      " << setw(3) << GACT_X_perfect << "/" << NUMBER << endl;
    fout << "GACT-X good:         " << setw(3) << GACT_X_good << "/" << NUMBER << endl;
    fout << "time cost by SW:     " << setw(7) << ((SW_time.uTime + SW_time.sTime) /1000.0) << "ms" << endl;
    fout << "time cost by BSW:    " << setw(7) << ((BSW_time.uTime + BSW_time.sTime) /1000.0) << "ms" << endl;
    fout << "time cost by GACT-X: " << setw(7) << ((GACT_X_time.uTime + GACT_X_time.sTime) /1000.0) << "ms" << endl;

    return 0;
}