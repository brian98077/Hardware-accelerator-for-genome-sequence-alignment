// ==========================================
// ==========================================
//     genome alignment visualize version
//     arthor : b10502070

//     how to use:

//     input:
//     bandwidth of BSW
//     parameter X of GACT-X
//     reference sequence 
//     query sequence 
// ==========================================
// ==========================================


#define MATCH 2
#define MISMATCH -1
#define GAP 1

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <windows.h>
using namespace std;

struct SetColor {
    int n;
    SetColor(int n) : n(n) {}
};

std::ostream& operator<<(std::ostream& os, const SetColor& sc) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), sc.n);
    return os;
}

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
    vector<vector <int>> DP(Q.size());
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

    cout << "--------------------" << endl;
    for(int i=1;i<Q.size();i++){
        for(int j=1;j<DP[i].size();j++){
            if(DP[i][j])
                cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
            else
                cout << setw(3) << SetColor(7) << DP[i][j];
        }
        cout << endl;
    }
    cout << "--------------------" << endl;

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
    vector<vector <int>> DP(Q.size());
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
        

    cout << "--------------------" << endl;
    for(int i=1;i<Q.size();i++){
        for(int j=1;j<DP[i].size();j++){
            if(DP[i][j])
                cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
            else
                cout << setw(3) << SetColor(7) << DP[i][j];
        }
        cout << endl;
    }
    ;
    cout << "-------------------" << endl;

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
    vector<vector <int>> DP(Q.size());
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
        

    cout << "--------------------" << endl;
    for(int i=1;i<Q.size();i++){
        for(int j=1;j<DP[i].size();j++){
            if(DP[i][j] > 0)
                cout << setw(3) << SetColor(10) << DP[i][j] << SetColor(7);
            else if(DP[i][j] < 0)
                cout << setw(3) << SetColor(12) << DP[i][j] << SetColor(7);
            else
                cout << setw(3) << SetColor(7) << DP[i][j];
        }
        cout << endl;
    }
    cout << "--------------------" << endl;

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

int main(){
    
    //int match, mismatch, gap;
    string inputR,inputQ;
    int bandwidth,X;

    //cin >> match >> mismatch >> gap;
    cin >> bandwidth;
    cin >> X;
    cin >> inputR;
    cin >> inputQ;
    

    vector <char> R(inputR.begin(), inputR.end());
    vector <char> Q(inputQ.begin(), inputQ.end());
    R.insert(R.begin(), '*');
    Q.insert(Q.begin(), '*');


    // cout << "size: " << Q.size() << " " << R.size() << endl;
    // for(int i=0;i<R.size();i++)
    //     cout << R[i];
    // cout << endl;
    // for(int i=0;i<Q.size();i++)
    //     cout << Q[i];
    // cout << endl;

    arrangement answer = SmithWaterman(R, Q);
    arrangement answer_BSW = BSW(R, Q, bandwidth);
    arrangement answer_GACT_X = GACT_X(R, Q, X);

    cout << "----------start----------" << endl;
    cout << "score of SW: " << SetColor(11) << answer.score << SetColor(7) << endl;
    cout << "R: ";
    for(int i=0;i<answer.trackback_R.size();i++)
        cout << answer.trackback_R[i];
    cout << endl;
    cout << "Q: ";
    for(int i=0;i<answer.trackback_Q.size();i++)
        cout << answer.trackback_Q[i];
    cout << endl;

    cout << "-------------------------" << endl;

    cout << "score of BSW: " << SetColor(11) << answer_BSW.score << SetColor(7) << endl;
    cout << "R: ";
    for(int i=0;i<answer_BSW.trackback_R.size();i++)
        cout << answer_BSW.trackback_R[i];
    cout << endl;
    cout << "Q: ";
    for(int i=0;i<answer_BSW.trackback_Q.size();i++)
        cout << answer_BSW.trackback_Q[i];
    cout << endl;

    cout << "-------------------------" << endl;

    cout << "score of GACT-X: " << SetColor(11) << answer_GACT_X.score << SetColor(7) << endl;
    cout << "R: ";
    for(int i=0;i<answer_GACT_X.trackback_R.size();i++)
        cout << answer_GACT_X.trackback_R[i];
    cout << endl;
    cout << "Q: ";
    for(int i=0;i<answer_GACT_X.trackback_Q.size();i++)
        cout << answer_GACT_X.trackback_Q[i];
    cout << endl;
    cout << "-----------end-----------" << endl;

    return 0;
}