//
// Created by Rainy Memory on 2021/3/4.
//

#ifndef TICKETSYSTEM_AUTOMATA_TOKENSCANNER_H
#define TICKETSYSTEM_AUTOMATA_TOKENSCANNER_H

#include <iostream>

using std::string;

namespace RainyMemory {
    class TokenScanner {
        /*
         * class TokenScanner
         * --------------------------------------------------------
         * A class implements string processing devices like
         * std::stringstream. This class support custom delimitation
         * and stream manipulate operator, and built-in string to int
         * transformation.
         *
         */
    private:
        string Buffer;
        char Delim = ' ';
        int pos = 0;
        
        static int to_int(const string &o) {
            int res = 0;
            for (char i:o)res = (res << 3) + (res << 1) + i - '0';
            return res;
        }
        
        inline bool delimJudge(char c) {
            return c == Delim || c == '\n' || c == '\r';
        }
    
    public:
        TokenScanner() = default;
        
        explicit TokenScanner(const string &buffer, char delim = ' ') : Buffer(buffer), Delim(delim), pos(0) {
            while (delimJudge(Buffer[pos]))pos++;
        }
        
        string nextToken() {
            if (hasMoreTokens()) {
                int next = pos, _pos = pos;
                while (!delimJudge(Buffer[next]) && next < Buffer.length())next++;
                pos = next + 1;
                if (pos < Buffer.length())
                    while (delimJudge(Buffer[pos]))pos++;
                return Buffer.substr(_pos, next - _pos);
            }
            else return "";
        }
        
        int nextIntToken() {
            return to_int(nextToken());
        }
        
        bool empty() const {
            return pos >= Buffer.length();
        }
        
        bool hasMoreTokens() const {
            return pos < Buffer.length();
        }
        
        void resetBuffer(const char *buffer) {
            Buffer = buffer;
            pos = 0;
            while (delimJudge(Buffer[pos]))pos++;
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
            o = to_int(s);
            return *this;
        }
    };
    
}


#endif //TICKETSYSTEM_AUTOMATA_TOKENSCANNER_H
