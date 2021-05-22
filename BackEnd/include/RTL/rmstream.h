//
// Created by Rainy Memory on 2021/5/22.
//

#ifndef CODE_RMSTREAM_H
#define CODE_RMSTREAM_H

#include <cstdio>
#include <cstring>
#include <iostream>

namespace RainyMemory {
    class rmstream {
    private:
        char buffer[30];
        int ptr = 0;
    
    public:
        rmstream &operator<<(int msg) {
            if (msg < 0)msg = -msg, buffer[ptr++] = '-';
            while (msg > 0)buffer[ptr++] = (msg % 10) + '0', msg /= 10;
            if (ptr == 0)buffer[ptr++] = '0';
            while (ptr > 0)putchar(buffer[--ptr]);
            return *this;
        }
        
        rmstream &operator<<(char msg) {
            putchar(msg);
            return *this;
        }
        
        rmstream &operator<<(const char *msg) {
            int l = strlen(msg);
            for (int i = 0; i < l; i++)putchar(msg[i]);
            return *this;
        }
        
        rmstream &operator<<(long long msg) {
            if (msg < 0)msg = -msg, buffer[ptr++] = '-';
            while (msg > 0)buffer[ptr++] = (msg % 10) + '0', msg /= 10;
            if (ptr == 0)buffer[ptr++] = '0';
            while (ptr > 0)putchar(buffer[--ptr]);
            return *this;
        }
        
        rmstream &operator<<(const std::string &msg) {
            int l = msg.length();
            for (int i = 0; i < l; i++)putchar(msg[i]);
            return *this;
        }
    };
}

#endif //CODE_RMSTREAM_H
