#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<set>
#include<cstdlib>

using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::ifstream;
using std::istringstream;
using std::set;
using std::rand;

vector<vector<double>> loadData(const string& fileName){
    vector<vector<double>> data;
    ifstream file(fileName);
    string line;

    while(getline(file,line)){
        vector<double> row;
        istringstream ss(line);
        //read and store values
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

double leave_one_out_cross_validation(vector<vector<double>> data, set<int> current_set, int feature_to_add){
    return rand();
}

void forward_selection(vector<vector<double>> data);

void backward_elimination(vector<vector<double>> data);

int main(){
    string fileName;

    cout << "Welcome to my Feature Selection Algorithm." << endl;
    cout << "Type in the name of the file to test : " << endl;

    cin >> fileName;

    vector<vector<double>> data = loadData(fileName);

    int answer = 0;
    do{
        cout << "Type the number of the algorithm you want to run." << endl;
        cout << "1) Forward Selection" << endl;
        cout << "2) Backward Elimination" << endl;

        cin >> answer;
        if(cin.fail()){
            throw std::runtime_error("Invalid input!");
        }
    }while(answer != 1 || answer != 2);

    if(answer == 1){

    }
    else{

    }
    return 0;
}