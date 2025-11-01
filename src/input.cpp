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
// Additionally, support ${name} -> read value from file "name" in current directory (trimmed)
// Also support custom variables from command line or file header
std::string InputParser::replaceInputPlaceholders(const std::string& text, const std::string& wfnFile, const std::map<std::string, std::string>& customVars) {
    std::string result = text;
    
    // Extract filename without extension from wavefunction file path
    std::string wfnBaseName = getBaseName(wfnFile);
    
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        std::string varName;
        bool usedBraces = false;
        
        // Check for ${input} syntax
        if (endPos < result.length() && result[endPos] == '{') {
            size_t braceStart = endPos + 1;
            size_t braceEnd = result.find('}', braceStart);
            if (braceEnd != std::string::npos) {
                varName = result.substr(braceStart, braceEnd - braceStart);
                endPos = braceEnd + 1;
                usedBraces = true;
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
        
        std::string replacement;
        bool found = false;
        
        // Priority 1: Check custom variables from command line or file header
        auto customIt = customVars.find(varName);
        if (customIt != customVars.end()) {
            replacement = customIt->second;
            found = true;
        }
        // Priority 2: Check if the variable name is "input"
        else if (varName == "input") {
            replacement = wfnBaseName;
            found = true;
        }
        // Priority 3: For ${name} (braced) and name != input, attempt file-based replacement
        else if (usedBraces) {
            // Read file named exactly as varName from current working directory
            if (Utils::fileExists(varName)) {
                std::ifstream f(varName);
                if (f.good()) {
                    std::stringstream buffer;
                    buffer << f.rdbuf();
                    replacement = Utils::trim(buffer.str());
                    found = true;
                }
            }
        }
        
        if (found) {
            result.replace(pos, endPos - pos, replacement);
            pos += replacement.length();
        } else {
            // If no replacement found, skip this placeholder
            pos = endPos;
        }
    }
    
    return result;
}

// Apply placeholder replacement to all tasks using wavefunction filename and custom variables
void InputParser::applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile, const std::map<std::string, std::string>& customVars) {
    for (auto& task : tasks) {
        // Apply replacement to parameter values
        for (auto& param : task.params) {
            param.second = replaceInputPlaceholders(param.second, wfnFile, customVars);
        }
        
        // Apply replacement to post-processing step parameters
        for (auto& step : task.postProcessSteps) {
            for (auto& param : step.second) {
                param.second = replaceInputPlaceholders(param.second, wfnFile, customVars);
            }
        }
        
        // Apply replacement to commands
        for (auto& command : task.commands) {
            command = replaceInputPlaceholders(command, wfnFile, customVars);
        }
    }
}

