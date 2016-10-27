
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

using namespace std;

/* precision for double number */
const double EPS = 1e-9;

/* name of the input csv file */
string csv_file_name;

/* the vector to store current commands user input */
vector<string> commands;

/* column names of current csv file */
vector<string> col_names;

/* all the numbers in each column */
vector<vector<double> > col_nums;

/* new column to store numbers after operations */
vector<double> new_col;

/* the result of inner join operation */
vector<double> inner_join_res;

/* the result of outer join operation */
vector<double> outer_join_res;

/* the hash table used in the join operation */
unordered_map<double, pair<int, int> > hash_table;


/* 
 * delete all the spaces in a string
 */
string trim_file_name(const string &filename){
    string res = "";
    for(auto c: filename){
        if(c != ' '){
            res += c;
        }
    }
    return res;
}

/* 
 * function to get all the column names 
 */
void parse_cvs_first_line(string line){
    line.pop_back();
    int len = line.length();
    int i = 0;
    col_names.clear();
    while(i < len){
        string tmp = "";
        while(i < len && line[i] != ',') {
            tmp += line[i++];
        }
        col_names.push_back(tmp);
        i++;
    }
    col_nums.assign(col_names.size(), vector<double>());
}

/* 
 * if a string is a positive integer 
 */
bool valid_num(string num){
    if(num == "") {
        return false;
    }
    for(int i = 0; i < num.length(); i++){
        if(i == 0 && num[i] == '+'){
            continue;
        }
        if(!isdigit(num[i])){
            return false;
        }
    }
    return true;
}

/* 
 * get numbers of a line in the csv file 
 */
void parse_cvs_num_line(string line){
    int len = line.length();
    int i = 0, idx = 0;
    while(i < len){
        string tmp = "";
        while(i < len && line[i] != ',') {
            if(line[i] != ' ') tmp += line[i];
            i++;
        }
        double num;
        if(tmp == "") num = 0.0;
        else num = stod(tmp);
        col_nums[idx++].push_back(num);
        i++;
    }
}

/* 
 * read the csv file, store all the data
 * if successful return true, otherwise return false 
 */
bool load_cvs_file(string filename){
    ifstream file(filename);
    if(!file){
        return false;
    }
    string line;
    bool first_line = true;
    while(getline(file, line)){
        if(first_line){
            parse_cvs_first_line(line);
            first_line = false;
        }
        else{
            parse_cvs_num_line(line);
        }
    }
    
    //delete the '\n' in the end
    //col_names.back().pop_back();

    //for(auto name: col_names) cout<<name<<' '; cout<<endl;
    //for(int i = 0; i < col_nums.size(); i++){
        //for(int j = 0; j < col_nums[i].size(); j++){
            //cout<<col_nums[i][j]<<' ';
        //}
        //cout<<endl;
    //}
    
    file.close();
    return true;
}

/* 
 * write all the data in the buffer to the file 
 */
bool write_csv_file(string filename){
    ofstream file_out(filename, ios::out);
    if(!file_out){
        cout<<"Error: cannot write to the file "<<filename<<endl;
        return false;
    }
    int n = col_names.size();
    for(int i = 0; i < n; i++){
        if(i < n-1){ 
            file_out << col_names[i]+',';
        }
        else{
            file_out << col_names[i];
        }
    }
    file_out << endl;

    //the number of columns 
    int width = n;
    //the maximum length of all columns
    int height = 0;
    for(auto nums: col_nums){
        height = max(height, (int)nums.size());
    }
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(col_nums[j].size() > i){
                file_out << col_nums[j][i];
            }
            if(j < width-1){
                file_out << ',';
            }
        }
        file_out << endl;
    }
    file_out.close();
    return true;
}

/* 
 * get the mininum value in the column with index idx 
 */
double get_min_in_col(int idx){
    double ans = numeric_limits<double>::max();
    for(int i = 0; i < col_nums[idx].size(); i++){
        ans = min(ans, col_nums[idx][i]);
    }
    if(abs(ans) <= EPS){
        ans = 0.0;
    }
    return ans;
}

/* 
 * get the maximum value in the column with index idx 
 */
double get_max_in_col(int idx){
    double ans = numeric_limits<double>::min();
    for(int i = 0; i < col_nums[idx].size(); i++){
        ans = max(ans, col_nums[idx][i]);
    }
    if(abs(ans) <= EPS){
        ans = 0.0;
    }
    return ans;
}

/* 
 * process the min command, get the mininum value in a column 
 */
