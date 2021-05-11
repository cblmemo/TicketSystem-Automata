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
    Parser       *Ptilopsis;
    UserManager  *Saria;
    TrainManager *Silence;
    OrderManager *Ifrit;
    std::istream &defaultIn;
    std::ostream &defaultOut;
    
    using path_t = const std::string;
    
    path_t UserIndexPath    {"UserIndex.dat"};
    path_t UserStoragePath  {"UserStorage.dat"};
    path_t TrainIndexPath   {"TrainIndex.dat"};
    path_t TrainStoragePath {"TrainStorage.dat"};
    path_t TrainStationPath {"TrainStation.dat"};
    path_t OrderIndexPath   {"OrderIndex.dat"};
    path_t OrderStoragePath {"OrderStorage.dat"};
    path_t OrderPendingPath {"OrderPending.dat"};
    
    void initialize();
    
    void clean();

public:
    Administrator(std::istream &is, std::ostream &os);
    
    ~Administrator();
    
    void runProgramme();
};

#endif //TICKETSYSTEM_AUTOMATA_ADMINISTRATOR_H
