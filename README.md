Note: this is more for demonstaration than for an acctual use and therefor the source code is lacking any user interface or input.
For text size of 16M cahacthers and alphabet of 1000 elements it uses around 1800MB of RAM.

How to use Wavelet_tree.cpp:
In Main create a vector<int> for your text and a vector<int> for your alphabet.
Create a wavelet tree using: Tree* t = new Tree(&txt, &alphabet);
Simply get the result of any querry with t -> querry_name(arg1, arg2...).
Examples: t->access(4);, t->rank(1, 10);, t->select(2, 3);
Note: there is nothing stopping you for imputing queries with out of bounds elements or querries that don't make sense. Example t->select(1, 4), when there are less then 4 occurances of charachter 1.

How to use Wavelet_tree.cpp for compression:
Tree* t = new Tree(&txt, &alphabet, true)
tree now contains a compressed string, you can delete *txt now.
For decompression use t->accessCompressed(i) to access any element.

How to use Wavelet_matrix.cpp:
In Main create a vector<int> for your text and a vector<int> for your alphabet.
Create a wavelet tree using: Wavelet_matrix* m = new Wavelet_matrix(&txt, &alphabet);
Simply get the result of any querry with m -> querry_name(arg1, arg2...).
Examples: m->access(4);, m->rank(1, 10);, m->select(2, 3);
Note: there is nothing stopping you for imputing queries with out of bounds elements or querries that don't make sense. Example t->select(1, 4), when there are less then 4 occurances of charachter 1.
