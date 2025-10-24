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

// Replace input file placeholders ($input and ${input}) with wavefunction filename without extension
std::string InputParser::replaceInputPlaceholders(const std::string& text, const std::string& wfnFile) {
    std::string result = text;
    
    // Extract filename without extension from wavefunction file path
    std::string wfnBaseName = getBaseName(wfnFile);
    
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        std::string varName;
        
        // Check for ${input} syntax
        if (endPos < result.length() && result[endPos] == '{') {
            size_t braceStart = endPos + 1;
            size_t braceEnd = result.find('}', braceStart);
            if (braceEnd != std::string::npos) {
                varName = result.substr(braceStart, braceEnd - braceStart);
                endPos = braceEnd + 1;
            } else {
                // No closing brace found, treat as regular $variable
                while (endPos < result.length() && 
                       (isalnum(result[endPos]) || result[endPos] == '_')) {
                    endPos++;
                }
                varName = result.substr(pos + 1, endPos - pos - 1);
            }
        } else {
            // Regular $variable syntax
            while (endPos < result.length() && 
                   (isalnum(result[endPos]) || result[endPos] == '_')) {
                endPos++;
            }
            varName = result.substr(pos + 1, endPos - pos - 1);
        }
        
        // Only replace if the variable name is "input"
        if (varName == "input") {
            result.replace(pos, endPos - pos, wfnBaseName);
            pos += wfnBaseName.length();
        } else {
            // Skip this placeholder, move to next
            pos = endPos;
        }
    }
    
    return result;
}

// Apply placeholder replacement to all tasks using wavefunction filename
void InputParser::applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile) {
    for (auto& task : tasks) {
        // Apply replacement to parameter values
        for (auto& param : task.params) {
            param.second = replaceInputPlaceholders(param.second, wfnFile);
        }
        
        // Apply replacement to post-processing step parameters
        for (auto& step : task.postProcessSteps) {
            for (auto& param : step.second) {
                param.second = replaceInputPlaceholders(param.second, wfnFile);
            }
        }
        
        // Apply replacement to commands
        for (auto& command : task.commands) {
            command = replaceInputPlaceholders(command, wfnFile);
        }
    }
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
            // Command mode: store the entire line as a command (placeholder replacement will be done later)
            currentTask.commands.push_back(line);
        } else {
            std::vector<std::string> tokens = split(line, ' ');
            if (tokens.empty()) continue;
            
            if (!inProcessMode) {
                // Pre-processing parameter setting mode
                if (tokens.size() >= 2) {
                    // Store parameter value (placeholder replacement will be done later)
                    currentTask.params[tokens[0]] = tokens[1];
                }
            } else {
                // Post-processing command mode
                std::string sectionName = tokens[0];
                std::map<std::string, std::string> sectionParams;
                
                // Parse subsequent parameters
                for (size_t i = 1; i < tokens.size(); i += 2) {
                    if (i + 1 < tokens.size()) {
                        // Store parameter value (placeholder replacement will be done later)
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
