//
// Created by Honor on 2020/12/11.
//

#ifndef OS_PRM_H
#define OS_PRM_H


#include "PCB.h"
#include "RCB.h"
#include<list>
#include<map>
#include <sstream>
#include <iostream>

class PRM {
private:
    int ResourceNum;
    int PriorityNum;
    map<string, RCB *> Resources;
    map<string, PCB *> Processes;
    vector<list<PCB *> > RL;
    PCB *Running_Process;
public:
    PRM(int resource, int priority);

    ~PRM();

    int create(string id, int priority);

    int destroy(string id);

    int request(string rid, int n);

    int release(string rid, int n);

    int timeout();

    int scheduler();

    int killTree(PCB *p);

    //对terminal接口
    int listProcess();

    int listResource();

    int listReady();

    int listBlock();

    int printPCB(string pid);
};


#endif //OS_PRM_H
