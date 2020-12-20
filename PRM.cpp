//
// Created by Honor on 2020/12/11.
//

#include "PRM.h"

using namespace std;

//初始化
PRM::PRM(int resource, int priority) : ResourceNum(resource), PriorityNum(priority) {
    //资源初始化
    for (int i = 1; i <= ResourceNum; i++) {
        ostringstream rid;
        rid << "R" << i;
        //创建资源Ri
        RCB *R = new RCB(rid.str(), i, i);
        //资源Ri初始化为i个
        Resources.insert(pair<string, RCB *>(rid.str(), R));
    }
    //初始化就绪队列
    RL.resize(PriorityNum);
    //初始进程init，作为当前运行进程
    RunningProcess = new PCB("init", nullptr, 0);
    //将初始化进程设置为running状态
    RunningProcess->setRunning();
    //将init进程添加到进程管理器的进程集中
    Processes.insert(pair<string, PCB *>("init", RunningProcess));
    //插入到相应优先级就绪队列的尾部
    RL[0].push_back(RunningProcess);
    cout << "Init process is running" << endl;
}

PRM::~PRM() = default;

//创建进程
int PRM::create(string id, int priority) {
    //判断是否存在PID为id的进程
    auto iter = Processes.find(id);
    if (iter != Processes.end()) {
        //进程已经存在
        cout << "Process " << id << " already exists." << endl;
        //返回错误
        return -1;
    }
    //创建新进程：进程ID、父进程、优先级
    PCB *Process = new PCB(id, RunningProcess, priority);
    //插入到相应优先级就绪队列的尾部
    RL[priority].push_back(Process);
    //为当前运行进程添加子进程，即新进程
    RunningProcess->insertChild(Process);
    //将新进程添加到进程管理器的进程集中
    Processes.insert(pair<string, PCB *>(id, Process));

    //如果新进程的优先级比当前进程优先级高
    if (RunningProcess->Priority < priority) {
        //将当前进程设置为就绪状态
        RunningProcess->setReady();
        //将当前进程从相应优先级就绪队列的对头移到队尾
        RL[RunningProcess->Priority].push_back(RunningProcess);
        RL[RunningProcess->Priority].pop_front();
    }
    //进程调度
    scheduler();
    //返回成功
    return true;
}

//撤销进程
int PRM::destroy(string id) {
    //判断是否存在PID为id的进程
    auto iter = Processes.find(id);
    if (iter == Processes.end()) {
        //进程不存在
        cout << "Process " << id << " does not exist." << endl;
        //返回错误
        return -1;
    }
    //撤销的进程
    PCB *process = iter->second;
    //撤销进程的父进程
    PCB *parent = process->Parent;
    //如果父进程非空
    if (parent != nullptr)
        //把撤销的进程从父进程中的子进程列表中删除
        parent->Children.remove(process);
    //递归撤销进程
    killTree(process);
    //进程调度
    scheduler();
    //返回成功
    return true;
}

//递归撤销进程
int PRM::killTree(PCB *p) {
    //枚举所有子进程
    for (auto iter = p->Children.begin(); iter != p->Children.end(); ++iter)
        //递归撤销子进程
        killTree(*iter);

    //枚举待撤销进程p占用的所有资源
    for (auto iter = p->OtherResources.begin(); iter != p->OtherResources.end(); ++iter) {
        //释放占用的资源R
        Resource res = iter->second;
        RCB *R = res.rcb;
        R->Available += res.used;
        cout << "Release " << R->RID << ".";
        //唤醒因等待资源R阻塞的进程
        while (!R->WaitingList.empty() && R->WaitingList.front() != p &&
               R->Available >= R->WaitingList.front()->getBlockWait()) {
            //唤醒的进程process
            PCB *process = R->WaitingList.front();
            //从资源R的等待链表中删除
            R->removeProcess(process);
            //唤醒的进程process占用资源R
            R->Available -= process->getBlockWait();
            process->insertResource(R, process->getBlockWait());
            //唤醒的进程process设置为就绪态
            process->setReady();
            //唤醒的进程process插入到相应优先级就绪队列的尾部
            RL[process->Priority].push_back(process);
            cout << "Wake up process " << process->PID << ".";
        }
    }
    //删除PCB和指向这个PCB是所有指针
    Processes.erase(p->PID); //管理器进程集中删除
    RL[p->Priority].remove(p); //相应优先级就绪队列中删除
    if (p->Type == "blocked")
        p->BlockList->WaitingList.remove(p);//资源等待队列中删除
    delete p;//删除进程

    //返回成功
    return true;
}