void process_min_cmd(){
    //example: min col 2, must have 3 parameters
    if(commands.size() != 3){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get current column index, index in the buffer starts with 0
    int idx = stoi(commands[2])-1;
    if(idx < 0 || idx >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx].size() == 0){
        cout<<"Error: this column is empty"<<endl;
        return;
    }
    double ans = get_min_in_col(idx);
    cout<<"The mininum element in column "<<commands[2]<<" is: "<<ans<<endl;
}

/* 
 * process the max command, get the maximum value in a column 
 */
void process_max_cmd(){
    //example: max col 2, must have 3 parameters
    if(commands.size() != 3){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    
    //get current column index, index in the buffer starts with 0
    int idx = stoi(commands[2])-1;
    if(idx < 0 || idx >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx].size() == 0){
        cout<<"Error: this column is empty"<<endl;
        return;
    }
    double ans = get_max_in_col(idx);
    cout<<"The maxinum element in column "<<commands[2]<<" is: "<<ans<<endl;
}

/* 
 * process the med command, get the median value in a column 
 */
void process_median_cmd(){
    //example: med col 3, must have 3 parameters 
    if(commands.size() != 3){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }

    //get current column index, index in the buffer starts with 0
    int idx = stoi(commands[2])-1;
    if(idx < 0 || idx >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx].size() == 0){
        cout<<"Error: this column is empty"<<endl;
        return;
    }

    //temporary vector for sorting 
    vector<double> tmp = col_nums[idx];
    sort(tmp.begin(), tmp.end());
    double ans;
    int n = tmp.size();
    //get the median value
    if(n % 2 == 1) ans = tmp[n/2];
    else ans = (tmp[n/2]+tmp[n/2-1])/2.0;
    if(abs(ans) <= EPS){
        ans = 0.0;
    }
    cout<<"The median element in column "<<commands[2]<<" is: "<<ans<<endl;
}

/* 
 * process the avg command, get the average value in a column 
 */
void process_average_cmd(){
    //example: avg col 3  must have 3 parameters 
    if(commands.size() != 3){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }

    //get current column index, index in the buffer starts with 0
    int idx = stoi(commands[2])-1;
    if(idx < 0 || idx >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx].size() == 0){
        cout<<"Error: this column is empty"<<endl;
        return;
    }

    double total = 0.0;
    for(int i = 0; i < col_nums[idx].size(); i++){
        total += col_nums[idx][i];
    }
    double ans = total / (1.0 * col_nums[idx].size());
    if(abs(ans) <= EPS){
        ans = 0.0;
    }
    cout<<"The average value in column "<<commands[2]<<" is: "<<ans<<endl;
}

/* 
 * print the new column added
 */
void print_new_col(const vector<double> &new_col){
    for(int i = 0; i < new_col.size(); i++){
        cout<<new_col[i]<<' ';
    }
    cout<<endl;
}

void if_write_new_col_to_file(){
    cout<<"Write new column to the file? yes(y) or no(n): ";
    col_nums.push_back(new_col);
    string cmd;
    getline(cin, cmd);
    if(cmd == "n" || cmd == "no" || cmd == "N" || cmd == "NO" || cmd == "No"){
        col_nums.pop_back();
        return;
    }
    if(cmd == "y" || cmd == "yes" || cmd == "Y" || cmd == "YES" || cmd == "Yes"){
        while(true){
            cout<<"Please enter a name for the new column: ";
            getline(cin, cmd);
            cmd = trim_file_name(cmd);
            if(cmd.size() == 0){
                cout<<"The name should not be empty"<<endl;
                continue;
            }
            bool if_exist = false;
            for(auto name: col_names){
                if(cmd == name){
                    if_exist = true;
                    break;
                }
            }
            if(if_exist){
                cout<<"The name already exists, please choose a new name"<<endl;
                continue;
            }
            col_names.push_back(cmd);
            write_csv_file(csv_file_name);
            cout<<"Operation success, already add a new column"<<endl;
            break;
        }
    }
}

/* 
 * process the add command, get a new column
 * example: add col 1 2
 * add column 1 and column 2
 */
void process_col_add_cmd(){
    //must have 4 parameters 
    if(commands.size() != 4){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2]) || !valid_num(commands[3])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the indexes of the two columns 
    int idx1 = stod(commands[2])-1;
    int idx2 = stod(commands[3])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx1].size() == 0 && col_nums[idx2].size() == 0){
        cout<<"Error: both columns are empty"<<endl;
        return;
    }
    //get the size of two columns 
    int n1 = col_nums[idx1].size(), n2 = col_nums[idx2].size();
    //if sizes are different, use the larger one for the new column 
    new_col.assign(max(n1, n2), 0);
    for(int i = 0; i < max(n1, n2); i++){
        if(i < n1) new_col[i] += col_nums[idx1][i];
        if(i < n2) new_col[i] += col_nums[idx2][i];
    }
    if_write_new_col_to_file();
}

