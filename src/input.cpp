#include "input.h"
#include "config.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Utility function: split string (deprecated - use Utils::split instead)
std::vector<std::string> InputParser::split(const std::string& str, char delimiter) {
    return Utils::split(str, delimiter);
}

// Parse inp file, return all module tasks and optional wfn file
std::pair<std::vector<ModuleTask>, std::string> InputParser::parseInpFileWithWfn(const std::string& inpFile) {
    std::vector<ModuleTask> tasks;
    std::string wfnFile;
    std::ifstream file(inpFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open inp file: " << inpFile << std::endl;
        return {tasks, wfnFile};
    }
    
    std::string line;
    ModuleTask currentTask;
    bool inProcessMode = false;
    bool inCommandMode = false;
    std::map<std::string, int> moduleBlockCounters;  // Track block indices for each module name
    
    while (std::getline(file, line)) {
        line = ::trim(line);
        
        // 去除行内注释
        line = Utils::removeInlineComment(line);
        
        if (line.empty()) continue;
        
        // Check for wfn=xx format at the beginning of file
        if (line.find("wfn=") == 0 && currentTask.moduleName.empty()) {
            wfnFile = line.substr(4); // Extract everything after "wfn="
            continue;
        }
        
        // Module start [module_name]
        if (line[0] == '[' && line.back() == ']') {
            // If there is an unfinished task, save it
            if (!currentTask.moduleName.empty()) {
                tasks.push_back(currentTask);
            }
            
            currentTask = ModuleTask();
            currentTask.moduleName = line.substr(1, line.length() - 2); // Remove [ and ]
            currentTask.useWait = false;
            currentTask.blockIndex = moduleBlockCounters[currentTask.moduleName]++; // Assign unique index
            inProcessMode = false;
            inCommandMode = false;
            continue;
        }
        
        // Enter post-processing mode
        if (line == "%process") {
            if (currentTask.moduleName.empty()) {
                std::cerr << "Warning: %process without module definition" << std::endl;
                continue;
            }
            inProcessMode = true;
            inCommandMode = false;
            continue;
        }
        
        // Enter command mode
        if (line == "%command") {
            if (currentTask.moduleName.empty()) {
                std::cerr << "Warning: %command without module definition" << std::endl;
                continue;
            }
            inCommandMode = true;
            inProcessMode = false;
            continue;
        }
        
        // End current module with "end"
        if (line == "end") {
            if (!currentTask.moduleName.empty()) {
                tasks.push_back(currentTask);
                currentTask = ModuleTask();
            }
            inProcessMode = false;
            inCommandMode = false;
            continue;
        }
        
        // End current module with "wait" (interactive mode)
        if (line == "wait") {
            if (!currentTask.moduleName.empty()) {
                currentTask.useWait = true;
                tasks.push_back(currentTask);
                currentTask = ModuleTask();
            }
            inProcessMode = false;
            inCommandMode = false;
            continue;
        }
        
        // Parse parameters, post-processing commands, or shell commands
        if (inCommandMode) {
            // Command mode: store the entire line as a command
            currentTask.commands.push_back(line);
        } else {
            std::vector<std::string> tokens = split(line, ' ');
            if (tokens.empty()) continue;
            
            if (!inProcessMode) {
                // Pre-processing parameter setting mode
                if (tokens.size() >= 2) {
                    currentTask.params[tokens[0]] = tokens[1];
                }
            } else {
                // Post-processing command mode
                std::string sectionName = tokens[0];
                std::map<std::string, std::string> sectionParams;
                
                // Parse subsequent parameters
                for (size_t i = 1; i < tokens.size(); i += 2) {
                    if (i + 1 < tokens.size()) {
                        sectionParams[tokens[i]] = tokens[i + 1];
                    }
                }
                
                currentTask.postProcessSteps.push_back({sectionName, sectionParams});
            }
        }
    }
    
    // Save the last task
    if (!currentTask.moduleName.empty()) {
        tasks.push_back(currentTask);
    }
    
    file.close();
    return {tasks, wfnFile};
}

// Parse inp file, return all module tasks (backward compatibility)
std::vector<ModuleTask> InputParser::parseInpFile(const std::string& inpFile) {
    auto result = parseInpFileWithWfn(inpFile);
    return result.first;
}
