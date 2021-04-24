//
// Created by Rainy Memory on 2021/4/13.
//

#include "Administrator.h"

void Administrator::initialize(std::ostream &os) {
    Ptilopsis = new Parser;
    Saria     = new UserManager(UserIndexPath, UserStoragePath, os);
    Silence   = new TrainManager(TrainIndexPath, TrainStoragePath, TrainStationPath, os);
    Ifrit     = new OrderManager(Saria, Silence, OrderIndexPath, OrderStoragePath, OrderPendingPath, os);
}

void Administrator::clean() {
    Saria   -> clear();
    Silence -> clear();
    Ifrit   -> clear();
}

Administrator::Administrator() = default;

Administrator::~Administrator() {
    delete Ptilopsis;
    delete Saria;
    delete Silence;
    delete Ifrit;
}

void Administrator::runProgramme(std::istream &is, std::ostream &os) {
#ifdef speedup
    std::ios::sync_with_stdio(false);
#endif
    initialize(os);
    string cmd;
    bool flag = true;
    while (flag && getline(is, cmd)) {
        Ptilopsis->resetBuffer(cmd);
        switch (Ptilopsis->getType()) {
            case Parser::ADDUSER:
                Saria   -> addUser(*Ptilopsis);
                break;
            case Parser::LOGIN:
                Saria   -> login(*Ptilopsis);
                break;
            case Parser::LOGOUT:
                Saria   -> logout(*Ptilopsis);
                break;
            case Parser::QUERYPROFILE:
                Saria   -> queryProfile(*Ptilopsis);
                break;
            case Parser::MODIFYPROFILE:
                Saria   -> modifyProfile(*Ptilopsis);
                break;
            case Parser::ADDTRAIN:
                Silence -> addTrain(*Ptilopsis);
                break;
            case Parser::RELEASETRAIN:
                Silence -> releaseTrain(*Ptilopsis);
                break;
            case Parser::QUERYTRAIN:
                Silence -> queryTrain(*Ptilopsis);
                break;
            case Parser::DELETETRAIN:
                Silence -> deleteTrain(*Ptilopsis);
                break;
            case Parser::QUERYTICKET:
                Silence -> queryTicket(*Ptilopsis);
                break;
            case Parser::QUERYTRANSFER:
                Silence -> queryTransfer(*Ptilopsis);
                break;
            case Parser::BUYTICKET:
                Ifrit   -> buyTicket(*Ptilopsis);
                break;
            case Parser::QUERYORDER:
                Ifrit   -> queryOrder(*Ptilopsis);
                break;
            case Parser::REFUNDTICKET:
                Ifrit   -> refundTicket(*Ptilopsis);
                break;
            case Parser::CLEAN:
                clean();
                os << "0" << endl;
                break;
            case Parser::EXIT://exit
                flag = false;
                os << "bye" << endl;
                break;
            default:
                os << "[Error]Invalid command." << endl;
                break;
        }
    }
}

#ifdef debug

void Administrator::analyzeData(std::istream &is, std::ostream &os) {
    Ptilopsis = new Parser;
    string cmd;
    int cnt[17] = {0};
    string commands[17] = {"add_user", "login", "logout", "query_profile", "modify_profile", "add_train", "release_train", "query_train",
                           "delete_train", "query_ticket", "query_transfer", "buy_ticket", "query_order", "refund_ticket", "clean", "exit", "none"};
    while (getline(is, cmd)) {
        Ptilopsis->resetBuffer(cmd);
        if (Ptilopsis->getType() == Parser::EXIT)break;
        cnt[Ptilopsis->getType()]++;
    }
    for (int i = 0; i < 17; i++) {
        os << "Command [" << commands[i] << "] appears [" << cnt[i] << "] times." << endl;
    }
}

#endif