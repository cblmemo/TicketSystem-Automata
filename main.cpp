#include "BPlusTree.h"
#include <map>

using namespace std;

#define TESTSIZE 10000

void init() {
    fstream o;
    o.open("leaf_test.dat", ios::out | ios::binary);
    o.close();
    o.open("internal_test.dat", ios::out | ios::binary);
    o.close();
}

int main() {
    map<BPlusTreeString, vector<int>> answer;
    vector<BPlusTreeString> store;
    init();
    srand(2333);
    string s;
    BPlusTree<BPlusTreeString, int> temp("test");
    for (int i = 0; i < TESTSIZE; i++) {
        s = to_string(rand());
        BPlusTreeString tempBPTSTR(s);
        store.push_back(tempBPTSTR);
        temp.insert(tempBPTSTR, i);
        if (i % (TESTSIZE / 100) == 0)cout << "Inserting...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    
    for (int i = 0; i < TESTSIZE; i += 2) {
        answer[store[i]].push_back(i);
        if (i % (TESTSIZE / 100) == 0)cout << "Pushing...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    
    for (int i = 1; i < TESTSIZE; i += 2) {
        if(!temp.erase(store[i], i))cerr<<"[error]erase failed."<<endl;
        if (i % (TESTSIZE / 100) == 1)cout << "Erasing...\t" << (i - 1) * 100 / TESTSIZE + 1 << "%" << endl;
    }

//    vector<int> result;
//    s = "22592";
//    BPlusTreeString bpts(s);
//    temp.find(bpts,result);
    
    int cnt = 0;
    int size = answer.size();
    for (const auto &i:answer) {
        vector<int> result;
        temp.find(i.first, result);
        sort(result.begin(), result.end());
        if (i.second.size() != result.size()) {
            cerr << "[error]wrong size." << endl;
            cerr << "key: " << i.first << endl;
            cerr << "answer size: " << i.second.size() << endl;
            cerr << "answer:" << endl;
            for (auto j : i.second)cerr << j << " ";
            cerr << endl;
            cerr << "result size: " << result.size() << endl;
            cerr << "result:" << endl;
            for (auto j : result)cerr << j << " ";
            cerr << endl;
            continue;
        }
        for (int j = 0; j < i.second.size(); j++) {
            if (i.second[j] != result[j]) {
                cerr<<"[error]wrong answer."<<endl;
                cerr << "data: " << i.second[j] << endl;
                cerr << "your data: " << result[j] << endl;
            }
        }
        if (cnt % (size / 100) == 0)cout << "Checking...\t" << cnt * 100 / size + 1 << "%" << endl;
        cnt++;
    }
    //temp.showLeaves();
    return 0;
}
