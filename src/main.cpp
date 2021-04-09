//
// Created by Rainy Memory on 2021/3/4.
//

//#include "BPlusTree.h"
//#include "BPlusTree_SingleKeyVersion.h"
#include "LRUCacheMemoryPool.h"
#include "Parser.h"
#include "HashType.h"
#include <algorithm>
#include <sstream>

using namespace RainyMemory;
using std::cin;
using std::cout;
using std::endl;

class t {
public:
    int a = 0;
    int offset = 0;
    
    t() = default;
    
    t(int a_, int b_) : a(a_), offset(b_) {}
    
    void show() {
        cout << "a: " << a << " offset: " << offset << endl;
    }
};

void commandLineMemoryPool() {
    LRUCacheMemoryPool<t, int> mp("test.dat");
    string cmd;
    while (cin >> cmd) {
        int a, offset;
        if (cmd == "insert") {
            cin >> a;
            t tt(a, mp.tellWritePoint());
            tt.show();
            mp.write(tt);
        }
        else if (cmd == "visit") {
            cin >> offset;
            t tt = mp.read(offset);
            tt.show();
        }
        else if (cmd == "update") {
            cin >> a >> offset;
            t tt(a, offset);
            mp.update(tt, offset);
        }
        else if (cmd == "erase") {
            cin >> offset;
            mp.erase(offset);
        }
        else if (cmd == "exit") {
            break;
        }
        else cout << "Invalid." << endl;
    }
}

int main() {
//    std::string s;
//    getline(std::cin, s);
//    Parser ts(s);
//    while (std::cin >> s) {
//        cout << ts[s] << endl;
//    }
//    commandLineMemoryPool();
    string s;
    getline(cin, s);
    Parser p(s);
    cout << p["-a"] << endl;
    return 0;
}






