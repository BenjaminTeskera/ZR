#include<iostream>
#include <vector>
#include <memory>
#include <algorithm>
using namespace std;
 
bool contains(vector<char> *v, char b);

class Node{
    Node *parent = nullptr;
    Node *left_child = nullptr;
    Node *right_child = nullptr;
    string group0 = ""; string left_text_string = "";
    string group1 = ""; string right_text_string = "";
    string *text_string;
    vector<bool> bit_vector = {};
    public:
        Node(string *input_text_string, Node *input_parent, bool side){
        parent = input_parent;
        text_string = input_text_string;
        vector<char> unique;
        int size;
        if((*text_string).size() > 1){
            for(char a: *text_string){
                bit_vector.push_back(false);
                if(contains(&unique, a)){
                    continue;
                } else {
                    unique.push_back(a);
                }
            }
            size = unique.size();
            sort(unique.begin(), unique.end());
            for(int i = 0; i < size ; i++){
                if(i < ((size) / 2)){
                    group0 += unique[i];
                    for(int j = 0; j < bit_vector.size(); j++){ 
                        if((*text_string)[j] == unique[i]) {
                            bit_vector[j] = false;
                            left_text_string += unique[i];
                        }
                    }
                } else {
                    group1 += unique[i];
                    for(int j = 0; j < bit_vector.size(); j++){ 
                        if((*text_string)[j] == unique[i]) {
                            bit_vector[j] = true;
                            right_text_string += unique[i];
                        }
                    }
                }
            }
            cout << "--------------" << "\n";
            cout << "Ulazni niz: "<< *text_string << "\n";
            cout << "Stvorena 2 skupa: ["<< group0 << "] i [" << group1 << "]\n";
            cout << "Bit vektor: {|"; for(int a: bit_vector){cout << a << "|";}; cout << "}\n";
            cout << "--------------" << "\n";
            if(group0.size() > 1) {
                Node new_left_child = Node(&left_text_string, this, false);
                left_child = &new_left_child;
            } else {
                Node new_left_child = Node(&group0, this, false);
                left_child = &new_left_child;
            }
            if(group1.size() > 1) {
                Node new_right_child = Node(&right_text_string, this, true);
                right_child = &new_right_child;
            } else {
                Node new_right_child = Node(&group1, this, true);
                right_child = &new_right_child;
            }
        } else {
            cout << "--------------" << "\n";
            cout << "Ulazni znak: "<< *text_string << "\n";
            cout << "Bit znaka: {|"; cout << (int)side << "|}\n";
            cout << "--------------" << "\n";
        }
    };
};

bool contains(vector<char> *v, char b){
    if((*v).size() == 0) return false;
    for(char a: *v){
        if(a == b) return true; 
    }
    return false;
}

int main(void){
    
    string txt = "aacggtatttac";
    Node korijen = Node(&txt, nullptr, NULL);
    return 0;
}

