#include "simulator.h"
using namespace std;

const double context_switch_time = 14;

Process::Process(int burst, int time, int id, int importance, Process_state current_state){
    pid = id;
    burst_time = burst;
    burst_time_left = burst;
    arrival_time = time;
    state = current_state;
    priority = importance;

    start_burst_time = 0;
};

void Process::print(){
    cout << "Process [" << pid << "] Arrival at " << arrival_time << "ms; Burst-time " << burst_time << "ms; Priority " << priority << "\n";
}

Statistics::Statistics() {
    number_processes = 0;

    min_turnaround_time = std::numeric_limits<double>::max();
    average_turnaround_time = std::numeric_limits<double>::max();
    max_turnaround_time = 0;

    min_wait_time = std::numeric_limits<double>::max();
    average_wait_time = std::numeric_limits<double>::max();
    max_wait_time = 0;

    CPU_usage_time = 0;
    CPU_idle_time = 0;

    log = {};
}

void Statistics::update_completed_process(double turnaround_time, double wait_time, double cpu_time, double idle_time, int process_id){
    number_processes++;

    if (turnaround_time < min_turnaround_time) min_turnaround_time = turnaround_time;
    if (turnaround_time > max_turnaround_time) max_turnaround_time = turnaround_time;
    if ( number_processes == 1 ){ average_turnaround_time = turnaround_time; } else { average_turnaround_time = average_turnaround_time + ((turnaround_time - average_turnaround_time) / number_processes); }

    if (wait_time < min_wait_time) min_wait_time = wait_time;
    if (wait_time > max_wait_time) max_wait_time = wait_time;
    if ( number_processes == 1 ){ average_wait_time = wait_time; } else { average_wait_time = average_wait_time + ((wait_time - average_wait_time) / number_processes); }

    CPU_usage_time += cpu_time;
    CPU_idle_time += idle_time;
    total_time = CPU_usage_time + CPU_idle_time;

    log.push_back(make_tuple(-1, idle_time));
    log.push_back(make_tuple(process_id, cpu_time));
}

void Statistics::update_partial_process(double cpu_time, double idle_time, int process_id){
    CPU_usage_time += cpu_time;
    CPU_idle_time += idle_time;
    total_time = CPU_usage_time + CPU_idle_time;       

    log.push_back(make_tuple(-1, idle_time));
    log.push_back(make_tuple(process_id, cpu_time));
}

void Statistics::print(){
    cout << "Total wait time: min " << min_wait_time << "ms; avg " << average_wait_time << "ms; max " << max_wait_time << "ms \n"; 
    cout << "Turnaround time: min " << min_turnaround_time << "ms; avg " << average_turnaround_time << "ms; max " << max_turnaround_time << "ms \n"; 
    cout << "Total time was " << total_time << "ms, CPU was in usage for " << CPU_usage_time << "ms and was idle for " << CPU_idle_time << "ms \n"; 
    cout << "CPU Usage was [" << CPU_usage_time * 100 / total_time << "%] and idle-time was [" << CPU_idle_time * 100 / total_time << "%] \n";
}

bool sort_by_total_burst_time::operator()(Process * a, Process * b){
    return a->burst_time > b->burst_time;
}

bool sort_by_priority::operator()(Process * a, Process * b){
    return a->priority > b->priority;
}

bool sort_by_burst_time_left::operator()(Process * a, Process * b){
    return a->burst_time_left > b->burst_time_left;
}   

CPU::CPU(){
    clock = 0.0;
}

void CPU::restart(){
    clock = 0.0;
}

void CPU::first_come_first_serve(Statistics &stats , vector<Process> processes){
    queue<Process *> ready_queue;
    double last_completion_time = 0;

    while (stats.number_processes < processes.size()){
        // Search for processes that were created and just arrived

        for(int i = 0; i < processes.size(); i++){
            if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                processes[i].state = Process_state::READY; 
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

        stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        last_completion_time = completion_time;
    }
}

void CPU::shortest_job_first( Statistics &stats , vector<Process> processes){
    priority_queue<Process *, vector<Process *>, sort_by_total_burst_time> ready_queue;
    double last_completion_time = 0;

    while (stats.number_processes < processes.size()){
        // Search for processes that were created and just arrived

        for(int i = 0; i < processes.size(); i++){
            if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                processes[i].state = Process_state::READY; 
                ready_queue.push(&processes[i]); 
            }
        }

        // If ready_queue is empty and there is no process running then just increment clock until a new process arrives

        if (ready_queue.empty()) {
            clock++;
            continue;
        }

        // Select first Process in the queue which is the one with the shortest burst time

        Process * current = ready_queue.top();

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

        stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        last_completion_time = completion_time;
    }
}

