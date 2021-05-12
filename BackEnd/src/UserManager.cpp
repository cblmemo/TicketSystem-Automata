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

bool UserManager::isLogin(const UserManager::username_t &u) {
    //if a user is login, the it must haveThisArgument.
    return loginPool.containsKey(u);
}

void UserManager::addUser(const Parser &p) {
    if (storagePool.readExtraMessage()) {
        storagePool.updateExtraMessage(false);
        user_t newUser {p["-u"], p["-p"], p["-n"], p["-m"], 10};
        int offset = storagePool.write(newUser);
        indexPool.insert(newUser.username, offset);
        return outputSuccess();
    }
    if (isLogin(p["-c"]) && !indexPool.containsKey(p["-u"]) && loginPool[p["-c"]] > p("-g")) {
        user_t newUser {p["-u"], p["-p"], p["-n"], p["-m"], p("-g")};
        int offset = storagePool.write(newUser);
        indexPool.insert(newUser.username, offset);
        return outputSuccess();
    }
    outputFailure();
}

void UserManager::login(const Parser &p) {
    if (!isLogin(p["-u"])) {
        vector<int> temp;
        indexPool.find(p["-u"], temp);
        if (temp.empty())return outputFailure();
        user_t tempUser {storagePool.read(temp[0])};
        if (tempUser.password == p["-p"]) {
            loginPool[p["-u"]] = tempUser.privilege;
            return outputSuccess();
        }
    }
    outputFailure();
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
            user_t qUser {storagePool.read(temp[0])};
            return printUser(qUser);
        }
        vector<int> temp;
        indexPool.find(p["-u"], temp);
        if (temp.size() == 1) {
            user_t qUser {storagePool.read(temp[0])};
            if (loginPool[p["-c"]] > qUser.privilege)return printUser(qUser);
        }
    }
    outputFailure();
}

void UserManager::modifyProfile(const Parser &p) {
    if (isLogin(p["-c"]) && p("-g") < loginPool[p["-c"]]) {
        if (p["-c"] == p["-u"]) {
            vector<int> temp;
            indexPool.find(p["-u"], temp);
            user_t mUser {storagePool.read(temp[0])};
            if (p.haveThisArgument("-p"))mUser.password = p["-p"];
            if (p.haveThisArgument("-n"))mUser.name = p["-n"];
            if (p.haveThisArgument("-m"))mUser.mailAddr = p["-m"];
            if (p.haveThisArgument("-g"))mUser.privilege = p("-g");
            storagePool.update(mUser, temp[0]);
            return printUser(mUser);
        }
        vector<int> temp;
        indexPool.find(p["-u"], temp);
        if (temp.size() == 1) {
            user_t mUser {storagePool.read(temp[0])};
            if (loginPool[p["-c"]] > mUser.privilege) {
                if (p.haveThisArgument("-p"))mUser.password = p["-p"];
                if (p.haveThisArgument("-n"))mUser.name = p["-n"];
                if (p.haveThisArgument("-m"))mUser.mailAddr = p["-m"];
                if (p.haveThisArgument("-g"))mUser.privilege = p("-g");
                storagePool.update(mUser, temp[0]);
                return printUser(mUser);
            }
        }
    }
    outputFailure();
}

void UserManager::clear() {
    loginPool.clear();
    indexPool.clear();
    storagePool.clear(true);
}
