//
// Created by Rainy Memory on 2021/3/4.
//

#ifndef TICKETSYSTEM_AUTOMATA_PARSER_H
#define TICKETSYSTEM_AUTOMATA_PARSER_H

#include "TokenScanner.h"

namespace RainyMemory {
    class Parser {
        /*
         * class Parser
         * --------------------------------------------------------
         * A class implements commands processing devices for train
         * ticket management system, using TokenScanner to manipulate
         * original command.
         * This class overloaded operator[] to access command arguments,
         * and operator() to transform argument into int.
         *
         * Usage:
         * string cmd;
         * Parser p;
         * p.resetBuffer(cmd);
         * p["-a"] will return cmd's -a argument
         *
         */
    public:
        enum commandType {
            ADDUSER, LOGIN, LOGOUT, QUERYPROFILE, MODIFYPROFILE, ADDTRAIN,
            RELEASETRAIN, QUERYTRAIN, DELETETRAIN, QUERYTICKET, QUERYTRANSFER,
            BUYTICKET, QUERYORDER, REFUNDTICKET, CLEAN, EXIT, NONE
        };
    
    private:
        TokenScanner ts;
        commandType type = NONE;
        int timeStamp;
        string argument[26];
        bool exist[26];
        
        const string commands[16] = {"query_profile", "query_ticket", "buy_ticket", "login", "logout",
                                     "modify_profile", "query_order", "add_user", "add_train", "release_train",
                                     "query_train", "delete_train", "query_transfer", "refund_ticket", "clean", "exit"};
        
        const commandType types[16] = {QUERYPROFILE, QUERYTICKET, BUYTICKET, LOGIN, LOGOUT,
                                       MODIFYPROFILE, QUERYORDER, ADDUSER, ADDTRAIN, RELEASETRAIN,
                                       QUERYTRAIN, DELETETRAIN, QUERYTRANSFER, REFUNDTICKET, CLEAN, EXIT};
        
        commandType getTypeFromString(const string &o) {
            for (int i = 0; i < 16; i++) {
                if (o == commands[i])return types[i];
            }
            return NONE;
        }
        
        void reset() {
            for (int i = 0; i < 26; i++)exist[i] = false;
            string ty = ts.nextToken();
            timeStamp = to_int(ty.substr(1, ty.size() - 2));
            ty = ts.nextToken();
            type = getTypeFromString(ty);
            while (!ts.empty()) {
                ty = ts.nextToken();
                argument[ty[1] - 'a'] = ts.nextToken();
                exist[ty[1] - 'a'] = true;
            }
        }
        
        static int to_int(const string &o) {
            int ret = 0;
            for (int i = 0; i < o.length(); i++)ret = ret * 10 + o[i] - '0';
            return ret;
        }
    
    public:
        Parser() : ts("") {}
        
        explicit Parser(const string &buffer) : ts(buffer) {
            reset();
        }
        
        void resetBuffer(const char * buffer) {
            ts.resetBuffer(buffer);
            reset();
        }
        
        commandType getType() const {
            return type;
        }
        
        int getTimeStamp() const {
            return timeStamp;
        }
        
        bool haveThisArgument(const char * arg) const {
            return exist[arg[1] - 'a'];
        }
        
        const string &operator[](const char * arg) const {
            if (!exist[arg[1] - 'a'])return argument[25];
            return argument[arg[1] - 'a'];
        }
        
        int operator()(const char * arg) const {
            if (!exist[arg[1] - 'a'])return -1;
            return to_int(argument[arg[1] - 'a']);
        }
    };
}

#endif //TICKETSYSTEM_AUTOMATA_PARSER_H
