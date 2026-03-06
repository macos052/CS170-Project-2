#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<unordered_set>
#include<cstdlib>
#include<cmath>

using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::to_string;
using std::string;
using std::ifstream;
using std::istringstream;
using std::unordered_set;
using std::sqrt;
using std::pow;
using std::rand;

vector<vector<double> > loadData(const string& fileName){
    vector<vector<double> > data;
    ifstream file(fileName);
    string line;

    while(getline(file,line)){
        vector<double> row;
        istringstream ss(line);
        // Read and store values
        double val;
        while(ss >> val){
            row.push_back(val);
        }
        if(!row.empty()){ // Push valid, non-empty rows to vector
            data.push_back(row);
        }
    }
    file.close();
    return data;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const vector<T> obj){
    for(int i = 0; i < obj.size(); i++){
        os << obj.at(i);
        if(i != obj.size() - 1){
            os << ",";
        }
    }
    return os;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& obj){
    std::vector<T> currObj(obj.begin(), obj.end());

    std::sort(currObj.begin(), currObj.end());

    for(size_t i = 0; i < currObj.size(); i++){
        os << currObj[i];
        if(i != currObj.size() - 1){
            os << ",";
        }
    }

    return os;
}
double leave_one_out_cross_validation(const vector<vector<double> >& data, const vector<int>& current_set, int feature_to_add){
    if(data.empty()){ // Prevent division by 0 return
        throw std::runtime_error("Empty data!");
    }

    double correctClassification = 0;

    vector<int> currentFeatures = current_set;
    if(feature_to_add != 0){ // Prevent addition of dummy feature for backward elimination
        currentFeatures.push_back(feature_to_add);
    }

    for(int i = 0; i < data.size(); i++){
        double nearestNeighborDistance = std::numeric_limits<double>::max(); // Sets to maximum value of double 
        double nearestNeighborLabel = 0; 

        for(int k = 0; k < data.size(); k++){

            if(k == i){continue;} // Stops comparing against itself

            double distance = 0; // Reset distance
            for(int& j : currentFeatures){ // Sum of all feature distances
                distance += pow(data[i][j] - data[k][j],2);
            }
            distance = sqrt(distance);

            if(distance < nearestNeighborDistance){
                nearestNeighborDistance = distance;
                nearestNeighborLabel = data[k][0];
            }

        }

        if(nearestNeighborLabel == data[i][0]){
            correctClassification ++;
        }

    }
    return correctClassification / data.size();
}

void forward_selection(vector<vector<double> > data);

void backward_elimination(vector<vector<double> > data);

int main(){
    string fileName;

    cout << "Welcome to my Feature Selection Algorithm." << endl;
    cout << "Type in the name of the file to test : " << endl;

    cin >> fileName;

    vector<vector<double> > data = loadData(fileName);

    int answer = 0;
    do{
        cout << "Type the number of the algorithm you want to run." << endl;
        cout << "1) Forward Selection" << endl;
        cout << "2) Backward Elimination" << endl;

        cin >> answer;
        if(cin.fail()){
            throw std::runtime_error("Invalid input!");
        }
    }while(answer != 1 && answer != 2);

    if(answer == 1){
        forward_selection(data);
    }
    else{
        backward_elimination(data);
    }
    return 0;
}

void forward_selection(vector<vector<double> > data){
    unordered_set<int> current_set_of_features;
    vector<int> best_overall_features; // Tracks best subset features

    int total_features = data[0].size() - 1;
    double best_overall_accuracy = 0;

    for(int i = 1; i <= total_features; i++){

        int feature_to_add_to_this_level = 0;
        double best_so_far_acccuracy = 0;

        for(int j = 1; j <= total_features; j++){
            if(current_set_of_features.find(j) == current_set_of_features.end()){

                vector<int> current_vec(current_set_of_features.begin(),
                current_set_of_features.end());

                double accuracy = leave_one_out_cross_validation(data, current_vec, j) * 100;

                cout << "Using feature(s) {" << current_vec << "} accuracy is "
                << std::fixed << std::setprecision(2) << accuracy << "%" << endl;

                if(accuracy > best_so_far_acccuracy){
                    best_so_far_acccuracy = accuracy;
                    feature_to_add_to_this_level = j;
                }
            }
        }
        // Always add best feature this level
        current_set_of_features.insert(feature_to_add_to_this_level);

        if(best_so_far_acccuracy > best_overall_accuracy){ // Store best accuracy and features

            best_overall_accuracy = best_so_far_acccuracy;
            best_overall_features.push_back(feature_to_add_to_this_level);

            cout << "Feature set {" << best_overall_features << "} was best, accuracy is " 
            << std::fixed << std::setprecision(2) << best_overall_accuracy << "%"<< endl; 
        }

    }
    std::sort(best_overall_features.begin(), best_overall_features.end());

    cout << "Finished search! The best feature subset is {" << best_overall_features
     << "}, which has an accuracy of " << std::fixed << std::setprecision(2)
     << best_overall_accuracy << "%" << endl;
}

void backward_elimination(vector<vector<double> > data){
    unordered_set<int> current_set_of_features;
    vector<int> best_overall_features;

    int total_features = data[0].size() - 1;
    double best_overall_accuracy = 0;

    // Start with all features
    for(int i = 1; i <= total_features; i++){
        current_set_of_features.insert(i);
    }

    for(int i = 1; i <= total_features; i++){

        int feature_to_remove_this_level = 0;
        double best_so_far_acccuracy = 0;

        for(int j = 1; j <= total_features; j++){
            if(current_set_of_features.find(j) != current_set_of_features.end()){
                // Temporarily remove j to test without it
                unordered_set<int> temp_set = current_set_of_features;
                temp_set.erase(j);
                vector<int> current_vec(temp_set.begin(), temp_set.end());
                double accuracy = leave_one_out_cross_validation(data, current_vec, 0) * 100;

                cout << "Using feature(s) {" << current_vec << "} accuracy is " 
                << std::fixed << std::setprecision(2) << accuracy << "%" << endl;

                if(accuracy > best_so_far_acccuracy){
                    best_so_far_acccuracy = accuracy;
                    feature_to_remove_this_level = j;
                }
            }
        }
        // Always remove best feature this level
        current_set_of_features.erase(feature_to_remove_this_level);

        if(best_so_far_acccuracy > best_overall_accuracy){
            best_overall_accuracy = best_so_far_acccuracy;

            best_overall_features = vector<int>(current_set_of_features.begin(),
            current_set_of_features.end());

            cout << "Feature set {" << best_overall_features << "} was best, accuracy is " 
            << std::fixed << std::setprecision(2) << best_overall_accuracy << "%" << endl; 
        }
    }
    std::sort(best_overall_features.begin(), best_overall_features.end());

    cout << "Finished search! The best feature subset is {" << best_overall_features
    << "}, which has an accuracy of " << std::fixed << std::setprecision(2)
    << best_overall_accuracy << "%" << endl;
}