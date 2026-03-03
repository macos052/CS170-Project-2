#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>

using std::vector;
using std::cin;
using std::cout;
using std::getline;
using std::string;
using std::ifstream;
using std::istringstream;

vector<vector<double>> loadData(const string& fileName){
    vector<vector<double>> data;
    ifstream file(fileName);
    string line;

    while(getline(file,line)){
        vector<double> row;
        istringstream ss(line);
        //read values
        double val;
        while(ss >> val){
            row.push_back(val);
        }
        if(!row.empty()){ //push valid, non-empty rows to vector
            data.push_back(row);
        }
    }
    file.close();
    return data;
}

int main(){
    string fileName;

    return 0;
}