#pragma region HEADER

// Multithreading
#include <thread>   // std::thread
#include <future>   // std::promise

// Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// std
#include <sstream>
#include <iomanip>      // setw setfill
#include <functional>   // std::ref
#include <chrono>       // std::chrono::seconds(0)

// BackEnd
#include "Administrator.h"

#pragma endregion HEADER

std::stringstream inputSS, outputSS;
bool operatingFlag = false;

void socketService(std::promise<int> &promiseRet) {
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(10240);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sListen, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
        printf("Socket:  \033[;31mBind Error!\033[0m");
        promiseRet.set_value(1);
        return;
    }
    if (listen(sListen, 5) == -1) {
        printf("Socket:  \033[;31mListen Error!\033[0m");
        promiseRet.set_value(1);
        return;
    }
    std::cout << "Socket:  \033[;32mSocket Server Initialization Finished.\033[0m" << std::endl;

    int sClient;    // 创建客户端socket
    sockaddr_in clientAdd;  // 客户端的地址信息
    socklen_t clientAddLen = sizeof(clientAdd);
    char receiveDat[10240]; // 接收数据缓冲区

    char command[10240], result[10240], sendResult[10240];
    while (1) {
        memset(command, 0, sizeof(command));
        memset(result, 0, sizeof(result));
        std::cout << "Socket:  \033[;33m<< Begin Listening...\033[0m" << std::endl;
        sClient = accept(sListen, (struct sockaddr *) &clientAdd, &clientAddLen);
        if (sClient == -1) {
            printf("Socket:  \033[;31mAccept Error!\033[0m");
            continue;
        }
//        if (strcmp(inet_ntoa(clientAdd.sin_addr), "49.234.112.112")) {
//            printf("server attack from outside!\n");
//            continue;
//        }
        recv(sClient, receiveDat, 10240, 0);
        std::cout << "Socket:  \033[;33m>> Receive Data: \033[35m\"" << receiveDat << "\"\033[0m" << std::endl;
        std::stringstream receiveDatSS(receiveDat);
        int commandLen = 0, tmp, digitLen;
        receiveDatSS >> commandLen;
        tmp = commandLen;
        for (digitLen = 0; tmp != 0; digitLen++) tmp /= 10;
        strncpy(command, receiveDat + digitLen, commandLen);

        inputSS << command;
        operatingFlag = true; // 等待 backEnd 完成操作
        while (operatingFlag) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); }

        outputSS >> result;
        inputSS.clear(), outputSS.clear();
        inputSS.str(""), outputSS.str(""); // 释放 stringstream 内存占用

        std::string resultStr(result);
        std::cout << "Socket:  \033[;36m>> Operation Result: \033[35m\"" << resultStr << "\"\033[0m" << std::endl;
        std::stringstream resultSS("");
        resultSS << std::setw(6) << std::setfill('0') << strlen(result);
        resultSS << result;
        strcpy(sendResult, resultSS.str().c_str());
        resultSS.clear(), resultSS.str("");

        send(sClient, sendResult, strlen(sendResult), 0);

        if (resultStr == "bye") break;
    }
    /*
    std::cout << "cp21" << std::endl;
    while(1) {
        string tIn, tOut;
        std::cin >> tIn;
        std::cout << "<<<<<" << tIn << std::endl;
        inputSS << tIn;
        // 等待 backEnd 完成操作
        std::cout << "cp212" << std::endl;
        operatingFlag = true;
        while (operatingFlag) {}
        std::cout << "cp213" << std::endl;
        outputSS >> tOut;
        inputSS.str(""), outputSS.str(""); // 释放 stringstream 内存占用
        std::cout << tOut << std::endl;
        if(tOut == "bye") break;
    }
     */
    std::cout << "Socket:  \033[;32mBackEnd Exited.\033[0m" << std::endl;
    promiseRet.set_value(0);
    return;
}

//void backEnd(std::promise<int> &promiseRet) {
//    Administrator RhineLab{inputSS, outputSS};
//    int ret = RhineLab.runProgramme(operatingFlag);
//    // 此处将函数 runProgramme 返回值由 void 改为 int
//    std::cout << "BackEnd: \033[;32mBackEnd Exited.\033[0m" << std::endl;
//    promiseRet.set_value(ret);
//    return;
//}

int main() {
//  本程序中 future 用于传递线程顶层函数返回值(即 socketService 和 backEnd 返回值)
//  future.get() 会阻塞程序至获得返回值, 功能与 thread.join() 相同
//    std::promise<int> socketPromise, backEndPromise;
//    std::future<int> socketFuture = socketPromise.get_future();
//    std::future<int> backEndFuture = backEndPromise.get_future();
//
//    std::thread socketServiceThread(socketService, std::ref(socketPromise));
//    std::thread backEndThread(backEnd, std::ref(backEndPromise));
//    std::cout << "\033[44;37mThreads Initialization Finish.\033[0m" << std::endl;
//
//    while (socketFuture.wait_for(std::chrono::seconds(3)) != std::future_status::ready
//           && backEndFuture.wait_for(std::chrono::seconds(3)) != std::future_status::ready) {}
//    if (socketFuture.wait_for(std::chrono::seconds(3)) == std::future_status::ready)
//        std::cout << "\033[44;37mSocketService Stop.\033[0m" << std::endl;
//    if (backEndFuture.wait_for(std::chrono::seconds(3)) == std::future_status::ready)
//        std::cout << "\033[44;37mBackEnd Stop.\033[0m" << std::endl;
//
//    int socketF = -19260817, backEndF = -19260817;
//    if (socketFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
//        socketF = socketFuture.get();
//    if (backEndFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
//        backEndF = backEndFuture.get();
//    std::cout << "Preparing to Exit. (Return Code -19260817 for No Responding)\n"
//              << "SocketService Return Code: \033[33m" << socketF << "\033[0m\n"
//              << "BackEnd Return Code:       \033[33m" << backEndF << "\033[0m\n"
//              << std::endl;
//    return ((socketF == 0 && backEndF == 0) ? 0 : 1);
    Administrator RhineLab;
    RhineLab.runProgramme();
}