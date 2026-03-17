#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<unordered_set>
#include<cmath>
#include <chrono>
#include <ctime>

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
// Assumes first column of each row is class label (1 or 2), remaining columns are features
// Ignores empty lines
// Reads a space-separated ASCII dataset file into a 2D vector of doubles.
// Column 0 of each row is the class label (1 or 2); remaining columns are features.
// Uses istringstream with >> operator, which automatically handles whitespace separation.
vector<vector<double> > loadData(const string& fileName){
    vector<vector<double> > data;
    ifstream file(fileName);
    string line;

    if(!file.is_open()){
        throw std::runtime_error("file not open!");
    }
    
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

// Overloaded << operators to print vector and unordered_set nicely
// For unordered_set, convert to sorted vector before printing
template <typename T>
std::ostream& operator<<(std::ostream& os, const vector<T> obj){
    for(size_t i = 0; i < obj.size(); i++){
        os << obj.at(i);
        if(i != obj.size() - 1){
            os << ", ";
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
            os << ", ";
        }
    }

    return os;
}

// Evaluates a feature subset via Leave-One-Out Cross-Validation (LOOCV) using 1-NN (1-Nearest Neighbor).
// feature_to_add = 0 signals Backward Elimination (no new feature appended).
// Returns classification accuracy in range [0.0, 1.0].
double leave_one_out_cross_validation(const vector<vector<double> >& data, const vector<int>& current_set, int feature_to_add){
    if(data.empty()){ // Prevent division by 0 return
        throw std::runtime_error("Empty data!");
    }

    double correctClassification = 0;

    // Prepare feature subset: start with current_set, optionally add feature_to_add
    vector<int> currentFeatures = current_set;
    if(feature_to_add != 0){ // Prevent addition of dummy feature for backward elimination
        currentFeatures.push_back(feature_to_add);
    }

    // For each instance in the dataset
    for(int i = 0; i < data.size(); i++){
        double nearestNeighborDistance = std::numeric_limits<double>::max(); // Sets to maximum value of double 
        double nearestNeighborLabel = 0; 

        // For each other instance in the dataset
        for(int k = 0; k < data.size(); k++){

            if(k == i){continue;} // Stops comparing against itself

            // Compute Euclidean distance between instance i and instance k using selected features
            double distance = 0; // Reset distance
            for(int& j : currentFeatures){// Sum of squared differences for selected features
                distance += pow(data[i][j] - data[k][j],2);
            }
            distance = sqrt(distance);// Final Euclidean distance

            // Update nearest neighbor if this distance is smaller
            if(distance < nearestNeighborDistance){
                nearestNeighborDistance = distance;
                nearestNeighborLabel = data[k][0];
            }

        }

        // Increment correctClassification if predicted label matches actual
        if(nearestNeighborLabel == data[i][0]){
            correctClassification ++;
        }

    }
    // Return fraction of correctly classified instances (accuracy)
    return correctClassification / data.size();
}

// Greedy Forward Selection: starts with an empty set, adds the single best
// feature at each level based on LOOCV accuracy. 
// Tracks the globally best feature subset seen across all levels,
// not just the final accumulated set.
void forward_selection(const vector<vector<double> >& data);

// Greedy Backward Elimination: starts with all features, removes the feature
// whose removal yields the highest LOOCV accuracy at each level.
// Uses feature_to_add=0 in LOOCV to indicate no new feature is being tested.
void backward_elimination(const vector<vector<double> >& data);

// Main program flow:
// 1. Prompt user for input file
// 2. Load dataset
// 3. Ask user to select Forward Selection or Backward Elimination
// 4. Measure and print execution time
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

    auto start = std::chrono::high_resolution_clock::now();

    if(answer == 1){
        forward_selection(data);
    }
    else{
        backward_elimination(data);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    cout << "Time elapsed: " << elapsed.count() << " seconds" << endl;
    return 0;
}

void forward_selection(const vector<vector<double> >& data){
    unordered_set<int> current_set_of_features;
    vector<int> best_overall_features; // Tracks best subset features

    int total_features = data[0].size() - 1;
    double best_overall_accuracy = 0;

    //print default rate
    cout << "Using feature(s) {" << "} accuracy is " 
    << std::fixed << std::setprecision(2) << 
    leave_one_out_cross_validation(data,{},0)*100 << "%" << endl;

// Outer loop: iterate up to total number of features, adding one per level
// Inner loop: test all candidate features not yet in the current set
    for(int i = 1; i <= total_features; i++){

        int feature_to_add_to_this_level = 0;
        double best_so_far_accuracy = 0;

        for(int j = 1; j <= total_features; j++){
            if(current_set_of_features.find(j) == current_set_of_features.end()){

                vector<int> current_vec(current_set_of_features.begin(),
                current_set_of_features.end());

                double accuracy = leave_one_out_cross_validation(data, current_vec, j) * 100;

                vector<int> output_vec = current_vec;
                output_vec.push_back(j); // Add feature to output

                // Output feature and accuracy
                std::sort(output_vec.begin(), output_vec.end());
                cout << "Using feature(s) {" << output_vec << "} accuracy is "
                << std::fixed << std::setprecision(2) << accuracy << "%" << endl;

                if(accuracy > best_so_far_accuracy){
                    // Update best_so_far_accuracy and feature_to_add_to_this_level
                    // This tracks the best feature to add at this level
                    best_so_far_accuracy = accuracy;
                    feature_to_add_to_this_level = j;
                }
            }
        }
        // Always add best feature this level
        current_set_of_features.insert(feature_to_add_to_this_level);

        // Update best_overall_features if accuracy improves
        if(best_so_far_accuracy >= best_overall_accuracy){ // Store best accuracy and features

            best_overall_accuracy = best_so_far_accuracy;
            best_overall_features.push_back(feature_to_add_to_this_level);

            std::sort(best_overall_features.begin(), best_overall_features.end());
            cout << "Feature set {" << best_overall_features << "} was best, accuracy is " 
            << std::fixed << std::setprecision(2) << best_overall_accuracy << "%"<< endl; 
        }

    }
    std::sort(best_overall_features.begin(), best_overall_features.end());

    cout << "Finished search! The best feature subset is {" << best_overall_features
     << "}, which has an accuracy of " << std::fixed << std::setprecision(2)
     << best_overall_accuracy << "%" << endl;
}

void backward_elimination(const vector<vector<double> >& data){
    unordered_set<int> current_set_of_features;
    vector<int> best_overall_features;

    int total_features = data[0].size() - 1;
    double best_overall_accuracy = 0;

    // Start with all features
    for(int i = 1; i <= total_features; i++){
        current_set_of_features.insert(i);
    }

    vector<int> current_vec(current_set_of_features.begin(), current_set_of_features.end());
    std::sort(current_vec.begin(),current_vec.end());
    
    best_overall_accuracy = leave_one_out_cross_validation(data, current_vec, 0) * 100;
    
    cout << "Using feature(s) {" << current_vec << "} accuracy is " 
    << std::fixed << std::setprecision(2) << best_overall_accuracy << "%" << endl;

    // Outer loop: iterate up to total_features times, removing one feature per level
    // Inner loop: test removal of each feature currently in the set
    for(int i = 1; i <= total_features; i++){

        int feature_to_remove_this_level = 0;
        double best_so_far_accuracy = 0;

        for(int j = 1; j <= total_features; j++){
            if(current_set_of_features.find(j) != current_set_of_features.end()){
                // Temporarily remove feature j and compute accuracy
                // Keep track of feature whose removal improves accuracy the most
                unordered_set<int> temp_set = current_set_of_features;
                temp_set.erase(j);

                current_vec.assign(temp_set.begin(), temp_set.end());

                double accuracy = leave_one_out_cross_validation(data, current_vec, 0) * 100;

                vector<int> output_vec = current_vec;

                // Output features and accuracy
                std::sort(output_vec.begin(), output_vec.end());
                cout << "Using feature(s) {" << output_vec << "} accuracy is "
                << std::fixed << std::setprecision(2) << accuracy << "%" << endl;

                if(accuracy > best_so_far_accuracy){
                    best_so_far_accuracy = accuracy;
                    feature_to_remove_this_level = j;
                }
            }
        }
        // Always remove best feature this level
        current_set_of_features.erase(feature_to_remove_this_level);

        // Update best_overall_features if accuracy improves
        if(best_so_far_accuracy >= best_overall_accuracy){
            best_overall_accuracy = best_so_far_accuracy;

            best_overall_features = vector<int>(current_set_of_features.begin(),
            current_set_of_features.end());
            
            std::sort(best_overall_features.begin(), best_overall_features.end());
            
            cout << "Feature set {" << best_overall_features << "} was best, accuracy is " 
            << std::fixed << std::setprecision(2) << best_overall_accuracy << "%" << endl; 
        }
    }
    std::sort(best_overall_features.begin(), best_overall_features.end());

    cout << "Finished search! The best feature subset is {" << best_overall_features
    << "}, which has an accuracy of " << std::fixed << std::setprecision(2)
    << best_overall_accuracy << "%" << endl;
}