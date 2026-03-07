#include <iostream>
#include <vector>
#include <iomanip>
#include "pcb.h"
#include "input_parser.h"
#include "queue.h"

struct GanttEntry {
    std::string pid;
    int start_time;
    int end_time;
};

int main() {
    std::vector<PCB> processes = InputParser::parseInput("input.txt");

    if (processes.empty()) {
        std::cerr << "No processes found in input.txt" << std::endl;
        return 1;
    }

    std::cout << "=====================================" << std::endl;
    std::cout << "   SRTF CPU Scheduling Simulator" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;

    Queue ready_queue;
    PCB running_process;
    bool has_running = false;
    int current_time = 0;
    int terminated_count = 0;
    std::vector<GanttEntry> gantt_chart;
    int last_gantt_time = 0;

    while (terminated_count < (int)processes.size()) {
        // Admit newly arrived processes
        for (auto& p : processes) {
            if (p.state == NEW && p.arrival_time == current_time) {
                p.state = READY;
                ready_queue.enqueue(p);
            }
        }

        // Check for preemption: if a new READY process has shorter remaining time
        if (has_running && !ready_queue.isEmpty()) {
            PCB next = ready_queue.peek();
            if (next.remaining_time < running_process.remaining_time) {
                // Preempt: move running back to ready
                running_process.state = READY;
                ready_queue.enqueue(running_process);

                // Start the process with shorter time
                running_process = ready_queue.dequeue();
                running_process.state = RUNNING;
                last_gantt_time = current_time;

                // Mark start time if first execution
                for (auto& p : processes) {
                    if (p.pid == running_process.pid && p.start_time == -1) {
                        p.start_time = current_time;
                        running_process.start_time = current_time;
                    }
                }
            }
        }

        // Start a process if CPU is idle
        if (!has_running && !ready_queue.isEmpty()) {
            running_process = ready_queue.dequeue();
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
        std::cout << "Time " << current_time << ":" << std::endl;
        std::cout << "RUNNING :" << std::endl;
        if (has_running) {
            running_process.print();
        } else {
            std::cout << "IDLE" << std::endl;
        }
        std::cout << "READY :" << std::endl;
        if (!ready_queue.isEmpty()) {
            int count;
            PCB* ready_procs = ready_queue.getAllInOrder(count);
            for (int i = 0; i < count; i++) {
                ready_procs[i].print();
            }
            delete[] ready_procs;
        }
        std::cout << std::endl;

        current_time++;
    }

    // Print Gantt Chart
    std::cout << "Gantt Chart:" << std::endl;
    for (const auto& entry : gantt_chart) {
        std::cout << "| " << entry.pid << " ";
    }
    std::cout << "|" << std::endl;
    std::cout << "0";
    for (const auto& entry : gantt_chart) {
        std::cout << std::setw(4) << entry.end_time;
    }
    std::cout << std::endl;

    // Print statistics
    std::cout << "\n=== Scheduling Statistics ===" << std::endl;
    double total_waiting = 0, total_turnaround = 0, total_response = 0;

    for (const auto& p : processes) {
        if (p.state == TERMINATED) {
            int response = p.start_time - p.arrival_time;
            int turnaround = p.completion_time - p.arrival_time;
            int waiting = turnaround - p.total_burst;

            std::cout << "PID " << p.pid << ": "
                      << "Response=" << response << ", "
                      << "Waiting=" << waiting << ", "
                      << "Turnaround=" << turnaround << std::endl;

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
        std::cout << "\nAverage Response Time: " 
                  << (double)total_response / completed << std::endl;
        std::cout << "Average Waiting Time: " 
                  << (double)total_waiting / completed << std::endl;
        std::cout << "Average Turnaround Time: " 
                  << (double)total_turnaround / completed << std::endl;
    }

    return 0;
}