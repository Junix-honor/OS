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

int RCB::insertProcess(PCB *process) {
    WaitingList.push_back(process);
    return true;
}

int RCB::removeProcess(PCB *process) {
    WaitingList.remove(process);
    return true;
}