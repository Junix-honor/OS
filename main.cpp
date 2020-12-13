#include<cstdio>
#include<iostream>
#include<string>
#include"PRM.h"

void SplitString(const string &s, vector<string> &v, const string &c) {
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

int main() {
    int ResourceNum;
    int PriorityNum;
    while (true) {
        string shell;
        getline(cin, shell);
        vector<string> shellList;
        SplitString(shell, shellList, " ");
        string cmd = shellList[0];
        if (cmd == "init") {
            if (shellList.size() != 3) {
                cout << "parameter error" << endl;
                continue;
            }
            stringstream stream;
            int num;
            stream << shellList[1] << " " << shellList[2];
            stream >> ResourceNum >> PriorityNum;
            break;
        }
    }
    PRM manager(ResourceNum, PriorityNum);
    while (true) {
        string shell;
        getline(cin, shell);
        vector<string> shellList;
        SplitString(shell, shellList, " ");
        string cmd = shellList[0];
        if (cmd == "cr") {
            if (shellList.size() != 3) {
                cout << "parameter error" << endl;
                continue;
            }
            stringstream stream;
            int priority;
            stream << shellList[2];
            stream >> priority;
            manager.create(shellList[1], priority);
        } else if (cmd == "de") {
            if (shellList.size() != 2) {
                cout << "parameter error" << endl;
                continue;
            }
            manager.destroy(shellList[1]);
        } else if (cmd == "req") {
            if (shellList.size() != 3) {
                cout << "parameter error" << endl;
                continue;
            }
            stringstream stream;
            int num;
            stream << shellList[2];
            stream >> num;
            manager.request(shellList[1], num);
        } else if (cmd == "rel") {
            if (shellList.size() != 3) {
                cout << "parameter error" << endl;
                continue;
            }
            stringstream stream;
            int num;
            stream << shellList[2];
            stream >> num;
            manager.release(shellList[1], num);
        } else if (cmd == "to") {
            if (shellList.size() != 1) {
                cout << "parameter error" << endl;
                continue;
            }
            manager.timeout();
        } else if (cmd == "list") {
            if (shellList.size() != 2) {
                cout << "parameter error" << endl;
                continue;
            }
            if (shellList[1] == "ready")
                manager.listReady();
            else if (shellList[1] == "block")
                manager.listBlock();
            else if (shellList[1] == "res")
                manager.listResource();
            else if (shellList[1] == "pro")
                manager.listProcess();
            else
                cout << "command error" << endl;
        } else if (cmd == "pr") {
            if (shellList.size() != 2) {
                cout << "parameter error" << endl;
                continue;
            }
            manager.printPCB(shellList[1]);
        } else if (cmd == "exit") {
            exit(0);
        } else
            cout << "command error" << endl;
    }
    return 0;
}