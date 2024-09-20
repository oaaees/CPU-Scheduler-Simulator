#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <queue>
#include <algorithm> 
#include <iostream>
#include <limits>
#include <math.h>
using namespace std;

#include <QString>

enum Process_state {
    NEW,
    RUNNING,
    BLOCKED,
    READY,
    TERMINATED    
};

class Process {
    public:
        int arrival_time;
        int burst_time;
        int burst_time_left;
        Process_state state;
        int priority;
        int pid;

        int start_burst_time;
        
        Process(int burst, int time, int id, int importance, Process_state current_state);
        void print();
        QString get_info();
};

class Statistics {
    public:
        unsigned int number_processes;

        double min_turnaround_time;
        double average_turnaround_time;
        double max_turnaround_time;

        double min_wait_time;
        double average_wait_time;
        double max_wait_time;

        double CPU_usage_time;
        double CPU_idle_time;
        double total_time;

        vector<tuple<int, int>> log;

        Statistics ();

        void update_completed_process(double turnaround_time, double wait_time, double cpu_time, double idle_time, int process_id);
        void update_partial_process(double cpu_time, double idle_time, int process_id);

        void print();
};

class sort_by_total_burst_time {
    public:
        bool operator()(Process * a, Process * b);
};

class sort_by_priority {
    public:
        bool operator()(Process * a, Process * b);
};

class sort_by_burst_time_left {
    public:
        bool operator()(Process * a, Process * b);
};

class CPU {
    public:
    int clock;

    CPU ();
    void restart();

    void first_come_first_serve(Statistics &stats , vector<Process> processes);
    void shortest_job_first(Statistics &stats , vector<Process> processes);
    void random_selection(Statistics &stats , vector<Process> processes);
    void non_preemptive_priority(Statistics &stats , vector<Process> processes);
    void round_robin(Statistics &stats , vector<Process> processes);
    void shortest_remaining_time_first( Statistics &stats , vector<Process> processes);
    void preemptive_priority( Statistics &stats , vector<Process> processes); 
    
};

vector<Process> create_random_processes(int n);

#endif // SIMULATOR_H
