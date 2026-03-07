#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <fstream>
#include <sstream>
#include <vector>
#include "pcb.h"
using namespace std;

class InputParser {
public:
    static vector<PCB> parseInput(const string& filename) {
        vector<PCB> processes;
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return processes;
        }

        string line;
        // Skip header line
        getline(file, line);

        while (getline(file, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            string pid;
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