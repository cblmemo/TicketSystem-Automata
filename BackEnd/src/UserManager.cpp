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
    if (isLogin(p["-c"]) && !indexPool.containsKey(p["-u"]) && loginPool[p["-c"]].first > p("-g")) {
        user_t newUser {p["-u"], p["-p"], p["-n"], p["-m"], p("-g")};
        int offset = storagePool.write(newUser);
        indexPool.insert(newUser.username, offset);
        return outputSuccess();
    }
    outputFailure();
}

void UserManager::login(const Parser &p) {
    if (!isLogin(p["-u"])) {
        std::pair<int, bool> temp {indexPool.find(p["-u"])};
        if (!temp.second)return outputFailure();
        user_t tempUser {storagePool.read(temp.first)};
        if (tempUser.password == p["-p"]) {
            loginPool[p["-u"]] = {tempUser.privilege, temp.first};
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
        if (p["-c"] == p["-u"])return printUser(storagePool.read(loginPool[p["-u"]].second));
        if (isLogin(p["-u"]) && loginPool[p["-c"]].first > loginPool[p["-u"]].first)return printUser(storagePool.read(loginPool[p["-u"]].second));
        std::pair<int, bool> temp {indexPool.find(p["-u"])};
        if (!temp.second)return outputFailure();
        user_t qUser {storagePool.read(temp.first)};
        if (loginPool[p["-c"]].first > qUser.privilege)return printUser(qUser);
    }
    outputFailure();
}

void UserManager::modifyProfile(const Parser &p) {
    if (isLogin(p["-c"]) && (!p.haveThisArgument("-g") || p("-g") < loginPool[p["-c"]].first)) {
        if (p["-c"] == p["-u"]) {
            user_t mUser {storagePool.read(loginPool[p["-u"]].second)};
            if (p.haveThisArgument("-p"))mUser.password = p["-p"];
            if (p.haveThisArgument("-n"))mUser.name = p["-n"];
            if (p.haveThisArgument("-m"))mUser.mailAddr = p["-m"];
            if (p.haveThisArgument("-g"))mUser.privilege = p("-g");
            storagePool.update(mUser, loginPool[p["-u"]].second);
            loginPool[p["-u"]].first = mUser.privilege;
            return printUser(mUser);
        }
        if (isLogin(p["-u"])) {
            user_t mUser {storagePool.read(loginPool[p["-u"]].second)};
            if (loginPool[p["-c"]].first > mUser.privilege) {
                if (p.haveThisArgument("-p"))mUser.password = p["-p"];
                if (p.haveThisArgument("-n"))mUser.name = p["-n"];
                if (p.haveThisArgument("-m"))mUser.mailAddr = p["-m"];
                if (p.haveThisArgument("-g"))mUser.privilege = p("-g");
                storagePool.update(mUser, loginPool[p["-u"]].second);
                loginPool[p["-u"]].first = mUser.privilege;
                return printUser(mUser);
            }
        }
        else {
            std::pair<int, bool> temp {indexPool.find(p["-u"])};
            if (!temp.second)return outputFailure();
            user_t mUser {storagePool.read(temp.first)};
            if (loginPool[p["-c"]].first > mUser.privilege) {
                if (p.haveThisArgument("-p"))mUser.password = p["-p"];
                if (p.haveThisArgument("-n"))mUser.name = p["-n"];
                if (p.haveThisArgument("-m"))mUser.mailAddr = p["-m"];
                if (p.haveThisArgument("-g"))mUser.privilege = p("-g");
                storagePool.update(mUser, temp.first);
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