/* 
 * process the sub command, get a new column
 * example: sub col 1 2
 * subtract column 1 and column 2
 */
void process_col_sub_cmd(){
    //must have 4 parameters 
    if(commands.size() != 4){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2]) || !valid_num(commands[3])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the indexes of two columns 
    int idx1 = stod(commands[2])-1;
    int idx2 = stod(commands[3])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx1].size() == 0 && col_nums[idx2].size() == 0){
        cout<<"Error: both columns are empty"<<endl;
        return;
    }
    //if sizes are different, use the larger one for the new column 
    int n1 = col_nums[idx1].size(), n2 = col_nums[idx2].size();
    new_col.assign(max(n1, n2), 0);
    for(int i = 0; i < max(n1, n2); i++){
        if(i < n1) new_col[i] += col_nums[idx1][i];
        if(i < n2) new_col[i] -= col_nums[idx2][i];
    }
    if_write_new_col_to_file();
}

/* 
 * process the mul command, get a new column
 * example: mul col 1 2
 * multiply column 1 and column 2
 */
void process_col_mul_cmd(){
    //must have 4 parameters 
    if(commands.size() != 4){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2]) || !valid_num(commands[3])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the indexes of two columns 
    int idx1 = stod(commands[2])-1;
    int idx2 = stod(commands[3])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx1].size() == 0 && col_nums[idx2].size() == 0){
        cout<<"Error: both columns are empty"<<endl;
        return;
    }
    int n1 = col_nums[idx1].size(), n2 = col_nums[idx2].size();
    //if sizes are different, use the larger one for the new column 
    new_col.assign(max(n1, n2), 1.0);
    for(int i = 0; i < max(n1, n2); i++){
        if(i < n1) new_col[i] *= col_nums[idx1][i];
        if(i < n2) new_col[i] *= col_nums[idx2][i];
    }
    if_write_new_col_to_file();

}

/* 
 * process the div command, get a new column
 * example: div col 1 2
 * divide column 1 by column 2
 */
void process_col_div_cmd(){
    //must have 4 parameters 
    if(commands.size() != 4){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "col" || !valid_num(commands[2]) || !valid_num(commands[3])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the indexes of two columns 
    int idx1 = stod(commands[2])-1;
    int idx2 = stod(commands[3])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx1].size() == 0 && col_nums[idx2].size() == 0){
        cout<<"Error: both columns are empty"<<endl;
        return;
    }
    int n1 = col_nums[idx1].size(), n2 = col_nums[idx2].size();
    //if sizes are different, cannot perform division 
    if(n1 != n2){
        cout<<"Error: two columns have different size, cannot perform division"<<endl;
        return;
    }
    //if one divisor is zero 
    for(auto num: col_nums[idx2]){
        if(num >= -EPS && num <= EPS){
            cout<<"Error: 0.0 cannot be a divisor"<<endl;
            return;
        }
    }
    new_col.assign(n1, 0);
    for(int i = 0; i < n1; i++){
        new_col[i] = col_nums[idx1][i] * 1.0 / col_nums[idx2][i];
    }
    if_write_new_col_to_file();
}

/* 
 * get the inner join set of two sets
 * if an element appears more than once in the result, output all
 */
void get_inner_join_two_sets(const vector<double> &a, const vector<double> &b){
    hash_table.clear();
    //count all the elements in a
    for(auto num: a){
        hash_table[num].first++;
        hash_table[num].second++;
    }
    //subtract the count of elements in b
    for(auto num: b){
        if(hash_table.count(num) > 0){
            hash_table[num].first--;
        }
    }
    inner_join_res.clear();
    for(auto node: hash_table){
        //number of current element in the inner join set 
        int num = min(node.second.second, node.second.second - node.second.first);
        //add all the inner join elements to the result 
        for(int i = 0; i < num; i++){
            inner_join_res.push_back(node.first);
        }
    }
}

/* 
 * get the outer join set of two sets
 * if an element appears more than once in the result, output all
 */
