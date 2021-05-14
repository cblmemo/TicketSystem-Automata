//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_EXCEPTIONS_H
#define TICKETSYSTEM_AUTOMATA_EXCEPTIONS_H

/*
 * exceptions.h
 * --------------------------------------------------------
 * A header implements class exception for [vector.h].
 *
 */

#include <cstring>
#include <iostream>

namespace RainyMemory {
    class exception : public std::exception {
    protected:
        const std::string variant;
        std::string detail;
    public:
        exception() = default;
        
        exception(const exception &ec) = default;
        
        virtual std::string what() {
            return variant + " " + detail;
        }
    };
    
    class index_out_of_bound : public exception {
        /* __________________________ */
    };
    
    class runtime_error : public exception {
        /* __________________________ */
    };
    
    class invalid_iterator : public exception {
        /* __________________________ */
    };
    
    class container_is_empty : public exception {
        /* __________________________ */
    };
}

#endif //TICKETSYSTEM_AUTOMATA_EXCEPTIONS_H
