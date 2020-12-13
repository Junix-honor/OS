//
// Created by Honor on 2020/12/11.
//

#include "PRM.h"

using namespace std;

PRM::PRM(int resource, int priority) : ResourceNum(resource), PriorityNum(priority) {
    //资源初始化
    for (int i = 1; i <= ResourceNum; i++) {
        ostringstream rid;
        rid << "R" << i;
        RCB *R = new RCB(rid.str(), i, i);
        Resources.insert(pair<string, RCB *>(rid.str(), R));
    }
    RL.resize(PriorityNum);
    //初始进程
    Running_Process = new PCB("init", nullptr, 0);
    Running_Process->setRunning();
    Processes.insert(pair<string, PCB *>("init", Running_Process));
    RL[0].push_back(Running_Process);
    cout << "Init process is running" << endl;
}

PRM::~PRM() = default;

int PRM::create(string id, int priority) {
    PCB *Process = new PCB(id, Running_Process, priority);
    RL[priority].push_back(Process);
    Running_Process->insertChild(Process);
    Running_Process->setReady();
    Processes.insert(pair<string, PCB *>(id, Process));
    scheduler();
    return true;
}

int PRM::destroy(string id) {
    auto iter = Processes.find(id);
    if (iter == Processes.end())
        return -1; //没有这个进程
    PCB *process = iter->second;
    PCB *parent = process->Parent;
    if (parent != nullptr)
        parent->Children.remove(process);
    killTree(process);
    scheduler();
    return true;
}

int PRM::killTree(PCB *p) {
    //枚举所有子孙进程
    for (auto iter = p->Children.begin(); iter != p->Children.end(); ++iter)
        killTree(*iter);

    //释放资源
    for (auto iter = p->Other_resources.begin(); iter != p->Other_resources.end(); ++iter) {
        Resource res = iter->second;
        RCB *R = res.rcb;
        R->Available += res.used;
        cout << "Release " << R->RID << ".";
        while (!R->WaitingList.empty() && R->WaitingList.front() != p &&
               R->Available >= R->WaitingList.front()->getReqnum()) {
            PCB *process = R->WaitingList.front();

            R->Available -= process->getReqnum();
            R->removeProcess(process);
            process->setReady();
            process->insertResource(R, process->getReqnum());
            RL[process->Priority].push_back(process);
            cout << "Wake up process " << process->PID << ".";
        }
    }
    //删除PCB和指向这个PCB是所有指针
    Processes.erase(p->PID); //进程map中删除
    RL[p->Priority].remove(p); //Ready List中删除
    delete p;
    return true;
}

int PRM::scheduler() {
    int highestPriority = -1;
    for (int i = PriorityNum - 1; i >= 0; i--) {
        if (!RL[i].empty()) {
            highestPriority = i;
            break;
        }
    }
    if (highestPriority == -1)
        return -1;
    if (RL[highestPriority].front()->Type != "running"
        && (RL[highestPriority].front() != nullptr)) {
        Running_Process = RL[highestPriority].front();
        Running_Process->setRunning();
    }
    cout << "Process " << Running_Process->PID << " is running." << endl;
    return true;
}

int PRM::timeout() {
    Running_Process->setReady();
    RL[Running_Process->Priority].push_back(Running_Process);
    RL[Running_Process->Priority].pop_front();
    cout << "Process " << Running_Process->PID << " is ready.";
    scheduler();
    return true;
}

int PRM::request(string rid, int n) {
    auto iter = Resources.find(rid);
    if (iter == Resources.end())
        return false;
    RCB *R = iter->second;
    if (n > R->Initial)
        return false;
    if (R->Available >= n) {
        Running_Process->insertResource(R, n);
        R->Available -= n;
        cout << "Process " << Running_Process->PID << " request " << n << " " << rid << endl;
    } else {
        RL[Running_Process->Priority].remove(Running_Process);
        Running_Process->setBlock(R, n);
        R->insertProcess(Running_Process);
        cout << "Process " << Running_Process->PID << " is blocked.";
        scheduler();
    }
    return true;
}

int PRM::release(string rid, int n) {
    auto iter = Resources.find(rid);
    if (iter == Resources.end())
        return -1; //没有这个资源

    RCB *R = iter->second;
    Running_Process->releaseResource(R, n);
    R->Available += n;

    while (!R->WaitingList.empty() && R->Available >= R->WaitingList.front()->getReqnum()) {
        PCB *process = R->WaitingList.front();
        R->Available -= process->getReqnum();
        R->removeProcess(process);
        process->setReady();
        process->insertResource(R, process->getReqnum());
        RL[process->Priority].push_back(process);
    }
    scheduler();
    return true;
}

int PRM::listProcess() {
    cout << "======Process List======" << endl;
    cout << "PID" << " " << "Status" << " " << "Priority" << endl;
    for (auto iter = Processes.begin(); iter != Processes.end(); ++iter) {
        PCB *process = iter->second;
        cout << process->PID << " " << process->Type << " " << process->Priority << endl;
    }
    cout << endl;

    return true;
}

int PRM::listResource() {
    cout << endl << "======Resource List======" << endl;
    cout << "RID" << " " << "Initial" << " " << "Available" << endl;
    for (auto iter = Resources.begin(); iter != Resources.end(); ++iter) {
        RCB *resource = iter->second;
        cout << resource->RID << "    " << resource->Initial << "       " << resource->Available << endl;
    }
    cout << endl;

    return true;
}


int PRM::listReady() {
    cout << endl << "======" << "Ready List" << "======" << endl;
    for (int i = PriorityNum - 1; i >= 0; i--) {
        cout << i << ":";
        for (auto iter = RL[i].begin(); iter != RL[i].end(); ++iter)
            cout << (*iter)->PID << " ";
        cout << endl;
    }
    cout << endl;
    return true;
}

int PRM::listBlock() {
    cout << endl << "======" << "Block List" << "======" << endl;
    for (auto iter = Resources.begin(); iter != Resources.end(); ++iter) {
        cout << iter->first << " ";
        RCB *R = iter->second;
        for (auto it = R->WaitingList.begin(); it != R->WaitingList.end(); ++it)
            cout << (*it)->PID << " ";
        cout << endl;
    }
    cout << endl;
    return true;
}

int PRM::printPCB(string pid) {
    auto iter = Processes.find(pid);
    if (iter == Processes.end())
        return -1; //没有这个进程
    PCB *process = iter->second;
    cout << endl << "======" << "Process Information" << "======" << endl;
    cout << "PID : " << process->PID << endl;
    cout << "Status : " << process->Type << endl;

    if (process->Type == "blocked")
        cout << "BlcokList : " << process->BlockList->RID << " " << "wait " << process->req_num << endl;
    cout << "Priority : " << process->Priority << endl;

    if (process->Parent == nullptr)
        cout << "Parent : None" << endl;
    else
        cout << "Parent : " << process->Parent->PID << endl;

    cout << "Children : ";
    for (auto iter = process->Children.begin(); iter != process->Children.end(); iter++)
        cout << (*iter)->PID << " ";
    cout << endl;

    cout << "Resource : ";
    for (auto iter = process->Other_resources.begin(); iter != process->Other_resources.end(); iter++)
        cout << iter->first << "-" << iter->second.used << " ";
    cout << endl;

    cout << endl;
    return true;
}
