//
// Created by Rainy Memory on 2021/4/13.
//

#include "Administrator.h"

Administrator::Administrator() {
    Ptilopsis = new Parser;
    Saria = new UserManager(UserIndexPath, UserStoragePath);
//    Silence = new TrainManager;
//    Ifrit = new OrderManager;
}

Administrator::~Administrator() {
    delete Ptilopsis;
    delete Saria;
    delete Silence;
    delete Ifrit;
}

void Administrator::runProgramme(std::istream &is) {
    string cmd;
    bool flag = true;
    while (getline(is, cmd) && flag) {
        Ptilopsis->resetBuffer(cmd);
        switch (Ptilopsis->getType()) {
            case Parser::ADDUSER:
                Saria->addUser(*Ptilopsis);
                break;
            case Parser::LOGIN:
                Saria->login(*Ptilopsis);
                break;
            case Parser::LOGOUT:
                Saria->logout(*Ptilopsis);
                break;
            case Parser::QUERYPROFILE:
                Saria->queryProfile(*Ptilopsis);
                break;
            case Parser::MODIFYPROFILE:
                Saria->modifyProfile(*Ptilopsis);
                break;
            default:
                flag = false;
                break;
        }
    }
}
