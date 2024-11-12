#define LENGTH 128
#define NUMBER 100

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <algorithm>
using namespace std;

string ramdom_ATCG(int length){
    char temp;
    string sequence;

    random_device rd;
    default_random_engine random(rd());

    for(int i=0;i<length*NUMBER;i++){
        int RANDOM = random() % 4;
        //cout << RANDOM << endl;
        if(RANDOM == 0)
            temp = 'A';
        else if(RANDOM == 1)
            temp = 'T';
        else if(RANDOM == 2)
            temp = 'C';
        else if(RANDOM == 3)
            temp = 'G';
        
        sequence += temp;
    }
    return sequence;
}

int main(){
    
    string data[NUMBER];
    random_device rd;
    default_random_engine random(rd());
    int swap_count[LENGTH*NUMBER];

    for(int i=0;i<NUMBER;i++){
        for(int j=0;j<LENGTH;j++){
           data[i] += ramdom_ATCG(LENGTH)[i*LENGTH + j];
           swap_count[i*LENGTH + j] = random() % LENGTH;
        }
    }
    

    for(int i=0;i<NUMBER;i++){

        int swap_num = i % (LENGTH/4) + (LENGTH/16);
        //cout << "num = " << swap_num << endl;

        // original data
        //cout << data[i] << endl;
        
        
        for(int k=0;k<swap_num;k++){
            // cout << "RANDOM 1 : " << swap_count[i*LENGTH + 2 * k] << endl;
            // cout << "RANDOM 2 : " << swap_count[i*LENGTH + 2 * k + 1] << endl;
            swap(data[i][swap_count[i*LENGTH + 2 * k]], data[i][swap_count[i*LENGTH + 2 * k + 1]]);
        }

        // // data after random modify
        // cout << data[i] << endl;
        for(int j=LENGTH-1;j>=0;j--){
            if(data[i][j] == 'A') cout << "00";
            else if(data[i][j] == 'T') cout << "11";
            else if(data[i][j] == 'C') cout << "01";
            else if(data[i][j] == 'G') cout << "10";
            else{
                cout << "error";
                break;
            }
        }
        cout << endl;
    }

    return 0;
}