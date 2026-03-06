#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <fstream>
#include <sstream>
#include <vector>
#include "pcb.h"

class InputParser {
public:
    static std::vector<PCB> parseInput(const std::string& filename) {
        std::vector<PCB> processes;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return processes;
        }

        std::string line;
        // Skip header line
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string pid;
            int arrival, burst, priority;

            if (iss >> pid >> arrival >> burst >> priority) {
                processes.push_back(PCB(pid, arrival, burst, priority));
            }
        }

        file.close();
        return processes;
    }
};

#endif // INPUT_PARSER_H