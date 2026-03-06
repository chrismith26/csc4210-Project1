#ifndef PCB_H
#define PCB_H

#include <string>
#include <iostream>

enum State {
    NEW,
    READY,
    RUNNING,
    TERMINATED
};

struct PCB {
    std::string pid;
    int arrival_time;
    int total_burst;
    int remaining_time;
    int priority;
    State state;
    int start_time;      // First time CPU executes
    int completion_time; // When process finishes
    int time_in_running; // For RR quantum tracking

    PCB() : pid(""), arrival_time(0), total_burst(0), remaining_time(0),
            priority(0), state(NEW), start_time(-1), completion_time(-1), 
            time_in_running(0) {}

    PCB(std::string p, int arr, int burst, int prio)
        : pid(p), arrival_time(arr), total_burst(burst), 
          remaining_time(burst), priority(prio), state(NEW),
          start_time(-1), completion_time(-1), time_in_running(0) {}

    std::string getStateString() const {
        switch (state) {
            case NEW:       return "NEW";
            case READY:     return "READY";
            case RUNNING:   return "RUNNING";
            case TERMINATED: return "TERMINATED";
            default:        return "UNKNOWN";
        }
    }

    void print() const {
        std::cout << "PID = " << pid 
                  << " Arr =" << arrival_time 
                  << " Burst =" << total_burst 
                  << " Rem =" << remaining_time 
                  << " Prio =" << priority 
                  << " State = " << getStateString() << std::endl;
    }
};

#endif // PCB_H