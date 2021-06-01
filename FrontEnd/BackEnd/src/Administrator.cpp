//
// Created by Rainy Memory on 2021/4/13.
//

#include "Administrator.h"

void Administrator::clean() {
    Saria->clear();
    Silence->clear();
    Ifrit->clear();
}

Administrator::Administrator() {
    Ptilopsis = new Parser;
    Saria = new UserManager(UserIndexPath, UserStoragePath);
    Silence = new TrainManager(TrainIndexPath, TrainStoragePath, TrainStationPath);
    Ifrit = new OrderManager(Saria, Silence, OrderIndexPath, OrderPendingPath);
}

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

void Administrator::runProgramme() {
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(10240);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sListen, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
        printf("Socket:  \033[;31mBind Error!\033[0m");
        return;
    }
    if (listen(sListen, 5) == -1) {
        printf("Socket:  \033[;31mListen Error!\033[0m");
        return;
    }
    std::cout << "Socket:  \033[;32mSocket Server Initialization Finished.\033[0m" << std::endl;
    
    int sClient;    // 创建客户端socket
    sockaddr_in clientAdd;  // 客户端的地址信息
    socklen_t clientAddLen = sizeof(clientAdd);
    char receiveDat[10240]; // 接收数据缓冲区
    char command[10240], sendResult[10240];
    
    string cmd;
    std::cout << "BackEnd: \033[32mBackEnd Initialization Finished.\033[0m" << std::endl;
    bool flag = true;
    while (flag) {
        memset(command, 0, sizeof(command));
        std::cout << "Socket:  \033[;33m<< Begin Listening...\033[0m" << std::endl;
        sClient = accept(sListen, (struct sockaddr *) &clientAdd, &clientAddLen);
        if (sClient == -1) {
            printf("Socket:  \033[;31mAccept Error!\033[0m");
            continue;
        }
        recv(sClient, receiveDat, 10240, 0);
        std::cout << "Socket:  \033[;33m>> Receive Data: \033[35m\"" << receiveDat << "\"\033[0m" << std::endl;
        std::stringstream receiveDatSS(receiveDat);
        int commandLen = 0, tmp, digitLen;
        receiveDatSS >> commandLen;
        tmp = commandLen;
        for (digitLen = 0; tmp != 0; digitLen++) tmp /= 10;
        strncpy(command, receiveDat + digitLen, commandLen);
        cmd = std::string(command);
        std::cout << "BackEnd: \033[;36m<< Handling Operation: \033[35m\"" << cmd << "\"\033[0m" << std::endl;
        std::string resultStr = process(cmd);
        while (resultStr[resultStr.length() - 1] == '\n'
               || resultStr[resultStr.length() - 1] == '\r'
               || resultStr[resultStr.length() - 1] == ' ')
            resultStr.pop_back();
        std::cout << "Socket:  \033[;36m>> Operation Result: \033[35m\"" << resultStr << "\"\033[0m" << std::endl;
        std::stringstream resultSS("");
        resultSS << std::setw(6) << std::setfill('0') << resultStr.length();
        resultSS << resultStr;
        strcpy(sendResult, resultSS.str().c_str());
        send(sClient, sendResult, strlen(sendResult), 0);
        if (resultStr == "bye")flag = false;
    }
}