// Parse inp file, return all module tasks, optional wfn file, core count, and custom variables
std::tuple<std::vector<ModuleTask>, std::string, int, std::map<std::string, std::string>> InputParser::parseInpFileWithWfnAndCoresAndVars(const std::string& inpFile) {
    std::vector<ModuleTask> tasks;
    std::string wfnFile;
    int cores = -1;  // -1 means not specified
    std::map<std::string, std::string> customVars;
    std::ifstream file(inpFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open inp file: " << inpFile << std::endl;
        return {tasks, wfnFile, cores, customVars};
    }
    
    std::string line;
    ModuleTask currentTask;
    bool inProcessMode = false;
    bool inCommandMode = false;
    std::map<std::string, int> moduleBlockCounters;  // Track block indices for each module name
    
    while (std::getline(file, line)) {
        // 保留前导空白用于模块行"顶格"判断
        std::string noComment = Utils::removeInlineComment(line);
        std::string trimmed = Utils::trim(noComment);

        if (trimmed.empty()) continue;

        // Check for wfn=xx format at the beginning of file
        if (trimmed.find("wfn=") == 0 && currentTask.moduleName.empty()) {
            wfnFile = Utils::trim(trimmed.substr(4));
            continue;
        }
        
        // Check for core=xx format at the beginning of file
        if (trimmed.find("core=") == 0 && currentTask.moduleName.empty()) {
            std::string coreStr = Utils::trim(trimmed.substr(5));
            cores = std::atoi(coreStr.c_str());
            continue;
        }
        
        // Check for key=value format at the beginning of file (custom variables)
        // This should come before module definitions, so check if no module is active
        if (currentTask.moduleName.empty() && !inProcessMode && !inCommandMode) {
            size_t eqPos = trimmed.find('=');
            // Only treat as variable if:
            // 1. Contains exactly one '='
            // 2. Key is not empty
            // 3. Key contains only alphanumeric and underscore (valid variable name)
            // 4. Not a special keyword (wfn, core)
            if (eqPos != std::string::npos && eqPos > 0 && eqPos < trimmed.length() - 1) {
                std::string key = Utils::trim(trimmed.substr(0, eqPos));
                std::string value = Utils::trim(trimmed.substr(eqPos + 1));
                
                // Validate key name (alphanumeric and underscore only)
                bool validKey = true;
                for (char c : key) {
                    if (!isalnum(c) && c != '_') {
                        validKey = false;
                        break;
                    }
                }
                
                // Only accept if key is valid and not a special keyword
                if (validKey && !key.empty() && key != "wfn" && key != "core") {
                    customVars[key] = value;
                    continue;
                }
            }
        }
        
        // Module start [module_name] 必须顶格
        if (!noComment.empty() && noComment[0] == '[') {
            size_t lastNonWS = noComment.find_last_not_of(" \t\r\n");
            if (lastNonWS != std::string::npos && noComment[lastNonWS] == ']') {
                // If there is an unfinished task, save it
                if (!currentTask.moduleName.empty() || !currentTask.commands.empty()) {
                    tasks.push_back(currentTask);
                }
                currentTask = ModuleTask();
                std::string inside = noComment.substr(1, lastNonWS - 1);
                currentTask.moduleName = Utils::trim(inside);
                currentTask.useWait = false;
                currentTask.blockIndex = moduleBlockCounters[currentTask.moduleName]++;
                inProcessMode = false;
                inCommandMode = false;
                continue;
            }
        }
        
        // Enter post-processing mode
        if (trimmed == "%process") {
            if (currentTask.moduleName.empty()) {
                std::cerr << "Warning: %process without module definition" << std::endl;
                continue;
            }
            inProcessMode = true;
            inCommandMode = false;
            continue;
        }
        
        // Enter command mode
        if (trimmed == "%command") {
            // Allow top-level %command without module definition ("裸command")
            if (currentTask.moduleName.empty()) {
                currentTask = ModuleTask();
                currentTask.useWait = false;
                currentTask.blockIndex = moduleBlockCounters[currentTask.moduleName]++;
            }
            inCommandMode = true;
            inProcessMode = false;
            continue;
        }
        
        // End current module with "end"
        if (trimmed == "end") {
            if (!currentTask.moduleName.empty() || !currentTask.commands.empty()) {
                tasks.push_back(currentTask);
                currentTask = ModuleTask();
            }
            inProcessMode = false;
            inCommandMode = false;
            continue;
        }
        
        // End current module with "wait" (interactive mode)
        if (trimmed == "wait") {
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
            currentTask.commands.push_back(trimmed);
        } else {
            std::vector<std::string> tokens = split(trimmed, ' ');
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
    
    // Save the last task (module or command-only)
    if (!currentTask.moduleName.empty() || !currentTask.commands.empty()) {
        tasks.push_back(currentTask);
    }
    
    file.close();
    return {tasks, wfnFile, cores, customVars};
}

// Parse inp file, return all module tasks, optional wfn file, and core count (backward compatibility)
std::tuple<std::vector<ModuleTask>, std::string, int> InputParser::parseInpFileWithWfnAndCores(const std::string& inpFile) {
    auto result = parseInpFileWithWfnAndCoresAndVars(inpFile);
    return {std::get<0>(result), std::get<1>(result), std::get<2>(result)};
}

// Parse inp file, return all module tasks and optional wfn file (backward compatibility)
std::pair<std::vector<ModuleTask>, std::string> InputParser::parseInpFileWithWfn(const std::string& inpFile) {
    auto result = parseInpFileWithWfnAndCores(inpFile);
    return {std::get<0>(result), std::get<1>(result)};
}

// Parse inp file, return all module tasks (backward compatibility)
std::vector<ModuleTask> InputParser::parseInpFile(const std::string& inpFile) {
    auto result = parseInpFileWithWfn(inpFile);
    return result.first;
}
