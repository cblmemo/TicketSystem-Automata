#include "BPlusTree.h"
#include <map>

using namespace std;

void init() {
    fstream o;
    o.open("leaf_test.dat", ios::out | ios::binary);
    o.close();
    o.open("internal_test.dat", ios::out | ios::binary);
    o.close();
}

int main() {
    map<BPlusTreeString, vector<int>> answer;
    init();
    srand(1213);
    string s = "aa";
    BPlusTree<BPlusTreeString, int> temp("test");
    for (int i = 0; i < 100000; i++) {
        s = to_string(rand());
        BPlusTreeString tempBPTSTR(s);
        answer[tempBPTSTR].push_back(i);
        temp.insert(tempBPTSTR, i);
        if (i % 1000 == 0)cout << i << endl;
    }
    
    for (const auto &i:answer) {
        vector<int> result;
        temp.find(i.first, result);
        sort(result.begin(),result.end());
        if (i.second.size() != result.size()) {
            cerr << "wrong size!" << endl;
            cerr << "key: " << i.first << endl;
            cerr << "answer size: " << i.second.size() << endl;
            cerr << "answer:" << endl;
            for (auto j : i.second)cerr << j << " ";
            cerr << endl;
            cerr << "result size: " << result.size() << endl;
            continue;
        }
        for (int j = 0; j < i.second.size(); j++) {
            if (i.second[j] != result[j]) {
                cerr << "data: " << i.second[j] << endl;
                cerr << "your data: " << result[j] << endl;
            }
        }
    }
    //temp.show();
    return 0;
}