void CPU::random_selection( Statistics &stats , vector<Process> processes){
    vector<Process *> ready_queue;
    double last_completion_time = 0;

    while (stats.number_processes < processes.size()){
        // Search for processes that were created and just arrived

        for(int i = 0; i < processes.size(); i++){
            if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                processes[i].state = Process_state::READY; 
                ready_queue.push_back(&processes[i]); 
            }
        }

        // If ready_queue is empty and there is no process running then just increment clock until a new process arrives

        if (ready_queue.empty()) {
            clock++;
            continue;
        }

        // Select random Process in the queue
        int random_index = (rand()%ready_queue.size());
        Process * current = ready_queue[random_index];

        // Change the context in the CPU

        clock += context_switch_time;

        // Start processing

        double start_burst_time = clock;
        clock += current->burst_time;

        // Completed!

        double completion_time = clock;
        current->state = Process_state::TERMINATED;
        ready_queue.erase(ready_queue.begin() + random_index);

        // Calculations for analysis

        double process_turnaround_time = completion_time - current->arrival_time;
        double process_wait_time = process_turnaround_time - current->burst_time;
        double cpu_usage_time = completion_time - start_burst_time;
        double cpu_idle_time = start_burst_time - last_completion_time;

        stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        last_completion_time = completion_time;
    }
}

void CPU::non_preemptive_priority( Statistics &stats , vector<Process> processes){
    priority_queue<Process *, vector<Process *>, sort_by_priority> ready_queue;
    double last_completion_time = 0;

    while (stats.number_processes < processes.size()){
        // Search for processes that were created and just arrived

        for(int i = 0; i < processes.size(); i++){
            if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                processes[i].state = Process_state::READY; 
                ready_queue.push(&processes[i]); 
            }
        }

        // If ready_queue is empty and there is no process running then just increment clock until a new process arrives

        if (ready_queue.empty()) {
            clock++;
            continue;
        }

        // Select first Process in the queue which is the one with the lowest priority

        Process * current = ready_queue.top();

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

        stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        last_completion_time = completion_time;
    }
}

