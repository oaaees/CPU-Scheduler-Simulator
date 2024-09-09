#include <vector>
#include <queue>
#include <algorithm> 
#include <iostream>
#include <limits>
#include <math.h>
using namespace std;

const double context_switch_time = 14;

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
        Process_state state;
        int priority;
        int pid;
        
        Process(int burst, int time, int id, int importance, Process_state current_state){
            pid = id;
            burst_time = burst;
            arrival_time = time;
            state = current_state;
            priority = importance;
        };

        void print(){
            cout << "Proceso [" << pid << "] Arrival at " << arrival_time << "ms; Burst-time " << burst_time << "ms; Priority " << priority << "\n";
        }

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

    Statistics () {
        number_processes = 0;

        min_turnaround_time = std::numeric_limits<double>::max();
        average_turnaround_time = std::numeric_limits<double>::max();
        max_turnaround_time = 0;

        min_wait_time = std::numeric_limits<double>::max();
        average_wait_time = std::numeric_limits<double>::max();
        max_wait_time = 0;

        CPU_usage_time = 0;
        CPU_idle_time = 0;
    }

    void update(double turnaround_time, double wait_time, double burst_time, double idle_time){
        number_processes++;

        if (turnaround_time < min_turnaround_time) min_turnaround_time = turnaround_time;
        if (turnaround_time > max_turnaround_time) max_turnaround_time = turnaround_time;
        if ( number_processes == 1 ){ average_turnaround_time = turnaround_time; } else { average_turnaround_time = average_turnaround_time + ((turnaround_time - average_turnaround_time) / number_processes); }

        if (wait_time < min_wait_time) min_wait_time = wait_time;
        if (wait_time > max_wait_time) max_wait_time = wait_time;
        if ( number_processes == 1 ){ average_wait_time = wait_time; } else { average_wait_time = average_wait_time + ((wait_time - average_wait_time) / number_processes); }
    
        CPU_usage_time += burst_time;
        CPU_idle_time += idle_time;
        total_time = CPU_usage_time + CPU_idle_time;
    }

    void print(){
        cout << "Turnaround time: min " << min_turnaround_time << "ms; avg " << average_turnaround_time << "ms; max " << max_turnaround_time << "ms \n"; 
        cout << "Total wait time: min " << min_wait_time << "ms; avg " << average_wait_time << "ms; max " << max_wait_time << "ms \n"; 
        cout << "Total time was " << total_time << "ms, CPU was in usage for " << CPU_usage_time << "ms and was idle for " << CPU_idle_time << "ms \n"; 
        cout << "CPU Usage was [" << CPU_usage_time * 100 / total_time << "%] and idle-time was [" << CPU_idle_time * 100 / total_time << "%] \n";
    }
};



class CPU {
    public:
    int clock;

    CPU (){
        clock = 0.0;
    }

    void first_come_first_serve( Statistics &stats , vector<Process> processes){
        queue<Process *> ready_queue;
        double last_completion_time = 0;

        while (stats.number_processes < processes.size()){
            // Search for processes that were created and just arrived

            for(int i = 0; i < processes.size(); i++){
                if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                    processes[i].state = READY; 
                    ready_queue.push(&processes[i]); 
                }
            }

            // If ready_queue is empty and there is no process running then just increment clock until a new process arrives

            if (ready_queue.empty()) {
                clock++;
                continue;
            }

            // Select first Process in the queue

            Process * current = ready_queue.front();

            // Change the context in the CPU

            clock += context_switch_time;

            // Start processing

            double start_burst_time = clock;
            clock += current->burst_time;

            // Completed!

            double completion_time = clock;
            current->state = Process_state::TERMINATED;
            ready_queue.pop();

            // Calculations for analysis

            double process_turnaround_time = completion_time - current->arrival_time;
            double process_wait_time = process_turnaround_time - current->burst_time;
            double cpu_usage_time = completion_time - start_burst_time;
            double cpu_idle_time = start_burst_time - last_completion_time;

            stats.update(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time);

            last_completion_time = completion_time;
        }
    }
};


int main(){

	//Sets number of processes
	int num_processes = 3;

	srand((unsigned)time(0));
	int random_burst;
	int importance;

	//Vector will hold the processes
	vector<Process> processes;

	//80% of processes have a semi random arrival time determined by exponential distribution
	//20% of processes have 0 arrival time
	int num_rand = num_processes * .8;
	int num_zero = num_processes - num_rand;

	//Creates processes with zero arrival time
	for (int i = 0; i < num_zero; i++){
		random_burst = (rand()%3500)+500;
		importance = (rand()%5);
		processes.push_back(Process(random_burst, 0, i+1, importance, Process_state::NEW));
	}

	//Creates processes with non zero arrival time
	for(int i = 0; i < num_rand; i++){
		random_burst = (rand()%3500)+500;
		importance = (rand()%5);
		double lambda = 0.001;
		double r = ((double) rand()/(RAND_MAX));
		double x = -(log(r)/lambda);
		if ( x > 8000 ) { i--; continue; }
		processes.push_back(Process(random_burst, (int)x, (num_zero + i+1), importance, Process_state::NEW));
    }

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](Process a, Process b){ return a.arrival_time < b.arrival_time; });

    for(int i = 0; i < processes.size(); i++) processes[i].print();
    cout << "\n\n";

    CPU cpu;
    Statistics stats;

    cpu.first_come_first_serve(stats, processes);
    stats.print();

    return 0;
}