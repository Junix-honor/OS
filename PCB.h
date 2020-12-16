//
// Created by Honor on 2020/12/11.
//

#ifndef OS_PCB_H
#define OS_PCB_H


#include <cstdio>
#include <string>
#include <vector>
#include <string>
#include<map>
#include "RCB.h"
#include<list>

using namespace std;

class RCB;

struct Resource {
    RCB *rcb;
    int used;
};

class PCB {
private:
    friend class PRM;

    string PID;
    map<string, Resource> OtherResources;
    //Status: Type
    string Type;
    //block
    RCB *BlockList;
    int BlcokWait;

    int Priority;
    //Creation_tree
    PCB *Parent;
    list<PCB *> Children;

public:
    PCB();

    PCB(string pid, PCB *parent, int priority);

    ~PCB();

    int insertChild(PCB *child);

    void setReady();

    void setBlocked();

    void setRunning();

    int insertResource(RCB *rcb, int num);

    int releaseResource(RCB *rcb, int num);

    void blockProcess(RCB *res, int req);

    string getPID();

    int getPriority();

    int getBlockWait();
};

#endif //OS_PCB_H