void get_outer_join_two_sets(const vector<double> &a, const vector<double> &b){
    hash_table.clear();
    //count all the elements in a
    for(auto num: a){
        hash_table[num].first++;
        hash_table[num].second++;
    }
    for(auto num: b){
        //subtract the count of the element in b if it is a
        //otherwise, add element to hash table
        if(hash_table.count(num) > 0){
            hash_table[num].first--;
        }
        else{
            hash_table[num].first++;
            hash_table[num].second++;
        }
    }
    outer_join_res.clear();
    for(auto node: hash_table){
        //get the element which only in one set
        if(node.second.first == node.second.second){
            int num = node.second.second;
            //get all the elements
            for(int i = 0; i < num; i++){
                outer_join_res.push_back(node.first);
            }
        }
    }
}

/* 
 * process the inner join command, output the result
 * example: inner join col 1 2
 * get the inner join result of column 1 and 2
 */
void process_inner_join_cmd(){
    //must have 5 parameteres
    if(commands.size() != 5){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "join" || commands[2] != "col" || !valid_num(commands[3]) || !valid_num(commands[4])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the columns indexes 
    int idx1 = stod(commands[3])-1;
    int idx2 = stod(commands[4])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    if(col_nums[idx1].size() == 0 || col_nums[idx2].size() == 0){
        cout<<"The inner join set is empty"<<endl;
        return;
    }
    //if the same just output itself 
    if(idx1 == idx2){
        cout<<"The inner join set of two columns is: "<<endl;
        print_new_col(col_nums[idx1]);
        return;
    }
    //optimization: in order to just add all the elements in 
    //the set with a smaller size to the hash table 
    if(col_nums[idx1].size() > col_nums[idx2].size()){
        swap(idx1, idx2);
    }
    get_inner_join_two_sets(col_nums[idx1], col_nums[idx2]);
    if(inner_join_res.size() == 0){
        cout<<"The inner join set is empty"<<endl;
        return;
    }
    cout<<"The inner join result of column "<<commands[3]
        <<" and column "<<commands[4]<<" is: "<<endl;
    print_new_col(inner_join_res);
}

/* 
 * process the outer join command, output the result
 * example: outer join col 1 2
 * get the outer join result of column 1 and 2
 */
void process_outer_join_cmd(){
    //must have 5 parameters 
    if(commands.size() != 5){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands[1] != "join" || commands[2] != "col" || !valid_num(commands[3]) || !valid_num(commands[4])){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //get the columns indexes 
    int idx1 = stoi(commands[3])-1;
    int idx2 = stoi(commands[4])-1;
    if(idx1 < 0 || idx1 >= col_nums.size() || idx2 < 0 || idx2 >= col_nums.size()){
        cout<<"Error: column index is not valid"<<endl;
        return;
    }
    //same column, empty result
    if(idx1 == idx2){
        cout<<"The outer join set is empty"<<endl;
        return;
    }
    //both empty, empty result
    if(col_nums[idx1].size() == 0 && col_nums[idx2].size() == 0){
        cout<<"The outer join set is empty"<<endl;
        return;
    }
    //first one is empty, just output the second one 
    if(col_nums[idx1].size() == 0){
        cout<<"The outer join set of two columns is: "<<endl;
        print_new_col(col_nums[idx2]);
        return;
    }
    //second one is empty, just output the first one 
    if(col_nums[idx2].size() == 0){
        cout<<"The outer join set of two columns is: "<<endl;
        print_new_col(col_nums[idx1]);
        return;
    }
    get_outer_join_two_sets(col_nums[idx1], col_nums[idx2]);
    if(outer_join_res.size() == 0){
        cout<<"The outer join set is empty"<<endl;
        return;
    }
    cout<<"The outer join result of column "<<commands[3]
        <<" and column "<<commands[4]<<" is: "<<endl;
    print_new_col(outer_join_res);
}

/* 
 * list all the current column names 
 */
void process_list_col_names_cmd(){
    //only have 1 parameter 
    if(!((commands.size() == 2 && commands[1] == "col") || 
       (commands.size() == 3 && commands[1] == "col" && valid_num(commands[2])))){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    if(commands.size() == 2){
        if(col_names.size() == 0){
            cout<<"Current file is empty"<<endl;
            return;
        }
        cout<<"All column names: "<<endl;
        int idx = 1;
        for(string name: col_names){
            cout<<name<<'('<<(idx++)<<')'<<' ';
        }
        cout<<endl;
        return;
    }
    if(commands.size() == 3){
        int idx = stoi(commands[2]) - 1;
        if(idx < 0 || idx >= col_names.size()){
            cout<<"Error: column index is out of boundary"<<endl;
            return;
        }
        cout<<col_names[idx]<<endl;
        for(auto num: col_nums[idx]){
            cout<<num<<' ';
        }
        cout<<endl;
    }
}

/* 
 * delete a column by giving the name
 * example: del col age
 * delete the column with a name age
 * each column should have a unique name
 */
void process_delete_col(){
    //only have 1 parameter 
    if(commands.size() != 3 || commands[1] != "col" || commands[2].size() == 0){
        cout<<"Error: command is not valid"<<endl;
        return;
    }
    //find the index of the column 
    int name_idx = -1;
    for(int i = 0; i < col_names.size(); i++){
        if(commands[2] == col_names[i]){
            name_idx = i;
        }
    }
    //if cannot find, show error and return 
    if(name_idx == -1){
        cout<<"Error: cannot find a column with the name "<<commands[2]<<endl;
        return;
    }

    //if find, ask user to confirm this deletion
    cout<<"Delete? yes(y) or no(n): ";
    string cmd;
    getline(cin, cmd);
    if(cmd == "n" || cmd == "no" || cmd == "N" || cmd == "NO" || cmd == "No"){
        return;
    }
    if(cmd == "y" || cmd == "yes" || cmd == "Y" || cmd == "YES" || cmd == "Yes"){
        //delete the column name
        col_names.erase(col_names.begin() + name_idx);
        //delete all the data in the column
        col_nums.erase(col_nums.begin() + name_idx);
        //update the csv file
        write_csv_file(csv_file_name);
        cout<<"deletion finished"<<endl;
    }
}

/* 
 * parse the command each time the user inputs
 */
void parse_cmd(string cmd){
    int i = 0;
    //add all the strings in the command to commands
    commands.clear();
    while(i < cmd.length()){
        string command = "";
        while(i < cmd.length() && cmd[i] != ' ') { 
            command += cmd[i++];
        }
        commands.push_back(command);
        while(i < cmd.length() && cmd[i] == ' ') {
            i++;
        }
    }
    //call related function to process this command according to
    //the first string in the command 
    if(commands[0] == "min"){
        process_min_cmd();
    }
    else if(commands[0] == "max"){
        process_max_cmd();
    }
    else if(commands[0] == "med"){
        process_median_cmd();
    }
    else if(commands[0] == "avg"){
        process_average_cmd();
    }
    else if(commands[0] == "add"){
        process_col_add_cmd();
    }
    else if(commands[0] == "sub"){
        process_col_sub_cmd();
    }
    else if(commands[0] == "mul"){
        process_col_mul_cmd();
    }
    else if(commands[0] == "div"){
        process_col_div_cmd();
    }
    else if(commands[0] == "inner"){
        process_inner_join_cmd();
    }
    else if(commands[0] == "outer"){
        process_outer_join_cmd();
    }
    else if(commands[0] == "ls"){
        process_list_col_names_cmd();
    }
    else if(commands[0] == "del"){
        process_delete_col();
    }
    else{
        cout<<"Error: "<<"\""+cmd+"\""<<" is not a valid command"<<endl;
    }
}


/* 
 * if a string only contains spaces 
 */
bool empty_input(const string &input){
    for(auto c: input){
        if(c != ' '){
            return false;
        }
    }
    return true;
}

int main(){
    //let user input the csv filename, if cannot load the file
    //the user should input again until success 
    while(true){
        cout<<"Please input the csv file name: ";
        //getline(cin, csv_file_name);
        //csv_file_name = trim_file_name(csv_file_name);
        csv_file_name = "haha.csv";
        int name_len = csv_file_name.length();
        if(name_len < 5 || csv_file_name.substr(name_len-4) != ".csv"){
            cout<<"Error: "<<csv_file_name<<" is not a valid csv file name"<<endl;
            continue;
        }
        if(!load_cvs_file(csv_file_name)){
            cout<<"Error: cannot open the file "<<csv_file_name<<endl;
        }
        else{
            cout<<"Open file "<<csv_file_name<<" successful"<<endl;
            break;
        }
    }
    //reading user's command until user input 'quit'
    while(true){
        cout<<">> ";
        string cmd;
        getline(cin, cmd);
        if(cmd == "quit"){
            cout<<"Quit? yes(y) or no(n): ";
            getline(cin, cmd);
            if(cmd == "n" || cmd == "no" || cmd == "N" || cmd == "NO" || cmd == "No"){
                continue;
            }
            if(cmd == "y" || cmd == "yes" || cmd == "Y" || cmd == "YES" || cmd == "Yes"){
                break;
            }
        }
        if(empty_input(cmd)){
            continue;
        }
        parse_cmd(cmd);
    };
    return 0;
}
