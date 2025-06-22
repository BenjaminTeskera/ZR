using namespace std;
#include <stdlib.h>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <Windows.h>
using namespace std::chrono;
unordered_map<int, vector<bool>> values;
unordered_map<int, int> num_of_occurances;
class Wavelet_matrix {
public:
    vector<int>* text;
    vector<int>* alphabet;
    vector<vector<bool>> B;
    vector<vector<int>> rank0B;
    vector<vector<int>> select0_or_1B;
    vector<int> Z;
    int rows;
    int columns;
    Wavelet_matrix(vector<int>* text, vector<int>* alphabet) {
        //Inicijalizacija prametara velièine matrice
        rows = ceil(log2(alphabet->size()));
        columns = text->size();
        //punjenje values mape kako korisnik ne bi morao
        values.clear();
        for (int i = 0; i < alphabet->size(); i++) {
            vector<bool> bits(rows);
            for (int j = 0; j < rows; ++j) {
                bits[rows - 1 - j] = (i >> j) & 1;
            }
            values[alphabet->at(i)] = bits;
        }

        //Inicijalizacija velièine matrice i potpornih struktura podataka
        this->text = text;
        this->alphabet = alphabet;
        B.resize((rows), vector<bool>(columns));
        rank0B.resize((rows), vector<int>(columns));
        select0_or_1B.resize((rows), vector<int>(columns));
        Z.resize(rows);
        //konstrukcija
        int a; bool bit;
        vector<int> indeksi(columns);
        iota(indeksi.begin(), indeksi.end(), 0);
        for (int l = 0; l < rows; l++) {
            //Konstrukcija B[l], rank0B[l] i Z[l]
            for (int i = 0; i < columns; ++i) {
                a = text->at(indeksi[i]);
                bit = values[a].at(l);
                B[l][i] = bit;
                if (i == 0) {
                    rank0B[l][i] = bit ? 0 : 1;
                }
                else {
                    rank0B[l][i] = rank0B[l][i - 1] + (bit ? 0 : 1);
                }
            }
            Z[l] = rank0B[l][columns - 1];
            //Konstrukcija select0_or_1B[l] i sortiranje indeksa (ako nije poslijednji redak)
            if (l < rows - 1) {
                vector<int> sortirani_indeksi(columns);
                int pos_0 = 0, pos_1 = Z[l];
                for (int i = 0; i < columns; ++i) {
                    if (!B[l][i]) {
                        sortirani_indeksi[pos_0] = indeksi[i];
                        select0_or_1B[l][pos_0++] = i + 1;
                    }
                    else {
                        sortirani_indeksi[pos_1] = indeksi[i];
                        select0_or_1B[l][pos_1++] = i + 1;
                    }
                }
                indeksi = move(sortirani_indeksi);
            }
            else {
                int pos_0 = 0, pos_1 = Z[l];
                for (int i = 0; i < columns; ++i) {
                    if (!B[l][i]) {
                        select0_or_1B[l][pos_0++] = i + 1;
                    }
                    else {
                        select0_or_1B[l][pos_1++] = i + 1;
                    }
                }
            }
        }
    }
    string bin_to_dec(string binary_number) {
        int dec_number = 0;
        for (int i = 0; i < binary_number.size(); i++) {
            dec_number += pow(2, i) * (binary_number.at(binary_number.size() - i - 1) - 48);
            //cout << "   " << dec_number << endl;
        }
        return to_string(dec_number);
    }
    int memoryUsage() const {
        return 4 * 7 + //int rows, columns, a, i, j, pos0 i pos1
            +8 * 2 // text i alphabet vector<int> pointeri
            + rows * (columns + 7) / 8 // Size of B
            + rows * columns * 4 // Size of rank0B
            + rows * columns * 4 // Size of select0_or_1B
            + rows * sizeof(int) // Size of Z
            + columns * 2; //indeksi i sortirani indexi
    }
    int f_rank0B(int l, int i) {
        if (i <= 0) return  0;
        return rank0B[l][i - 1];
    }
    int f_rank1B(int l, int i) {
        if (i <= 0) return  0;
        return i - rank0B[l][i - 1];
    }
    int f_select0_or_1B(int l, int i) {
        if (i <= 0) return 0;
        return select0_or_1B[l][i - 1];
    }
    int access(int i) {
        return access(0, i, "");
    }
    int access(int l, int i, string a) {
        if (i <= 0) return NULL;
        if (l == rows) {
            a = bin_to_dec(a);
            return alphabet->at(stoi(a));
        }
        if (B[l][i - 1] == 0) {
            a.push_back('0');
            i = f_rank0B(l, i);
        }
        else {
            a.push_back('1');
            i = Z[l] + f_rank1B(l, i);
        }
        //cout << "   access(" << l + 1 << ", " << i << ", " << a << ")" << endl;
        return access(l + 1, i, a);
    }
    int rank(int a, int i) {
        return rank(0, a, i, 0);
    }
    int rank(int l, int a, int i, int p) {
        if (l == rows) return i - p;
        if (values[a].at(l) == 0) {
            p = f_rank0B(l, p);
            i = f_rank0B(l, i);
        }
        else {
            p = Z[l] + f_rank1B(l, p);
            i = Z[l] + f_rank1B(l, i);
        }
        //cout << "   rank(" << l + 1 << ", " << (char)a << ", " << i << ", " << p << ")" << endl;
        return rank(l + 1, a, i, p);
    }
    int select(int a, int j) {
        if (j == 0) return NULL;
        return select(0, a, j, 0);
    }
    int select(int l, int a, int j, int p) {
        if (l == rows) return j + p;
        if (values[a].at(l) == 0) {
            p = f_rank0B(l, p);
            j = select(l + 1, a, j, p);
            //cout << "   ++l, a, j, p = " << l + 1 << ", " << (char)a << ", " << j << ", " << p << endl;
            if (f_select0_or_1B(l, j) == -1) return -1;
            return f_select0_or_1B(l, j);
        }
        else {
            p = Z[l] + f_rank1B(l, p);
            j = select(l + 1, a, j, p);
            //cout << "   ++l, a, j, p = " << l + 1 << ", " << (char)a << ", " << j << ", " << p << endl;
            if (f_select0_or_1B(l, j - Z[l]) == -1) return -1;
            return f_select0_or_1B(l, j);
        }
    }
    void print() {
        cout << "-------DEBUG----------" << endl;
        cout << "rows = " << rows << endl;
        cout << "columns = " << columns << endl;
        cout << "Z[] = ";
        copy(Z.begin(), Z.end(), ostream_iterator<int>(cout, " ")); cout << endl;
        cout << "B[][] = " << endl;
        for_each(B.begin(), B.end(), [](const vector<bool>& row) {
            copy(row.begin(), row.end(), ostream_iterator<bool>(cout, " "));
            cout << endl;
            });
        cout << "rank0B[][] = " << endl;
        for_each(rank0B.begin(), rank0B.end(), [](const vector<int>& row) {
            copy(row.begin(), row.end(), ostream_iterator<int>(cout, " "));
            cout << endl;
            });
        cout << "select0_or_1B[][] = " << endl;
        for_each(select0_or_1B.begin(), select0_or_1B.end(), [](const vector<int>& row) {
            copy(row.begin(), row.end(), ostream_iterator<int>(cout, " "));
            cout << endl;
            });
        cout << "alphabet = " << endl;
        copy(alphabet->begin(), alphabet->end(), ostream_iterator<int>(cout, " "));
        cout << endl;
        cout << "values = " << endl;
        for (auto i : values) {
            cout << (char)i.first << " -> ";
            copy(i.second.begin(), i.second.end(), ostream_iterator<bool>(cout, ""));
            cout << endl;
        }
        cout << "----------------------" << endl;
    }
};
int main(void) {
    SetConsoleOutputCP(CP_UTF8);
    vector<int> alphabet;
    vector<int> txt;
    //debuging txt = "AACGGTATTTAC" ; txt = "001234875491265898231" ; txt = "476532101417";
    //alphabet.push_back('A'); alphabet.push_back('C'); alphabet.push_back('G'); alphabet.push_back('T');

    int sigma;
    int txt_size;

    int result = 0;
    int values_construction_time = 0;
    int matrix_construction_time = 0;
    int sum_access_time = 0;
    int sum_rank_time = 0;
    int sum_select_time = 0;
    int access_op_counter = 0;
    int rank_op_counter = 0;
    int select_op_counter = 0;
    int memory = 0;
    auto beg = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    cout << "text_size,sigma,matrix_construction_time,values_map_construction_time,avg_access_time,avg_rank_time,avg_select_time,memory" << endl;
    for (int sigma_magnitude = 2; sigma_magnitude < 3; sigma_magnitude++) {
        for (int txt_magnitude = 1; txt_magnitude < 2; txt_magnitude++) {
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
            matrix_construction_time = 0;

            //sigma = sigma_magnitude;
            sigma = pow(2, sigma_magnitude);
            txt_size = pow(10, txt_magnitude);

            //txt_size = txt_magnitude;
            /*
            if (txt_magnitude < 1000000) {
                txt_magnitude += 100000;
            } else {
                txt_magnitude += 250000;
            }
            */
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
                copy(txt.begin(), txt.end(), ostream_iterator<int>(cout, ",")); cout << endl;
                //Stvaranje strukture s svim znakovima abecede i njihovim vrijednostima.
                beg = high_resolution_clock::now();
                int rows = ceil(log2(alphabet.size()));
                for (int i = 0; i < alphabet.size(); i++) {
                    vector<bool> bits(rows);
                    for (int j = 0; j < rows; ++j) {
                        bits[rows - 1 - j] = (i >> j) & 1;
                    }
                    values[alphabet.at(i)] = bits;
                }
                end = high_resolution_clock::now();
                values_construction_time += duration_cast<microseconds>(end - beg).count();
                beg = high_resolution_clock::now();
                Wavelet_matrix* m = new Wavelet_matrix(&txt, &alphabet);
                end = high_resolution_clock::now();
                matrix_construction_time += duration_cast<microseconds>(end - beg).count();
                memory += m->memoryUsage();
                //m->print();
                for (int j = 0; j < txt_size + 1; j++) {
                    beg = std::chrono::high_resolution_clock::now();
                    result = m->access(j);
                    end = std::chrono::high_resolution_clock::now();
                    sum_access_time += std::chrono::duration_cast<microseconds>(end - beg).count();
                    access_op_counter++;
                    //cout << "access(" << j << ")," << result << "," << std::chrono::duration_cast<microseconds>(end - beg).count() << endl;
                    if (access_op_counter % 100 == 0) break;
                }
                for (auto a : alphabet) {
                    for (int j = 0; j < txt_size + 1; j++) {
                        beg = high_resolution_clock::now();
                        result = m->rank(a, j);
                        end = high_resolution_clock::now();
                        sum_rank_time += duration_cast<microseconds>(end - beg).count();
                        rank_op_counter++;
                        //cout << "rank(" << a << ", " << j << ")," << result << "," << duration_cast<microseconds>(end - beg).count() << endl;
                        if (rank_op_counter % 100 == 0) break;
                    }
                    for (int j = 0; j < num_of_occurances[a] + 1; j++) {
                        beg = high_resolution_clock::now();
                        result = m->select(a, j); select_op_counter++;
                        end = high_resolution_clock::now();
                        sum_select_time += duration_cast<microseconds>(end - beg).count();
                        select_op_counter++;
                        cout << "select(" << a << ", " << j << ")," << result << "," << duration_cast<microseconds>(end - beg).count() << endl;
                        if (select_op_counter % 100 == 0) break;
                    }
                }
                delete(m);
            }
            cout << txt_size << ","
                << sigma << ","
                << ((matrix_construction_time - values_construction_time) / num_of_runs) << ","
                << (values_construction_time / num_of_runs) << ","
                << ((sum_access_time / access_op_counter))  << ","
                << ((sum_rank_time / rank_op_counter))  << ","
                << ((sum_select_time / select_op_counter)) << ","
                << (memory / num_of_runs)
                << endl;
        }
    }
}