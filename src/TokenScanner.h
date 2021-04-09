//
// Created by Rainy Memory on 2021/3/4.
//

#ifndef BPLUSTREE_TOKENSCANNER_H
#define BPLUSTREE_TOKENSCANNER_H

#include <iostream>

using std::string;

namespace RainyMemory {
    class TokenScanner {
    private:
        string Buffer;
        char Delim = ' ';
        int pos = 0;
        
        static int toInt(const string &o) {
            int res = 0;
            for (char i:o)res = (res << 3) + (res << 1) + i - '0';
            return res;
        }
    
    public:
        TokenScanner() = default;
        
        explicit TokenScanner(const string &buffer, char delim = ' ') : Buffer(buffer), Delim(delim), pos(0) {
            while (Buffer[pos] == Delim)pos++;
        }
        
        string nextToken() {
            if (hasMoreTokens()) {
                int next = pos, _pos = pos;
                while (Buffer[next] != Delim && next < Buffer.length())next++;
                pos = next + 1;
                if (pos < Buffer.length())while (Buffer[pos] == Delim)pos++;
                return Buffer.substr(_pos, next - _pos);
            }
            else return "";
        }
        
        bool empty() const {
            return pos >= Buffer.length();
        }
        
        bool hasMoreTokens() const {
            return pos < Buffer.length();
        }
        
        void resetBuffer(const string &buffer) {
            Buffer = buffer;
            pos = 0;
            while (Buffer[pos] == Delim)pos++;
        }
        
        void resetDelim(char delim) {
            Delim = delim;
        }
        
        void clear() {
            Buffer.clear();
            Delim = ' ';
            pos = 0;
        }
        
        TokenScanner &operator>>(string &o) {
            o = nextToken();
            return *this;
        }
        
        TokenScanner &operator>>(int &o) {
            string s = nextToken();
            o = toInt(s);
            return *this;
        }
    };
    
}


#endif //BPLUSTREE_TOKENSCANNER_H
