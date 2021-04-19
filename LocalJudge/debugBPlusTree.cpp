#include "BPlusTree.h"
#include <map>
#include <sstream>

using namespace std;
using RainyMemory::BPlusTree;

#define TESTSIZE 100000

void init() {
    fstream o;
    o.open("leaf_test.dat", ios::out | ios::binary);
    o.close();
    o.open("internal_test.dat", ios::out | ios::binary);
    o.close();
}

void testInsertFind(int seed) {
    map<int, vector<int>> answer;
    vector<int> store;
    init();
    srand(seed);
    int s;
    BPlusTree<int, int> temp("LocalJudge");
    for (int i = 0; i < TESTSIZE; i++) {
        s = rand();
        store.push_back(s);
        answer[store[i]].push_back(i);
        temp.insert(s, i);
        if (i % (TESTSIZE / 100) == 0)cout << "[count down] Inserting...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    
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
        }
        else {
            for (int j = 0; j < i.second.size(); j++) {
                if (i.second[j] != result[j]) {
                    cerr << "[error]wrong answer." << endl;
                    cerr << "data: " << i.second[j] << endl;
                    cerr << "your data: " << result[j] << endl;
                }
            }
        }
        if (cnt % (size / 100) == 0)cout << "[count down] Checking...\t" << cnt * 100 / size + 1 << "%" << endl;
        cnt++;
    }
    //temp.showLeaves();
}

void testIntAll() {
    map<int, vector<int>> answer;
    vector<int> store;
    init();
    srand(22);
    int s;
    BPlusTree<int, int> temp("LocalJudge");
    for (int i = 0; i < TESTSIZE; i++) {
        s = rand();
        store.push_back(s);
        temp.insert(s, i);
        if (i % (TESTSIZE / 100) == 0)cout << "[count down] Inserting...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    cout << endl;
    
    for (int i = 0; i < TESTSIZE; i++) {
        if (i % 2 == 0) {
            answer[store[i]].push_back(i);
        }
        if (i % (TESTSIZE / 100) == 0)cout << "[count down] Pushing...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    cout << endl;
    
    for (int i = 0; i < TESTSIZE; i++) {
        if (i % 2 == 1) {
            if (!temp.erase(store[i], i))cerr << "[error]erase failed when erasing\nkey: " << store[i] << "\tdata: " << i << endl;
        }
        if (i % (TESTSIZE / 100) == 0)cout << "[count down] Erasing...\t" << i * 100 / TESTSIZE + 1 << "%" << endl;
    }
    cout << endl;

//    int testNumber = 30325;
//    for (int i = 1; i < testNumber; i += 2) {
//        int t = store[i];
//        if (!temp.erase(t, i))cerr << "[error]erase failed." << endl;
//        if (i % (TESTSIZE / 100) == 1)cout << "Erasing...\t" << (i - 1) * 100 / TESTSIZE + 1 << "%" << endl;
//        //if((i - 1) * 100 / TESTSIZE>=24)cout<<i<<endl;
//    }
//    int ttt = store[testNumber];
//    temp.erase(ttt, testNumber);
    
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
        }
        else {
            for (int j = 0; j < i.second.size(); j++) {
                if (i.second[j] != result[j]) {
                    cerr << "[error]wrong answer." << endl;
                    cerr << "data: " << i.second[j] << endl;
                    cerr << "your data: " << result[j] << endl;
                }
            }
        }
        if (cnt % (size / 100) == 0)cout << "[count down] Checking...\t" << cnt * 100 / size + 1 << "%" << endl;
        cnt++;
    }
    cout << endl;
    //temp.showLeaves();
    
    temp.size();
    temp.empty();
    temp.clear();
    vector<int> tt;
    temp.traversal(tt);
    for (auto i:tt)cerr << "[error] clear failed." << endl;
    temp.showLeaves();
}

void testRepeatedInsert() {
    init();
    int a = 100;
    BPlusTree<int, int> temp("LocalJudge");
    for (int i = 0; i < TESTSIZE; i++) {
        temp.insert(a, i);
    }
    vector<int> tt;
    temp.find(a, tt);
    if (tt.size() != TESTSIZE)cerr << "[error] size wrong!" << endl;
    else {
        sort(tt.begin(), tt.end());
        int cnt = 0;
        for (auto i: tt) {
            if (i != cnt)cerr << "[error] element wrong!" << endl;
            cnt++;
        }
    }
    
}

void commandLine() {
//    init();
    BPlusTree<int, int> bpt("LocalJudge");
    string cmd;
    while (getline(cin, cmd)) {
        if (cmd == "exit")break;
        stringstream ss(cmd);
        string cmdType;
        ss >> cmdType;
        if (cmdType == "insert") {
            int key, offset;
            ss >> key >> offset;
            bpt.insert(key, offset);
            cout << "Insert successful." << endl;
        }
        else if (cmdType == "erase") {
            int key, offset;
            ss >> key >> offset;
            if (bpt.erase(key, offset))cout << "Erase successful." << endl;
            else cout << "Erase failed." << endl;
        }
        else if (cmdType == "find") {
            int key;
            ss >> key;
            vector<int> result;
            bpt.find(key, result);
            if (result.empty()) {
                cout << "No result." << endl;
            }
            else {
                cout << "[result]" << endl;
                for (int i: result) {
                    cout << i << " ";
                }
                cout << endl;
            }
        }
        else if (cmdType == "show") {
            bpt.showLeaves();
        }
        else if (cmdType == "size") {
            if (bpt.empty())cout << "BPT is empty." << endl;
            else cout << "[size] " << bpt.size() << endl;
        }
        else if (cmdType == "traversal") {
            if (bpt.empty())cout << "BPT is empty." << endl;
            else {
                vector<int> result;
                bpt.traversal(result);
                cout << "[traversal]" << endl;
                for (int i:result) {
                    cout << i << " ";
                }
                cout << endl;
            }
        }
        else if (cmdType == "showtree") {
            bpt.show();
        }
        else if (cmdType == "clear") {
            bpt.clear();
            cout << "Clear successful." << endl;
        }
        else cout << "Invalid" << endl;
    }
}


int main() {
    //testBPlusTreeString();
    //testInsertFind(21);
    //testIntAll();
    //testRepeatedInsert();
    commandLine();
    //testMemoryPool();
    return 0;
}
