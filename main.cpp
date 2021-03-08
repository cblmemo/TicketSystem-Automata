//
// Created by Rainy Memory on 2021/3/4.
//

//#include "BPlusTree.h"
#include "BPlusTree_SingleKeyVersion.h"
#include "Parser.h"
#include "HashType.h"

using namespace std;

int main() {
    string s;
    getline(std::cin, s);
    RainyMemory::TokenScanner ts(s);
    ts << s;
    while (ts.hasMoreTokens()) {
        ts >> s;
        HashType ht(s);
        ht.print();
    }
    return 0;
}
