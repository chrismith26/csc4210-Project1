#include <iostream>
#include <vector>
#include <iomanip>
#include "pcb.h"
#include "input_parser.h"
#include "queue.h"

using namespace std;

struct GanttEntry {
    string pid;
    int start_time;
    int end_time;
};

int main() {
    vector<PCB> processes = InputParser::parseInput("input.txt");

    if (processes.empty()) {
        cerr << "No processes found in input.txt" << endl;
        return 1;
    }

    cout << "=====================================" << endl;
    cout << "   Priority CPU Scheduling Simulator" << endl;
    cout << "=====================================" << endl << endl;

    Queue ready_queue;
    PCB running_process;
    bool has_running = false;
    int current_time = 0;
    int terminated_count = 0;
    vector<GanttEntry> gantt_chart;
    int last_gantt_time = 0;

    while (terminated_count < (int)processes.size()) {
        // Admit newly arrived processes
        for (auto& p : processes) {
            if (p.state == NEW && p.arrival_time == current_time) {
                p.state = READY;
                ready_queue.enqueue(p);
            }
        }

        // Start a process if CPU is idle - pick highest priority from ready queue
        if (!has_running && !ready_queue.isEmpty()) {
            // Find process with highest priority (lowest number)
            int count;
            PCB* ready_procs = ready_queue.getAllInOrder(count);
            
            int best_idx = 0;
            for (int i = 1; i < count; i++) {
                if (ready_procs[i].priority < ready_procs[best_idx].priority ||
                    (ready_procs[i].priority == ready_procs[best_idx].priority && 
                     ready_procs[i].arrival_time < ready_procs[best_idx].arrival_time) ||
                    (ready_procs[i].priority == ready_procs[best_idx].priority && 
                     ready_procs[i].arrival_time == ready_procs[best_idx].arrival_time && 
                     ready_procs[i].pid < ready_procs[best_idx].pid)) {
                    best_idx = i;
                }
            }

            // Store the selected process
            PCB selected = ready_procs[best_idx];
            delete[] ready_procs;
            
            // Clear the queue and rebuild without the selected process
            ready_procs = ready_queue.getAllInOrder(count);
            Queue new_queue;
            for (int i = 0; i < count; i++) {
                if (ready_procs[i].pid != selected.pid) {
                    new_queue.enqueue(ready_procs[i]);
                }
            }
            delete[] ready_procs;

            // Now clear old queue and copy new queue back
            while (!ready_queue.isEmpty()) {
                ready_queue.dequeue();
            }
            int new_count;
            PCB* new_procs = new_queue.getAllInOrder(new_count);
            for (int i = 0; i < new_count; i++) {
                ready_queue.enqueue(new_procs[i]);
            }
            delete[] new_procs;

            running_process = selected;
            running_process.state = RUNNING;
            has_running = true;
            last_gantt_time = current_time;

            // Mark start time if first execution
            for (auto& p : processes) {
                if (p.pid == running_process.pid && p.start_time == -1) {
                    p.start_time = current_time;
                    running_process.start_time = current_time;
                }
            }
        }

        // Execute one time unit if a process is running
        if (has_running) {
            running_process.remaining_time--;

            // Check if process completed
            if (running_process.remaining_time <= 0) {
                running_process.completion_time = current_time + 1;
                running_process.state = TERMINATED;

                // Add to gantt chart
                if (gantt_chart.empty() || gantt_chart.back().pid != running_process.pid) {
                    gantt_chart.push_back({running_process.pid, last_gantt_time, current_time + 1});
                } else {
                    gantt_chart.back().end_time = current_time + 1;
                }

                // Update in processes
                for (auto& p : processes) {
                    if (p.pid == running_process.pid) {
                        p = running_process;
                    }
                }

                terminated_count++;
                has_running = false;
            }
        }

        // Print PCB trace
        cout << "Time " << current_time << ":" << endl;
        cout << "RUNNING :" << endl;
        if (has_running) {
            running_process.print();
        } else {
            cout << "IDLE" << endl;
        }
        cout << "READY :" << endl;
        if (!ready_queue.isEmpty()) {
            int count;
            PCB* ready_procs = ready_queue.getAllInOrder(count);
            for (int i = 0; i < count; i++) {
                ready_procs[i].print();
            }
            delete[] ready_procs;
        }
        cout << endl;

        current_time++;
    }

    // Print Gantt Chart
    cout << "Gantt Chart:" << endl;
    for (const auto& entry : gantt_chart) {
        cout << "| " << entry.pid << " ";
    }
    cout << "|" << endl;
    cout << "0";
    for (const auto& entry : gantt_chart) {
        cout << setw(4) << entry.end_time;
    }
    cout << endl;

    // Print statistics
    cout << "\n=== Scheduling Statistics ===" << endl;
    double total_waiting = 0, total_turnaround = 0, total_response = 0;

    for (const auto& p : processes) {
        if (p.state == TERMINATED) {
            int response = p.start_time - p.arrival_time;
            int turnaround = p.completion_time - p.arrival_time;
            int waiting = turnaround - p.total_burst;

            cout << "PID " << p.pid << ": "
                      << "Response=" << response << ", "
                      << "Waiting=" << waiting << ", "
                      << "Turnaround=" << turnaround << endl;

            total_response += response;
            total_waiting += waiting;
            total_turnaround += turnaround;
        }
    }

    int completed = 0;
    for (const auto& p : processes) {
        if (p.state == TERMINATED) completed++;
    }

    if (completed > 0) {
        cout << "\nAverage Response Time: " 
                  << (double)total_response / completed << endl;
        cout << "Average Waiting Time: " 
                  << (double)total_waiting / completed << endl;
        cout << "Average Turnaround Time: " 
                  << (double)total_turnaround / completed << endl;
    }

    return 0;
}