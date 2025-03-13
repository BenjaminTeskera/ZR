#include<iostream>
#include <vector>
#include <memory>
#include <algorithm>
using namespace std;

bool contains(vector<char>* v, char b);
bool contains(string* v, char b);

class Node {
public:
    Node* parent = nullptr;
    Node* left_child = nullptr;
    Node* right_child = nullptr;
    string group0 = ""; string left_text_string = "";
    string group1 = ""; string right_text_string = "";
    string text_string;
    vector<bool> bit_vector = {};
    Node(string input_text_string, Node* input_parent, bool side) {
        parent = input_parent;
        text_string = input_text_string;
        vector<char> unique;
        int size;
        if ((text_string).size() > 1) {
            for (char a : text_string) {
                bit_vector.push_back(false);
                if (contains(&unique, a)) {
                    continue;
                }
                else {
                    unique.push_back(a);
                }
            }
            size = unique.size();
            sort(unique.begin(), unique.end());
            for (int i = 0; i < size; i++) {
                if (i < ((size) / 2)) {
                    group0 += unique[i];
                }
                else {
                    group1 += unique[i];
                }
            }
            for (int i = 0; i < (text_string).size(); i++) {
                if (contains(&group0, (text_string)[i])) {
                    bit_vector[i] = false;
                    left_text_string += (text_string)[i];
                }
                else {
                    bit_vector[i] = true;
                    right_text_string += (text_string)[i];
                }
            }
            cout << "--------------" << "\n";
            cout << "Ulazni niz: " << text_string << "\n";
            cout << "Stvorena 2 skupa: [" << group0 << "] i [" << group1 << "]\n";
            cout << "Bit vektor: {|"; for (int a : bit_vector) { cout << a << "|"; }; cout << "}\n";
            cout << "--------------" << "\n";
            if (group0.size() > 1) {
                left_child = new Node(left_text_string, this, false);
            }
            else {
                left_child = new Node(group0, this, false);
            }
            if (group1.size() > 1) {
                right_child = new Node(right_text_string, this, true);
            }
            else {
                right_child = new Node(group1, this, true);
            }
        }
        else {
            bit_vector.push_back(side);
            cout << "--------------" << "\n";
            cout << "Ulazni znak: " << text_string << "\n";
            cout << "Bit znaka: {|"; cout << (int)bit_vector[0] << "|}\n";
            cout << "--------------" << "\n";
        }
    };
    ~Node() {
        delete(left_child);
        delete(right_child);
    };
    
    bool access(int offset_i) {
        Node* N = this;
        int help_i = 0;
        vector<bool>* B = &(*N).bit_vector;
        bool b = false;
        cout << "-------start-------" << "\n";
        cout << "text, offset_i, b: [" << (N->text_string) << ", " << offset_i << ", " << b << "]\n";
        cout << "B vektor: {|"; for (int a : (*B)) { cout << a << "|"; }; cout << "}\n";
        cout << "--------------" << "\n";
        while (!(((*N).left_child == nullptr) && ((*N).right_child == nullptr))) {
            B = &(*N).bit_vector;
            b = (*B)[offset_i];
            cout << "--------------" << "\n";
            cout << "text, offset_i, b: [" << (N->text_string) << ", " << offset_i << ", " << b << "]\n";
            cout << "B vektor: {|"; for (int a : (*B)) { cout << a << "|"; }; cout << "}\n";
            cout << "--------------" << "\n";
            if (!b) {
                N = (*N).left_child;
            }
            else {
                N = (*N).right_child;
            }
            for (int j = 0; j < offset_i; j++) { if ((*B)[j] == b) help_i++; } offset_i = help_i; help_i = 0; // equivalent to i <- B.rank[b](i) or i = B.rank(b, i)
        }
        return (*N).bit_vector[0];
    }
    
    string C(char char_x) {
        string c = "";
        cout << "   current node adress: {" << &(this->text_string) << "} ";
        cout << "   current node value: " << (this->text_string) << "\n";
        if (!(((*this).left_child == nullptr) && ((*this).right_child == nullptr))) {
            if (contains(&group0, char_x)) {
                c.push_back('0');
                cout << "   left";
                c.append((*left_child).C(char_x));
                return c;
            }
            else {
                c.push_back('1');
                cout << "   right";
                c.append((*right_child).C(char_x));
            }
        }
        return c;
    }

