//
// Created by Rainy Memory on 2021/2/3.
//

#include "BPlusTree_SingleKeyVersion.h"
#include <map>
#include <sstream>

using namespace std;
using RainyMemory::BPlusTree;

void init() {
    fstream o;
    o.open("leaf_test.dat", ios::out | ios::binary);
    o.close();
    o.open("internal_test.dat", ios::out | ios::binary);
    o.close();
}

void commandLine() {
//    init();
    BPlusTree<int, int> bpt("test");
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
            int key;
            ss >> key;
            if (bpt.erase(key))cout << "Erase successful." << endl;
            else cout << "Erase failed." << endl;
        }
        else if (cmdType == "find") {
            int key;
            ss >> key;
            int result = bpt.find(key);
            if (result<0) {
                cout << "No result." << endl;
            }
            else {
                cout << "[result] " <<result<< endl;
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
    commandLine();
    return 0;
}