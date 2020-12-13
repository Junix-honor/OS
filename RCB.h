//
// Created by Honor on 2020/12/11.
//

#ifndef OS_RCB_H
#define OS_RCB_H


#include <cstdio>
#include <string>
#include <vector>
#include <list>
#include "PCB.h"

using namespace std;

class PCB;

class RCB {
private:
    friend class PRM;

    string RID;
    int Initial;
    int Available;
    list<PCB *> WaitingList;
public:
    RCB(/* args */);

    RCB(string rid, int init, int avail);

    ~RCB();

    string getRID();

    int getAvailable();

    int getInitial();

    int insertProcess(PCB *process);

    int removeProcess(PCB *process);
};


#endif //OS_RCB_H
