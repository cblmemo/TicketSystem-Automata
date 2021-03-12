//
// Created by Rainy Memory on 2021/3/4.
//

//#include "BPlusTree.h"
#include "BPlusTree_SingleKeyVersion.h"
#include "Parser.h"
#include "HashType.h"
#include <algorithm>

using namespace RainyMemory;
int a[1000000] = {0};

int main() {
//    std::string s;
//    getline(std::cin, s);
//    Parser ts(s);
//    while (std::cin >> s) {
//        cout << ts[s] << endl;
//    }
    srand(1000000);
    int n;
    std::cin >> n;
    for (int i = 0; i < n; i++)a[i] = rand() % 10000;
//    for (int i = 0; i < n; i++)std::cin >> a[i];
//    for (int i = 0; i < n; i++)cout << a[i] << " ";
//    cout << endl;
    sort(a, 0, n - 1);
//    for (int i = 0; i < n; i++)cout << a[i] << " ";
//    cout << endl;
    for (int i = 0; i < n - 1; i++) {
        if (a[i] > a[i + 1])cout << "error" << endl;
    }
    return 0;
}