//调度
int PRM::scheduler() {
    //找到最高优先级
    int highestPriority = -1;
    for (int i = PriorityNum - 1; i >= 0; i--) {
        if (!RL[i].empty()) {
            highestPriority = i;
            break;
        }
    }

    //系统出错退出
    if (highestPriority == -1)
        exit(0);

    //如果最高优先级的进程没有在运行而且非空
    if (RL[highestPriority].front()->Type != "running"
        && (RL[highestPriority].front() != nullptr)) {
        //调度最高优先级的进程
        RunningProcess = RL[highestPriority].front();
        RunningProcess->setRunning();
    }
    cout << "Process " << RunningProcess->PID << " is running." << endl;
    return true;
}

//时间中断
int PRM::timeout() {
    //讲当前进程置为就绪态
    RunningProcess->setReady();

    //不考虑降级：将当前进程从相应优先级就绪队列的对头移到队尾
    RL[RunningProcess->Priority].push_back(RunningProcess);
    RL[RunningProcess->Priority].pop_front();
    cout << "Process " << RunningProcess->PID << " is ready.";

//    //考虑降级
//    int priority = RunningProcess->Priority;
//    int degrade = (priority > 1) ? priority - 1 : priority;
//    //降级
//    RunningProcess->Priority = degrade;
//    RL[degrade].push_back(RunningProcess);
//    RL[priority].pop_front();
//    cout << "Process " << RunningProcess->PID << " is ready.";

    //调度进程
    scheduler();
    return true;
}

//请求资源
int PRM::request(string rid, int n) {
    //判断资源是否存在
    auto iter = Resources.find(rid);
    if (iter == Resources.end()) {
        //资源不存在
        cout << "Resource " << rid << " does not exit." << endl;
        //返回错误
        return -1;
    }

    //请求的资源
    RCB *R = iter->second;
    if (n > R->Initial) {
        //请求的资源数大于最大资源数
        cout << "The maximum number of resources " << rid << " is " << R->Initial << "." << endl;
        //返回错误
        return -2;
    }
    //如果可用的资源数足够分配
    if (R->Available >= n) {
        //当前进程占用资源n个资源R
        RunningProcess->insertResource(R, n);
        R->Available -= n;
        cout << "Process " << RunningProcess->PID << " request " << n << " " << rid << endl;
    }
        //如果可用的资源数不足够分配
    else {
        //阻塞当前进程
        RunningProcess->blockProcess(R, n);
        //当前进程从相应优先级就绪队列中删除
        RL[RunningProcess->Priority].remove(RunningProcess);
        //将当前进程添加到资源R的等待链表中
        R->insertProcess(RunningProcess);
        cout << "Process " << RunningProcess->PID << " is blocked.";
        //调度进程
        scheduler();
    }
    return true;
}

//释放资源
int PRM::release(string rid, int n) {
    //判断资源是否存在
    auto iter = Resources.find(rid);
    if (iter == Resources.end()) {
        //资源不存在
        cout << "Resource " << rid << " does not exit." << endl;
        //返回错误
        return -1;
    }
    //判断资源是否被当前进程占用
    auto it = RunningProcess->OtherResources.find(rid);
    if (it == RunningProcess->OtherResources.end()) {
        //当前进程不存在该资源
        cout << "Resource " << rid << " does not exist in the current process." << endl;
        //返回错误
        return -2;
    }

    //释放的资源
    RCB *R = iter->second;
    if (n > it->second.used) {
        //释放的资源数大于占用的资源数
        cout << "The number of resources " << rid << " occupied by the current process is "
             << it->second.used << "." << endl;
        //返回错误
        return -3;
    }

    //当前运行的进程释放n个资源R
    RunningProcess->releaseResource(R, n);
    R->Available += n;

    //唤醒因等待资源R阻塞的进程
    while (!R->WaitingList.empty() && R->Available >= R->WaitingList.front()->getBlockWait()) {
        //唤醒的进程process
        PCB *process = R->WaitingList.front();
        //从资源R的等待链表中删除
        R->removeProcess(process);
        //唤醒的进程process占用资源R
        R->Available -= process->getBlockWait();
        process->insertResource(R, process->getBlockWait());
        //唤醒的进程process设置为就绪态
        process->setReady();
        //唤醒的进程process插入到相应优先级就绪队列的尾部
        RL[process->Priority].push_back(process);
    }
    //进程调度
    scheduler();
    //返回成功
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
        cout << "BlcokList : " << process->BlockList->RID << " " << "wait " << process->BlcokWait << endl;
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
    for (auto iter = process->OtherResources.begin(); iter != process->OtherResources.end(); iter++)
        cout << iter->first << "-" << iter->second.used << " ";
    cout << endl;

    cout << endl;
    return true;
}
