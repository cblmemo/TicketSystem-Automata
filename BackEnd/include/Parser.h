//
// Created by Rainy Memory on 2021/3/4.
//

#ifndef TICKETSYSTEM_AUTOMATA_PARSER_H
#define TICKETSYSTEM_AUTOMATA_PARSER_H

#include "TokenScanner.h"

class Parser {
public:
    enum commandType {
        ADDUSER, LOGIN, LOGOUT, QUERYPROFILE, MODIFYPROFILE, ADDTRAIN,
        RELEASETRAIN, QUERYTRAIN, DELETETRAIN, QUERYTICKET, QUERYTRANSFER,
        BUYTICKET, QUERYORDER, REFUNDTICKET, CLEAN, EXIT, NONE
    };
private:
    RainyMemory::TokenScanner ts;
    commandType type = NONE;
    string argument[26];
    
    void reset() {
        for (string i:argument)i.clear();
        string ty = ts.nextToken();
        if (ty == "add_user")type = ADDUSER;                     //N
        else if (ty == "login")type = LOGIN;                     //F
        else if (ty == "logout")type = LOGOUT;                   //F
        else if (ty == "query_profile")type = QUERYPROFILE;      //SF
        else if (ty == "modify_profile")type = MODIFYPROFILE;    //F
        else if (ty == "add_train")type = ADDTRAIN;              //N
        else if (ty == "release_train")type = RELEASETRAIN;      //N
        else if (ty == "query_train")type = QUERYTRAIN;          //N
        else if (ty == "delete_train")type = DELETETRAIN;        //N
        else if (ty == "query_ticket")type = QUERYTICKET;        //SF
        else if (ty == "query_transfer")type = QUERYTRANSFER;    //N
        else if (ty == "buy_ticket")type = BUYTICKET;            //SF
        else if (ty == "query_order")type = QUERYORDER;          //F
        else if (ty == "refund_ticket")type = REFUNDTICKET;      //N
        else if (ty == "clean")type = CLEAN;                     //R
        else if (ty == "exit")type = EXIT;                       //R
        else type = NONE;
        while (!ts.empty()) {
            ty = ts.nextToken();
            argument[ty[1] - 'a'] = ts.nextToken();
        }
    }

public:
    Parser() : ts("") {}
    
    explicit Parser(const string &buffer) : ts(buffer) {
        reset();
    }
    
    void resetBuffer(const string &buffer) {
        ts.resetBuffer(buffer);
        reset();
    }
    
    commandType getType() const {
        return type;
    }
    
    const string &operator[](const string &arg) const {
        return argument[arg[1] - 'a'];
    }
};

#endif //TICKETSYSTEM_AUTOMATA_PARSER_H
