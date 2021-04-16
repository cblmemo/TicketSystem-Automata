//
// Created by Rainy Memory on 2021/4/13.
//

#ifndef TICKETSYSTEM_AUTOMATA_ADMINISTRATOR_H
#define TICKETSYSTEM_AUTOMATA_ADMINISTRATOR_H

#include "UserManager.h"
#include "TrainManager.h"
#include "OrderManager.h"

class Administrator {
private:
    Parser *Ptilopsis = nullptr;
    UserManager *Saria = nullptr;
    TrainManager *Silence = nullptr;
    OrderManager *Ifrit = nullptr;
    
    using path_t = const std::string;
    
    path_t UserIndexPath = "UserIndex.dat";
    path_t UserStoragePath = "userStorage.dat";

public:
    Administrator();
    
    ~Administrator();
    
    void runProgramme(std::istream &is = std::cin);
};

#endif //TICKETSYSTEM_AUTOMATA_ADMINISTRATOR_H