void CPU::round_robin( Statistics &stats , vector<Process> processes){
    queue<Process *> ready_queue;
    queue<Process *> blocked_queue;
    int time_quantum = 500;

    while (stats.number_processes < processes.size()){
        // Search for processes that were created and arrived

        for(int i = 0; i < processes.size(); i++){
            if (processes[i].arrival_time <= clock && processes[i].state == Process_state::NEW) { 
                processes[i].state = Process_state::READY; 
                ready_queue.push(&processes[i]); 
            }
        }

        // Push also any waiting process

        for (int i = 0; i < blocked_queue.size(); i++){
            blocked_queue.front()->state = Process_state::READY;
            ready_queue.push(blocked_queue.front());
            blocked_queue.pop();
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
        unsigned int start_of_processing = clock;

        // Start processing
        if (current->burst_time_left == current->burst_time){
            current->start_burst_time = clock;
        }
        
        if(current->burst_time_left <= time_quantum){
            clock += current->burst_time_left;
            current->burst_time_left = 0;

            // Completed!

            double completion_time = clock;
            current->state = Process_state::TERMINATED;

            // Calculations for analysis

            double process_turnaround_time = completion_time - current->arrival_time;
            double process_wait_time = process_turnaround_time - current->burst_time;
            double cpu_usage_time = completion_time - start_of_processing;
            double cpu_idle_time = context_switch_time;

            stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        } else {
            clock += time_quantum;
            current->burst_time_left = current->burst_time_left - time_quantum;
            current->state = Process_state::BLOCKED;
            blocked_queue.push(current);

            stats.update_partial_process(time_quantum, context_switch_time, current->pid);
        }

        ready_queue.pop();
    }
}

void CPU::shortest_remaining_time_first( Statistics &stats , vector<Process> processes){
    priority_queue<Process *, vector<Process *>, sort_by_burst_time_left> ready_queue;
    queue<Process *> blocked_queue;

    int current_p = 0;

    while (stats.number_processes < processes.size()){
        // Check for processes that were created and arrived

        if (current_p <= processes.size() - 1 && processes[current_p].arrival_time <= clock && processes[current_p].state == Process_state::NEW){ 
            processes[current_p].state = Process_state::READY; 
            ready_queue.push(&processes[current_p]);
            current_p++;
            continue;
        }

        // Push also any waiting process

        for (int i = 0; i < blocked_queue.size(); i++){
            blocked_queue.front()->state = Process_state::READY;
            ready_queue.push(blocked_queue.front());
            blocked_queue.pop();
        }

        // If ready_queue is empty then just increment clock until a new process arrives

        if (ready_queue.empty()) {
            clock++;
            continue;
        }

        // Select first Process in the queue

        Process * current = ready_queue.top();

        // Change the context in the CPU

        clock += context_switch_time;
        unsigned int start_of_processing = clock;

        // Start processing
        if (current->burst_time_left == current->burst_time){
            current->start_burst_time = clock;
        }

        // Check if process is going to get interrupted while processing, this means a process with a shorter burst_time_left arrives
        // Also get the time of interruption so we can fast forward to that timestamp

        bool is_going_to_get_interrupted = false;
        int time_of_interruption = -1;

        for (int i = current_p; i < processes.size(); i++){
            if (current->burst_time_left - (processes[i].arrival_time - clock) > processes[i].burst_time_left){
                is_going_to_get_interrupted = true;
                time_of_interruption = processes[i].arrival_time;
                break;
            }
            if (processes[i].arrival_time > clock + current->burst_time_left){
                break;
            }
        }
        
        if(!is_going_to_get_interrupted){
            clock += current->burst_time_left;
            current->burst_time_left = 0;

            // Completed!

            double completion_time = clock;
            current->state = Process_state::TERMINATED;

            // Calculations for analysis

            double process_turnaround_time = completion_time - current->arrival_time;
            double process_wait_time = process_turnaround_time - current->burst_time;
            double cpu_usage_time = completion_time - start_of_processing;
            double cpu_idle_time = context_switch_time;

            stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        } else {
            clock = time_of_interruption;
            current->burst_time_left -= (clock - start_of_processing);
            current->state = Process_state::BLOCKED;
            blocked_queue.push(current);

            stats.update_partial_process(clock - start_of_processing, context_switch_time, current->pid);
        }

        ready_queue.pop();
    }
}

void CPU::preemptive_priority( Statistics &stats , vector<Process> processes){
    priority_queue<Process *, vector<Process *>, sort_by_priority> ready_queue;
    queue<Process *> blocked_queue;

    int current_p = 0;

    while (stats.number_processes < processes.size()){
        // Check for processes that were created and arrived

        if (current_p <= processes.size() - 1 && processes[current_p].arrival_time <= clock && processes[current_p].state == Process_state::NEW){ 
            processes[current_p].state = Process_state::READY; 
            ready_queue.push(&processes[current_p]);
            current_p++;
            continue;
        }

        // Push also any waiting process

        for (int i = 0; i < blocked_queue.size(); i++){
            blocked_queue.front()->state = Process_state::READY;
            ready_queue.push(blocked_queue.front());
            blocked_queue.pop();
        }

        // If ready_queue is empty then just increment clock until a new process arrives

        if (ready_queue.empty()) {
            clock++;
            continue;
        }

        // Select first Process in the queue

        Process * current = ready_queue.top();

        // Change the context in the CPU

        clock += context_switch_time;
        unsigned int start_of_processing = clock;

        // Start processing
        if (current->burst_time_left == current->burst_time){
            current->start_burst_time = clock;
        }

        // Check if process is going to get interrupted while processing, this means a process with a lower priority arrives
        // Also get the time of interruption so we can fast forward to that timestamp
        
        bool is_going_to_get_interrupted = false;
        int time_of_interruption = -1;

        for (int i = current_p; i < processes.size(); i++){
            if (current->priority > processes[i].priority){
                is_going_to_get_interrupted = true;
                time_of_interruption = processes[i].arrival_time;
                break;
            }
            if (processes[i].arrival_time > clock + current->burst_time_left){
                break;
            }
        }
        
        if(!is_going_to_get_interrupted){
            clock += current->burst_time_left;
            current->burst_time_left = 0;

            // Completed!

            double completion_time = clock;
            current->state = Process_state::TERMINATED;

            // Calculations for analysis

            double process_turnaround_time = completion_time - current->arrival_time;
            double process_wait_time = process_turnaround_time - current->burst_time;
            double cpu_usage_time = completion_time - start_of_processing;
            double cpu_idle_time = context_switch_time;

            stats.update_completed_process(process_turnaround_time, process_wait_time, cpu_usage_time, cpu_idle_time, current->pid);

        } else {
            clock = time_of_interruption;
            current->burst_time_left -= (clock - start_of_processing);
            current->state = Process_state::BLOCKED;
            blocked_queue.push(current);

            stats.update_partial_process(clock - start_of_processing, context_switch_time, current->pid);
        }

        ready_queue.pop();
    }
}    


vector<Process> create_random_processes(int n){
	//Sets number of processes
    int num_processes = n;

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
        random_burst = (rand()%1000)+100;
		importance = (rand()%5);
		processes.push_back(Process(random_burst, 0, i+1, importance, Process_state::NEW));
	}

	//Creates processes with non zero arrival time
	for(int i = 0; i < num_rand; i++){
        random_burst = (rand()%1000)+100;
		importance = (rand()%5);
		double lambda = 0.001;
		double r = ((double) rand()/(RAND_MAX));
		double x = -(log(r)/lambda);
		if ( x > 8000 ) { i--; continue; }
		processes.push_back(Process(random_burst, (int)x, (num_zero + i+1), importance, Process_state::NEW));
    }

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](Process a, Process b){ return a.arrival_time < b.arrival_time; });

    return processes;
}
