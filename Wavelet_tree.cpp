using namespace std;
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>
#include <iterator>
#include <math.h>
#include <unordered_map>
using namespace std::chrono;
unordered_map<int, int> values;
unordered_map<int, int> num_of_occurances;
class Node {
public:
    int l_ni;
    int r_ni;
    int m_ni;
    int depth;
    Node* u_child = nullptr;
    Node* w_child = nullptr;
    Node* parent = nullptr;
    vector<int>* S;
    vector<int>* alphabet;
    vector<bool> B_ni;
    vector<int> C_ni;
    vector<int> select0B;
    vector<int> select1B;
    Node(vector<int>* alphabet, vector<int>* S, int S_size, Node* parent, int l_ni, int r_ni, int depth)
        : alphabet(alphabet), S(S), parent(parent), l_ni(l_ni), r_ni(r_ni), depth(depth) {
        //std::chrono::steady_clock::time_point beg = std::chrono::high_resolution_clock::now();
        if (l_ni == r_ni) {
            return;
        }
        int value;
        int a;
        int num_of_0 = 0;
        int num_of_1 = 0;
        int pos0 = 0;
        int pos1 = 0;
        m_ni = (l_ni + r_ni) / 2;
        for (int i = 0; i < S_size; i++) {
            a = S->at(i);
            value = values.at(a);
            if (value <= m_ni) {
                num_of_0++;
            }
            else {
                num_of_1++;
            }
        }
        B_ni.resize(S_size);
        C_ni.resize(S_size);
        select0B.resize(num_of_0);
        select1B.resize(num_of_1);
        vector<int> leftS; leftS.resize(num_of_0);
        vector<int> rightS; rightS.resize(num_of_1);
        for (int i = 0; i < S_size; i++) {
            a = S->at(i);
            value = values.at(a);
            if (value <= m_ni) {
                leftS[pos0] = a;
                B_ni[i] = (false);
                C_ni[i] = (i == 0 ? 0 : C_ni[i - 1]);
                select0B[pos0++] = (i + 1); //zbog indeksiranja od 1
            }
            else {
                rightS[pos1] = a;
                B_ni[i] = (true);
                C_ni[i] = (i == 0 ? 1 : C_ni[i - 1] + 1);
                select1B[pos1++] = (i + 1); //zbog indeksiranja od 1
            }
        }
        u_child = new Node(alphabet, &leftS, pos0, this, l_ni, m_ni, depth + 1);
        w_child = new Node(alphabet, &rightS, pos1, this, m_ni + 1, r_ni, depth + 1);
    }
    Node(vector<int>* alphabet, vector<int>* S, int S_size, Node* parent, int l_ni, int r_ni, int depth, bool compression) 
        : alphabet(alphabet), S(S), parent(parent), l_ni(l_ni), r_ni(r_ni), depth(depth) {
        if (l_ni == r_ni) {
            return;
        }
        m_ni = (l_ni + r_ni) / 2;
        int a, value;
        int num_of_0 = 0;
        int num_of_1 = 0;
        int pos0 = 0;
        int pos1 = 0;
        for (int i = 0; i < S_size; i++) {
            a = S->at(i);
            value = values.at(a);
            if (value <= m_ni) {
                num_of_0++;
            }
            else {
                num_of_1++;
            }
        }
        B_ni.resize(S_size);
        vector<int> *leftS = new vector<int>(); leftS->resize(num_of_0);
        vector<int> *rightS = new vector<int>(); rightS->resize(num_of_1);
        for (int i = 0; i < S_size; i++) {
            a = S->at(i);
            value = values.at(a);
            if (value <= m_ni) {
                (*leftS)[pos0++] = a;
                B_ni[i] = (false);
            }
            else {
                (*rightS)[pos1++] = a;
                B_ni[i] = (true);
            }
        }
        u_child = new Node(alphabet, leftS, pos0, this, l_ni, m_ni, depth + 1, true);
        delete leftS;
        w_child = new Node(alphabet, rightS, pos1, this, m_ni + 1, r_ni, depth + 1, true);
        delete rightS;
    }
    ~Node() {
        delete u_child;
        delete w_child;
    }
    int tree_memory() {
        if (l_ni == r_ni) {
            return    4 + 4 + 4 + 4 //int l_ni, int r_ni, int m_ni, int depth
                + 8 + 8 + 8 //Node* u_child, Node* w_child i Node* parent
                + 16 + 16 //vector<int>* S i vector<int>* alphabet
                + 32 * 4 //prazni vektori B_ni, C_ni, select0B i select1B
                + 24 * 2;
        }
        return    4 + 4 + 4 + 4 //int l_ni, int r_ni, int m_ni, int depth
            + 8 + 8 + 8 //Node* u_child, Node* w_child i Node* parent
            + 16 + 16 //vector<int>* S i vector<int>* alphabet
            + (B_ni.size() + 7) / 8
            + C_ni.size() * sizeof(int)
            + select0B.size() * sizeof(int) * 2 //za select0B i leftS
            + select1B.size() * sizeof(int) * 2 //za select1B i rightS
            + 4 + 4 + 4 + 4 + 4 + 4 //int value, a, num_of_0, num_of_1, pos0, pos1
            + 24 + u_child->tree_memory()
            + 24 + w_child->tree_memory();
    }
    void print_recursion(string a) {
        if (l_ni == r_ni) {
            cout << "---Node (leaf - << " << a << "), depth = " << depth << "---" << endl;
            cout << "l_ni = " << l_ni << ", r_ni = " << r_ni << endl;
            return;
        }
        cout << "---Node (" << a << "), depth = " << depth << "---" << endl;
        cout << "l_ni = " << l_ni << ", r_ni = " << r_ni << endl;
        cout << "B_ni = ";
        copy(B_ni.begin(), B_ni.end(), ostream_iterator<int>(cout, " "));
        cout << endl << "C_ni = ";
        copy(C_ni.begin(), C_ni.end(), ostream_iterator<int>(cout, " "));
        cout << std::endl;
        cout << "position_of_zeros = ";
        copy(select0B.begin(), select0B.end(), ostream_iterator<int>(cout, " "));
        cout << std::endl;
        cout << "position_of_ones = ";
        copy(select1B.begin(), select1B.end(), ostream_iterator<int>(cout, " "));
        cout << std::endl;
        if (u_child != nullptr) u_child->print_recursion("left");
        if (w_child != nullptr) w_child->print_recursion("right");
    }
    int accessCompressed(int i) {
        if (i == 0) return NULL;
        if (l_ni == r_ni) {
            return alphabet->at(l_ni);
        }
        if (!B_ni[i - 1]) {
            i = 0;
            for (bool bit : B_ni) bit == 0 ? i++ : 0;
            return u_child->accessCompressed(i);
        }
        else {
            i = 0;
            for (bool bit : B_ni) bit == 1 ? i++ : 0;
            return w_child->accessCompressed(i);
        }
    }
    int access(int i) {
        if (i == 0) return NULL;
        if (l_ni == r_ni) {
            return alphabet->at(l_ni);
        }
        if (!B_ni[i - 1]) {
            i = mapLeft(i);
            return u_child->access(i);
        }
        else {
            i = mapRight(i);
            return w_child->access(i);
        }
    }
    int mapLeft(int i) {
        if (i < 1) return 0;
        return i - C_ni[i - 1];
    }
    int mapRight(int i) {
        if (i < 1) return 0;
        return C_ni[i - 1];
    }
    int rank(int q, int i) {
        if (l_ni == r_ni) {
            return i;
        }
        if (values.at(q) <= m_ni) {
            return this->u_child->rank(q, mapLeft(i));
        }
        else {
            return this->w_child->rank(q, mapRight(i));
        }
    }
    int select(int a, int j) {
        if (j == 0) return 0;
        if (l_ni == r_ni) {
            return j;
        }
        if (values[a] <= m_ni) {
            j = u_child->select(a, j);
            return select0B[j - 1];
        }
        else {
            j = w_child->select(a, j);
            return select1B[j - 1];
        }
    }
    int quantile(int k, int i, int j) {
        if (k > (j - i + 1)) { cout << "INVALID K"; return -2; }
        //cout << "   quantile(" << k << ", " << i << ", " << j << ")\n";
        if (l_ni == r_ni) {
            return alphabet->at(l_ni);
        }
        int c = mapLeft(j) - mapLeft(i - 1);
        if (k <= c) {
            //cout << "       next is left\n";
            return u_child->quantile(k, mapLeft(i - 1) + 1, mapLeft(j));
        }
        else {
            //cout << "       next is right\n";
            return w_child->quantile(k - c, mapRight(i - 1) + 1, mapRight(j));
        }
    }
    int range(int x, int y, int i, int j) {
        if (x > y) { cout << "INVALID RANGE [x, y]"; return -3; }
        //cout << "   range(" << x << "," << y << "," << i << "," << j << ")\n";
        if (x <= l_ni && r_ni <= y) { // presjek je [l_ni, r_ni]
            //cout << "       range(" << x << "," << y << "," << i << "," << j << ") = ";
            if (j == 0) {
                return 0;
            }
            if (i == 0) {
                //cout << j << "\n";
                return j;
            }
            if (l_ni == r_ni && parent->m_ni == r_ni) {
                //cout << j - i << "\n";
                return j - i;
            }
            else {
                //cout << j - i + 1 << "\n";
                return j - i + 1;
            }
        }
        else if (y < l_ni || x > r_ni) { // nema presjeka
         //cout << "       range(" << x << "," << y << "," << i << "," << j << ") = " << 0 << "\n";
            return 0;
        }
        else {
            return u_child->range(x, y, mapLeft(i), mapLeft(j)) + w_child->range(x, y, mapRight(i), mapRight(j));
        }
    }
};
class Tree {
public:
    int value;
    vector<int> *alphabet;
    Node* root = nullptr;
    Tree(vector<int>* txt, vector<int>* alphabet_p) {
        values.clear();
        for (int i = 0; i < alphabet->size(); i++) {
            alphabet = new vector<int>(*alphabet_p);
            values[alphabet->at(i)] = i;
        }
        root = new Node(alphabet, txt, txt->size(), nullptr, 0, (alphabet->size()) - 1, 1);
    }
    Tree(vector<int>* txt, vector<int>* alphabet_p, bool compression) {
        alphabet = new vector<int>(*alphabet_p);
        values.clear();
        for (int i = 0; i < alphabet->size(); i++) {
            values[alphabet->at(i)] = i;
        }
        root = new Node(alphabet, txt, txt->size(), nullptr, 0, (alphabet->size()) - 1, 1, true);
    }
    int tree_memory() {
        return 4 + 24 + root->tree_memory();
    }
    int accessCompressed(int i) {
        return root->accessCompressed(i);
    }
    int access(int i) {
        return root->access(i);
    }
    int rank(int a, int i) {
        return root->rank(a, i);
    }
    int select(int a, int j) {
        return root->select(a, j);
    }
    ~Tree() {
        delete alphabet;
        delete root;
    }
};
bool contains(vector<int>* s, int a) {
    for (int b: *s) {
        if(b == a) return true;
    }
    return false;
}
int main(void) {
    vector<int> *alphabet = new vector<int>();
    vector<int> *txt = new vector<int>();
    //std::chrono::steady_clock::time_point beg;
    //std::chrono::steady_clock::time_point end;
    /*
    int sigma;
    int txt_size;
    int result = 0;
    int values_construction_time = 0;
    int tree_construction_time = 0;
    int sum_access_time = 0;
    int sum_rank_time = 0;
    int sum_select_time = 0;
    int access_op_counter = 0;
    int rank_op_counter = 0;
    int select_op_counter = 0;
    int memory = 0;
    cout << "text_size,sigma,tree_construction_time,values_map_construction_time,avg_access_time,avg_rank_time,avg_select_time,memory" << endl;
    for (int sigma_magnitude = 1; sigma_magnitude < 20; sigma_magnitude++) { //11
        for (int txt_magnitude = 1000000; txt_magnitude < 2000001; txt_magnitude += 1000000) { //25 za bazu 2 //txt_magnitude++
            int num_of_runs = 1;
            result = 0;
            sum_access_time = 0;
            sum_rank_time = 0;
            sum_select_time = 0;
            access_op_counter = 0;
            rank_op_counter = 0;
            select_op_counter = 0;
            memory = 0;
            values_construction_time = 0;
            tree_construction_time = 0;

            //sigma = sigma_magnitude;
            sigma = pow(2, sigma_magnitude);

            txt_size = txt_magnitude;
            //if (txt_magnitude < 1000000) {
            //   txt_magnitude += 100000;
            //} else {
            //    txt_magnitude += 250000;
            //}

            //txt_size = pow(2, txt_magnitude);
            for (int run = 1; run <= num_of_runs; run++) {
                txt.clear();
                alphabet.clear();
                values.clear();
                num_of_occurances.clear();

                //cout << txt_size << "," << sigma << ",";
                for (int i = 0; i < sigma; i++) {
                    alphabet.push_back(i);
                    num_of_occurances[i] = 0;
                }
                //Stvaranje nasumiènog teksta za testiranje performanse
                int a;
                for (int i = 0; i < txt_size; i++) {
                    a = (rand() % sigma);
                    num_of_occurances[a]++;
                    txt.push_back(a);
                }
                //cout << endl;
                //copy(txt.begin(), txt.end(), ostream_iterator<int>(cout, ",")); cout << endl;
                //Stvaranje strukture s svim znakovima abecede i njihovim vrijednostima.
                beg = high_resolution_clock::now();
                int rows = ceil(log2(alphabet.size()));
                for (int i = 0; i < alphabet.size(); i++) {
                    values[alphabet.at(i)] = i;
                }
                end = high_resolution_clock::now();
                values_construction_time += duration_cast<microseconds>(end - beg).count();
                beg = high_resolution_clock::now();
                Tree* t = new Tree(&txt, &alphabet);
                end = high_resolution_clock::now();
                tree_construction_time += duration_cast<microseconds>(end - beg).count();
                memory += t->tree_memory();
                //m->print();
                for (int j = 0; j < txt_size + 1; j++) {
                    beg = std::chrono::high_resolution_clock::now();
                    result = t->root->access(j);
                    end = std::chrono::high_resolution_clock::now();
                    sum_access_time += std::chrono::duration_cast<microseconds>(end - beg).count();
                    access_op_counter++;
                    //cout << "access(" << j << ")," << result << "," << std::chrono::duration_cast<microseconds>(end - beg).count() << endl;
                    if (access_op_counter % 100 == 0) break;
                }
                for (auto a : alphabet) {
                    for (int j = 0; j < txt_size + 1; j++) {
                        beg = high_resolution_clock::now();
                        result = t->root->rank(a, j);
                        end = high_resolution_clock::now();
                        sum_rank_time += duration_cast<microseconds>(end - beg).count();
                        rank_op_counter++;
                        //cout << "rank(" << a << ", " << j << ")," << result << "," << duration_cast<microseconds>(end - beg).count() << endl;
                        if (rank_op_counter % 100 == 0) break;
                    }
                    for (int j = 0; j < num_of_occurances[a] + 1; j++) {
                        beg = high_resolution_clock::now();
                        result = t->root->select(a, j); select_op_counter++;
                        end = high_resolution_clock::now();
                        sum_select_time += duration_cast<microseconds>(end - beg).count();
                        select_op_counter++;
                        //cout << "select(" << a << ", " << j << ")," << result << "," << duration_cast<microseconds>(end - beg).count() << endl;
                        if (select_op_counter % 100 == 0) break;
                    }
                }
                delete(t);
            }
            cout << txt_size << ","
                << sigma << ","
                << ((tree_construction_time - values_construction_time) / num_of_runs) << ","
                << (values_construction_time / num_of_runs) << ","
                << ((sum_access_time / access_op_counter)) << ","
                << ((sum_rank_time / rank_op_counter)) << ","
                << ((sum_select_time / select_op_counter)) << ","
                << (memory / num_of_runs)
                << endl;
        }
    }
    */
    string a;
    ifstream myfile("C:\\Users\\Benjamin\\Desktop\\Desktop mape\\skooool i fakultet\\faks\\6. semestar\\ZR\\message(6).txt");
    txt->resize(2110968); //1696601 //4411532 //13588 //6552 //4641652 //2110968 //size of whatever is suppoesd to be conpressed
    int i = 0;
    if (myfile.is_open()) {
        bool seen = false;
        while (getline(myfile, a)) {
            for (char b : a) {
                //if (b == 32) continue;
                if (!contains(alphabet, b)) alphabet->push_back(b);
                //txt->push_back(b);
                (*txt)[i++] = (int)b;
            }
        }
        myfile.close();
    }
    else {
        return -1;
    }
    //alphabet->push_back('A'); alphabet->push_back('C'); alphabet->push_back('G'); alphabet->push_back('T');
    copy(alphabet->begin(), alphabet->end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    cout << txt->size() << endl;
    //copy(txt.begin(), txt.end(), ostream_iterator<int>(cout, " "));
    Tree* t = new Tree(txt, alphabet, true);
    delete txt; delete alphabet;
    cout << t->tree_memory();
    cout << "amogus";
    delete t;
    cout << "sus";
    /*
    vector<int> alphabet;
    vector<int> txt;
    int sigma;
    int txt_size;
    int result = 0;
    int values_construction_time = 0;
    int tree_construction_time = 0;
    int sum_access_time = 0;
    int sum_rank_time = 0;
    int sum_select_time = 0;
    int access_op_counter = 0;
    int rank_op_counter = 0;
    int select_op_counter = 0;
    int memory = 0;
    for (int sigma_magnitude = 2; sigma_magnitude < 3; sigma_magnitude++) { //11
        for (int txt_magnitude = 1; txt_magnitude < 9; txt_magnitude ++) { //25 za bazu 2 //txt_magnitude++
            int num_of_runs = 1;
            result = 0;
            sum_access_time = 0;
            sum_rank_time = 0;
            sum_select_time = 0;
            access_op_counter = 0;
            rank_op_counter = 0;
            select_op_counter = 0;
            memory = 0;
            sigma = pow(2, sigma_magnitude);
            //txt_size = txt_magnitude;
            txt_size = pow(10, txt_magnitude);
            for (int run = 1; run <= num_of_runs; run++) {
                txt.clear();
                alphabet.clear();
                values.clear();
                num_of_occurances.clear();

                //cout << txt_size << "," << sigma << ",";
                for (int i = 0; i < sigma; i++) {
                    alphabet.push_back(i);
                    num_of_occurances[i] = 0;
                }
                //Stvaranje nasumiènog teksta za testiranje performanse
                int a;
                for (int i = 0; i < txt_size; i++) {
                    a = (rand() % sigma);
                    num_of_occurances[a]++;
                    txt.push_back(a);
                }
                //cout << endl;
                //copy(txt.begin(), txt.end(), ostream_iterator<int>(cout, ",")); cout << endl;
                //Stvaranje strukture s svim znakovima abecede i njihovim vrijednostima.
                beg = high_resolution_clock::now();
                int rows = ceil(log2(alphabet.size()));
                for (int i = 0; i < alphabet.size(); i++) {
                    values[alphabet.at(i)] = i;
                }
                end = high_resolution_clock::now();
                values_construction_time += duration_cast<microseconds>(end - beg).count();
                beg = high_resolution_clock::now();
                Tree* t = new Tree(&txt, &alphabet, true);
                end = high_resolution_clock::now();
                tree_construction_time += duration_cast<microseconds>(end - beg).count();
                memory += t->tree_memory();
                //m->print();
                for (int j = 0; j < txt_size + 1; j++) {
                    beg = std::chrono::high_resolution_clock::now();
                    result = t->root->accessCompressed(j);
                    end = std::chrono::high_resolution_clock::now();
                    sum_access_time += std::chrono::duration_cast<microseconds>(end - beg).count();
                    access_op_counter++;
                    //cout << "access(" << j << ")," << result << "," << std::chrono::duration_cast<microseconds>(end - beg).count() << endl;
                    if (access_op_counter % 100 == 0) break;
                }
                delete(t);
                cout << txt_size << endl;
            }
            cout << txt_size << ","
                << sigma << ","
                << ((tree_construction_time - values_construction_time) / num_of_runs) << ","
                << (values_construction_time / num_of_runs) << ","
                << ((sum_access_time / access_op_counter)) << ","
                << (memory / num_of_runs)
                << endl;
        }
    }
    */
    return 0;
}