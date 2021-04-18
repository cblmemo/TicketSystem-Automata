//
// Created by Rainy Memory on 2021/4/15.
//

#ifndef TICKETSYSTEM_AUTOMATA_GLOBAL_H
#define TICKETSYSTEM_AUTOMATA_GLOBAL_H

#include <iostream>
#include "../include/RTL/string_t.h"

//#define speedup
#define debug

#ifdef speedup
const char endl = '\n';
#else
using std::endl;
#endif

class exception_t : public std::exception {
};

class inexistUser : public exception_t {
};

constexpr int lengthOfChineseCharacters(int length) {
    return length * 4;
}

#endif //TICKETSYSTEM_AUTOMATA_GLOBAL_H