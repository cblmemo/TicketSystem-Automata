//
// Created by Rainy Memory on 2021/4/14.
//

#include "UserManager.h"

inline void UserManager::outputSuccess() {
    defaultOut << "0" << endl;
}

inline void UserManager::outputFailure() {
    defaultOut << "-1" << endl;
}

inline void UserManager::printUser(const UserManager::user_t &u) {
    defaultOut << u.username << " " << u.name << " " << u.mailAddr << " " << u.privilege << endl;
}

int UserManager::getPrivilege(const UserManager::username_t &u) {
    vector<int> temp;
    indexPool.find(u, temp);
#ifdef debug
    if (temp.empty())throw inexistUser();
#endif
    user_t tempUser = storagePool.read(temp[0]);
    return tempUser.privilege;
}

bool UserManager::checkPassword(const UserManager::username_t &u, const UserManager::password_t &p) {
    //ensure user's exist
    vector<int> temp;
    indexPool.find(u, temp);
    if (temp.empty())return false;
    user_t tempUser = storagePool.read(temp[0]);
    return tempUser.password == p;
}

bool UserManager::isLogin(const UserManager::username_t &u) {
    //if a user is login, the it must haveThisArgument.
    return loginPool.containsKey(u);
}

void UserManager::addUser(const Parser &p) {
    if (storagePool.readExtraMessage()) {
        storagePool.updateExtraMessage(false);
        user_t newUser(p["-u"], p["-p"], p["-n"], p["-m"], 10);
        int index = storagePool.write(newUser);
        indexPool.insert(newUser.username, index);
        outputSuccess();
        return;
    }
    if (isLogin(p["-c"]) && !indexPool.containsKey(p["-u"]) && getPrivilege(p["-c"]) > p("-g")) {
        user_t newUser(p["-u"], p["-p"], p["-n"], p["-m"], p("-g"));
        int index = storagePool.write(newUser);
        indexPool.insert(newUser.username, index);
        outputSuccess();
    }
    else outputFailure();
}

void UserManager::login(const Parser &p) {
    if (!isLogin(p["-u"]) && checkPassword(p["-u"], p["-p"]))loginPool[p["-u"]] = true, outputSuccess();
    else outputFailure();
}

void UserManager::logout(const Parser &p) {
    if (isLogin(p["-u"]))loginPool.erase(p["-u"]), outputSuccess();
    else outputFailure();
}

void UserManager::queryProfile(const Parser &p) {
    if (isLogin(p["-c"])) {
        if (p["-c"] == p["-u"]) {
            vector<int> temp;
            indexPool.find(p["-u"], temp);
            user_t queryUser(storagePool.read(temp[0]));
            printUser(queryUser);
        }
        else {
            vector<int> temp1, temp2;
            indexPool.find(p["-c"], temp1), indexPool.find(p["-u"], temp2);
            if (temp1.size() == 1 && temp2.size() == 1) {
                user_t nowUser(storagePool.read(temp1[0])), queryUser(storagePool.read(temp2[0]));
                if (nowUser.privilege > queryUser.privilege)printUser(queryUser);
                else outputFailure();
            }
            else outputFailure();
        }
    }
    else outputFailure();
}

void UserManager::modifyProfile(const Parser &p) {
    if (isLogin(p["-c"])) {
        if (p["-c"] == p["-u"]) {
            vector<int> temp;
            indexPool.find(p["-u"], temp);
            user_t modifyUser(storagePool.read(temp[0]));
            if (p.haveThisArgument("-p"))modifyUser.password = p["-p"];
            if (p.haveThisArgument("-n"))modifyUser.name = p["-n"];
            if (p.haveThisArgument("-m"))modifyUser.mailAddr = p["-m"];
            if (p.haveThisArgument("-g") && p("-g") < modifyUser.privilege)modifyUser.privilege = p("-g");
            storagePool.update(modifyUser, temp[0]);
            printUser(modifyUser);
        }
        else {
            vector<int> temp1, temp2;
            indexPool.find(p["-c"], temp1), indexPool.find(p["-u"], temp2);
            if (temp1.size() == 1 && temp2.size() == 1) {
                user_t nowUser(storagePool.read(temp1[0])), modifyUser(storagePool.read(temp2[0]));
                if (nowUser.privilege > modifyUser.privilege) {
                    if (p.haveThisArgument("-p"))modifyUser.password = p["-p"];
                    if (p.haveThisArgument("-n"))modifyUser.name = p["-n"];
                    if (p.haveThisArgument("-m"))modifyUser.mailAddr = p["-m"];
                    if (p.haveThisArgument("-g") && p("-g") < nowUser.privilege)modifyUser.privilege = p("-g");
                    storagePool.update(modifyUser, temp2[0]);
                    printUser(modifyUser);
                }
                else outputFailure();
            }
            else outputFailure();
        }
    }
    else outputFailure();
}

void UserManager::clear() {
    loginPool.clear();
    indexPool.clear();
    storagePool.clear(true);
}
