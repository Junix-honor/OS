//
// Created by Honor on 2020/12/11.
//

#include "RCB.h"

RCB::RCB(/* args */) : Initial(0), Available(0) {
}

RCB::RCB(string rid, int init, int avail) : RID(rid), Initial(init), Available(avail) {
}

RCB::~RCB() {
}

string RCB::getRID() {
    return RID;
}

int RCB::getAvailable() {
    return Available;
}

int RCB::getInitial() {
    return Initial;
}

//添加进程到资源的等待链表
int RCB::insertProcess(PCB *process) {
    //list<PCB *> WaitingList 添加进程
    WaitingList.push_back(process);
    return true;
}

//从资源的等待链表删除进程
int RCB::removeProcess(PCB *process) {
    //list<PCB *> WaitingList 删除进程
    WaitingList.remove(process);
    return true;
}