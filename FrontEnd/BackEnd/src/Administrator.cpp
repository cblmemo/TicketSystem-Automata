//
// Created by Rainy Memory on 2021/4/13.
//

#include "Administrator.h"
#include <thread> // std::this_thread

void Administrator::initialize() {
    Ptilopsis = new Parser;
    Saria = new UserManager(UserIndexPath, UserStoragePath);
    Silence = new TrainManager(TrainIndexPath, TrainStoragePath, TrainStationPath);
    Ifrit = new OrderManager(Saria, Silence, OrderIndexPath, OrderPendingPath);
}

void Administrator::clean() {
    Saria->clear();
    Silence->clear();
    Ifrit->clear();
}

Administrator::Administrator() = default;

Administrator::~Administrator() {
    delete Ptilopsis;
    delete Saria;
    delete Silence;
    delete Ifrit;
}

std::string Administrator::process(const std::string &cmd) {
    Ptilopsis->resetBuffer(cmd);
    switch (Ptilopsis->getType()) {
        case Parser::ADDUSER:
            return Saria->addUser(*Ptilopsis);
        case Parser::LOGIN:
            return Saria->login(*Ptilopsis);
        case Parser::LOGOUT:
            return Saria->logout(*Ptilopsis);
        case Parser::QUERYPROFILE:
            return Saria->queryProfile(*Ptilopsis);
        case Parser::MODIFYPROFILE:
            return Saria->modifyProfile(*Ptilopsis);
        case Parser::ADDTRAIN:
            return Silence->addTrain(*Ptilopsis);
        case Parser::RELEASETRAIN:
            return Silence->releaseTrain(*Ptilopsis);
        case Parser::QUERYTRAIN:
            return Silence->queryTrain(*Ptilopsis);
        case Parser::DELETETRAIN:
            return Silence->deleteTrain(*Ptilopsis);
        case Parser::QUERYTICKET:
            return Silence->queryTicket(*Ptilopsis);
        case Parser::QUERYTRANSFER:
            return Silence->queryTransfer(*Ptilopsis);
        case Parser::BUYTICKET:
            return Ifrit->buyTicket(*Ptilopsis);
        case Parser::QUERYORDER:
            return Ifrit->queryOrder(*Ptilopsis);
        case Parser::REFUNDTICKET:
            return Ifrit->refundTicket(*Ptilopsis);
        case Parser::CLEAN:
            clean();
            return "0\n";
        case Parser::EXIT:
            return "bye\n";
        default:
            return "[Error]Invalid command.\n";
    }
}

int Administrator::runProgramme() {
    initialize();
    string cmd;
//    std::cout << "BackEnd: \033[32mBackEnd Initialization Finished.\033[0m" << std::endl;
    bool flag = true;
    while (flag) {
        getline(std::cin, cmd);
//        std::cout << "BackEnd: \033[;36m<< Handling Operation: \033[35m\"" << cmd << "\"\033[0m" << std::endl;
        std::string result = process(cmd);
        if (result == "bye\n")flag = false;
        if (result == "[Error]Invalid command.\n")return 1;
        std::cout << result;
    }
    return 0;
}
