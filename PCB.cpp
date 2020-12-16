//
// Created by Honor on 2020/12/11.
//

#include "PCB.h"

PCB::PCB(/* args */)
        : Parent(nullptr), Priority(-1), Type("ready"), BlcokWait(0), BlockList(NULL) {
}

PCB::PCB(string pid, PCB *parent, int priority)
        : PID(pid), Parent(parent), Priority(priority), Type("ready"), BlcokWait(0), BlockList(NULL) {
}

PCB::~PCB() {
}

//添加子进程
int PCB::insertChild(PCB *child) {
    //list<PCB *> Children 添加子进程
    Children.push_back(child);
    return true;
}

string PCB::getPID() {
    return PID;
}

int PCB::getPriority() {
    return Priority;
}

//设置为ready状态
void PCB::setReady() {
    Type = "ready";
}

//设置为running状态
void PCB::setRunning() {
    Type = "running";
}

//设置为blocked状态
void PCB::setBlocked() {
    Type = "blocked";
}

//添加资源
int PCB::insertResource(RCB *rcb, int num) {
    //初始化引发进程阻塞的资源及其等待的资源数
    BlcokWait = 0;
    BlockList = nullptr;
    //在占用资源的map中查找是否已经占用该资源
    auto iter = OtherResources.find(rcb->getRID());
    if (iter == OtherResources.end())
        //如果没有占用，则增加占有资源
        OtherResources.insert((pair<string, Resource>(rcb->getRID(), {rcb, num})));
    else
        //如果已经占用，则更新占有数目
        iter->second.used += num;
    return true;
}

//释放资源
int PCB::releaseResource(RCB *rcb, int num) {
    //在占用资源的map中查找是否存在这个资源
    auto iter = OtherResources.find(rcb->getRID());
    if (iter == OtherResources.end())
        //如果不存在，则返回错误
        return -1;
    else if (iter->second.used < num)
        //如果请求的释放资源数大于占用的资源数，则返回错误
        return -2;
    else
        //更新占用资源数
        iter->second.used -= num;
    return true;
}

//阻塞进程
void PCB::blockProcess(RCB *res, int req) {
    //设置为阻塞态
    setBlocked();
    //设置引发进程阻塞的资源
    BlockList = res;
    //设置等待的资源数
    BlcokWait = req;
}

int PCB::getBlockWait() {
    return BlcokWait;
}