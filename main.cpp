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
//    BPlusTree<BPlusTreeString, int> temp("1");
//    temp.test();
//    temp.size();
//    temp.empty();
//    BPlusTreeString t1("ptilopsis");
//    int t2 = 0;
//    temp.insert(t1, t2);
//    temp.find(t1);
//    temp.erase(t1,t2);
//    vector<int> s;
    map<BPlusTreeString, vector<int>> answer;
    init();
    srand(1213);
    string s="aa";
    BPlusTree<BPlusTreeString, int> temp("test");
//    for (int i = 0; i < 100; i++) {
//        s = to_string(rand());
//        BPlusTreeString tempBPTSTR(s);
//        answer[tempBPTSTR].push_back(i);
//        temp.insert(tempBPTSTR, i);
//    }
//
//    for (const auto &i:answer) {
//        vector<int> result;
//        temp.find(i.first, result);
//        if (i.second.size() != result.size()) {
//            cerr << "wrong size!" << endl;
//            cerr << "key: " << i.first << endl;
//            cerr << "answer size: " << i.second.size() << endl;
//            cerr << "ansewr:" << endl;
//            for (auto j : i.second)cerr << j << " ";
//            cerr << endl;
//            cerr << "result size: " << result.size() << endl;
//            continue;
//        }
//        for (int j = 0; j < i.second.size(); j++) {
//            if (i.second[j] != result[j]) {
//                cerr << "data: " << i.second[j] << endl;
//                cerr << "your data: " << result[j] << endl;
//            }
//        }
//    }
    BPlusTreeString ss(s);
    for(int i=0;i<37;i++){
        temp.insert(ss,i);
        //temp.show();
    }
    temp.show();
    temp.insert(ss,500);
    temp.insert(ss,600);
    temp.insert(ss,700);
    temp.insert(ss,800);
    vector<int> res;
    temp.find(ss,res);
    for(auto i:res)cout<<i<<" ";
    cout<<endl;
    temp.show();
    return 0;
}