    int rank(char char_x, int offset_i) {
        Node* N = this;
        int help_i = 0;
        int k = 0;
        vector<bool>* B = &(*N).bit_vector;
        bool b = false;
        while (!(((*N).left_child == nullptr) && ((*N).right_child == nullptr))) {
            cout << "--------------" << "\n";
            cout << "text, offset_i, k, b, x: [" << (N->text_string) << ", " << offset_i << ", " << k << ", " << b << ", " << char_x << "]\n";
            cout << "B vektor: {|"; for (int a : (*B)) { cout << a << "|"; }; cout << "}\n";
            cout << "--------------" << "\n";
            B = &(*N).bit_vector;
            b = (bool)(int)(C(char_x)[k] - '0');
            if (!b) {
                N = (*N).left_child;    
            } else {
                N = (*N).right_child;
            }
            for (int j = 0; j < offset_i; j++) { if ((*B)[j] == b) help_i++; } offset_i = help_i; help_i = 0; // equivalent to i <- B.rank[b](i) or i = B.rank(b, i)
            k++;
        }
        return offset_i;
    }

    int select(char char_x, int offset_i) {
        Node* N = this;
        string c = C(char_x);
        while (!(((*N).left_child == nullptr) && ((*N).right_child == nullptr))) {
            if (contains(&((*N).group0), char_x)) {
                cout << " >left (" << char_x << " is in " << (*N).group0 << ")\n";
                N = (*N).left_child;
            } else {
                cout << " >right (" << char_x << " is in " << (*N).group1 << ")\n";
                N = (*N).right_child;
            }
        }
        int help_i = -1;
        int k = c.length() - 1; // equivalent to node depth
        vector<bool>* B;
        bool b;
        cout << "------start--------" << "\n";
        cout << "text, offset_i, k C(), b, x: [" << (N->text_string) << ", " << offset_i << ", " << k << "{" << c  << "} , " << "undefined" << ", " << char_x << "]\n";
        cout << "--------------" << "\n";
        while ((*N).parent != nullptr) {
            N = (*N).parent;
            B = &(*N).bit_vector;
            b = (bool)(int)(c[k] - '0');
            ////B.select(b, i) find i-th occurance of b in bit vector B
            cout << "-------before B.select(b, i)-------" << "\n";
            cout << "text, offset_i, k, b, x: [" << (N->text_string) << ", " << offset_i << ", " << k << ", " << b << ", " << char_x << "]\n";
            cout << "B vektor: {|"; for (int a : (*B)) { cout << a << "|"; }; cout << "}\n";
            cout << "--------------" << "\n";
            for (int j = 0; j < (*B).size(); j++) {
                if ((*B)[j] == b) {
                    cout << "+\n";
                    help_i++;
                    if (help_i == offset_i) {
                        offset_i = j;
                        break;
                    }
                }
                try {
                    if ((help_i < offset_i)&&(j == (*B).size() - 1)) {
                        throw exception("!!go away!! not enough instances of the requested charachter for the requested method");
                    }
                }
                catch (exception e) {
                    cout << e.what() << "\n";
                    return -1;
                }
            }
            help_i = -1; //end of B.select(b, i)
            k--;
            cout << "-------end of loop-------" << "\n";
            cout << "text, offset_i, k, b, x: [" << (N->text_string) << ", " << offset_i << ", " << k << ", " << b << ", " << char_x << "]\n";
            cout << "B vektor: {|"; for (int a : (*B)) { cout << a << "|"; }; cout << "}\n";
            cout << "--------------" << "\n";
        }
        return offset_i;
    }
};

bool contains(vector<char>* v, char b) {
    if ((*v).size() == 0) return false;
    for (char a : *v) {
        if (a == b) return true;
    }
    return false;
}

bool contains(string* v, char b) {
    if ((*v).size() == 0) return false;
    for (char a : *v) {
        if (a == b) return true;
    }
    return false;
}

int main(void) {
    string txt = "aacggtatttac";
    Node korijen = Node(txt, nullptr, NULL);
    Node *test = &korijen;
    //cout << "=====================\n";
    //cout << txt << ".rank(t, 10): \n" << korijen.rank('t', 10) << "\n";
    //cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-\n";
    //cout << txt << ".access(2): \n" << korijen.access(2) << "\n";

    cout << "=====================\n";
    cout << txt << ".select(a, 3): \n" << korijen.select('a', 3) << "\n";

    return 0;
}

