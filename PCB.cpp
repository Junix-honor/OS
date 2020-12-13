//
// Created by Honor on 2020/12/11.
//

#include "PCB.h"
PCB::PCB(/* args */)
        :Parent(nullptr), Priority(-1), Type("ready"), req_num(0), BlockList(NULL)
{
}
PCB::PCB(string pid, PCB* parent, int priority)
        : PID(pid), Parent(parent), Priority(priority), Type("ready"), req_num(0), BlockList(NULL)
{
}
PCB::~PCB()
{
}
int PCB::insertChild(PCB* child)
{
    Children.push_back(child);
    return true;
}
string PCB::getPID()
{
    return PID;
}
int PCB::getPriority()
{
    return Priority;
}
void PCB::setReady()
{
    Type = "ready";
}
void PCB::setRunning()
{
    Type = "running";
}
void PCB::setBlocked()
{
    Type = "blocked";
}
void PCB::insertResource(RCB* rcb, int num)
{
    req_num = 0;
    BlockList = NULL;
    auto iter = Other_resources.find(rcb->getRID());
    if (iter == Other_resources.end())
        Other_resources.insert((pair<string, Resource>(rcb->getRID(), { rcb,num })));
    else
        iter->second.used += num;
}
int  PCB::releaseResource(RCB* rcb, int num)
{
    auto iter = Other_resources.find(rcb->getRID());
    if (iter == Other_resources.end())
        return -1;
    else
        iter->second.used -= num;
    return true;
}
void PCB::setBlock(RCB* res, int req)
{
    setBlocked();
    BlockList = res;
    req_num = req;
}
int PCB::getReqnum()
{
    return req_num;
}