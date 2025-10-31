#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include "config.h"
#include "input.h"
#include "ui.h"
#include "utils.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// Utility function: split string (deprecated - use Utils::split instead)
std::vector<std::string> split(const std::string& str, char delimiter) {
    return Utils::split(str, delimiter);
}

class MultiwfnScriptGenerator {
private:
    ConfigManager configManager;
    
public:
    // Load banewfn.rc configuration file
    bool loadBaneWfnConfig(const std::string& configFile) {
        return configManager.loadBaneWfnConfig(configFile);
    }
    
    // Load module-specific conf file
    bool loadModuleConfig(const std::string& moduleName) {
        return configManager.loadModuleConfig(moduleName);
    }
    
    // Generate command sequence
    std::vector<std::string> generateCommands(const std::string& moduleName,
                                             const std::string& sectionName,
                                             const std::map<std::string, std::string>& params) {
        std::vector<std::string> result;
        
        if (!configManager.hasModuleConfig(moduleName)) {
            std::cerr << "Warning: Module config [" << moduleName << "] not loaded" << std::endl;
            return result;
        }
        
        const ModuleConfig& modConfig = configManager.getModuleConfig(moduleName);
        
        if (modConfig.sections.find(sectionName) == modConfig.sections.end()) {
            std::cerr << "Warning: Section [" << sectionName << "] not found in module " << moduleName << std::endl;
            return result;
        }
        
        const Section& section = modConfig.sections.at(sectionName);
        
        // Merge default values and provided parameters
        // Only override defaults if provided value is non-empty, to mimic ${var:-default}
        std::map<std::string, std::string> finalParams = section.defaults;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                finalParams[param.first] = param.second;
            }
        }
        
        // Generate commands
        for (const auto& cmd : section.commands) {
            result.push_back(replacePlaceholders(cmd, finalParams));
        }
        
        return result;
    }
    
    // Parse inp file, return all module tasks
    std::vector<ModuleTask> parseInpFile(const std::string& inpFile) {
        return InputParser::parseInpFile(inpFile);
    }
    
    // Parse inp file, return all module tasks and optional wfn file
    std::pair<std::vector<ModuleTask>, std::string> parseInpFileWithWfn(const std::string& inpFile) {
        return InputParser::parseInpFileWithWfn(inpFile);
    }
    
    // Parse inp file, return all module tasks, optional wfn file, and core count
    std::tuple<std::vector<ModuleTask>, std::string, int> parseInpFileWithWfnAndCores(const std::string& inpFile) {
        return InputParser::parseInpFileWithWfnAndCores(inpFile);
    }
    
    // Generate command script for a single module
    std::string generateModuleScript(const ModuleTask& task, bool includeQuit) {
        std::stringstream output;
        
        if (!configManager.hasModuleConfig(task.moduleName)) {
            std::cerr << "Error: Module config not loaded for " << task.moduleName << std::endl;
            return "";
        }
        
        const ModuleConfig& modConfig = configManager.getModuleConfig(task.moduleName);
        
        // Generate main module commands (pre-processing)
        auto commands = generateCommands(task.moduleName, "main", task.params);
        for (const auto& cmd : commands) {
            output << cmd << "\n";
        }
        
        // Generate post-processing commands
        for (const auto& step : task.postProcessSteps) {
            auto stepCommands = generateCommands(task.moduleName, step.first, step.second);
            for (const auto& cmd : stepCommands) {
                output << cmd << "\n";
            }
        }
        
        // Add quit commands only if requested
        if (includeQuit) {
            for (const auto& quitCmd : modConfig.quitCommands) {
                output << quitCmd << "\n";
            }
        }
        
        return output.str();
    }
    
    // Execute single module Multiwfn task (file-based mode)
    bool executeModuleTaskFile(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\n>>> Processing module: " << task.moduleName << std::endl;
        
        // Generate command script with quit commands
        std::string commands = generateModuleScript(task, true);
        if (commands.empty()) {
            return false;
        }
        
        // Create command file
        std::string wfnBaseName = getBaseName(wfnFile);
        std::string cmdFileName = task.moduleName + "_" + wfnBaseName;
        if (task.blockIndex > 0) {
            cmdFileName += "_" + std::to_string(task.blockIndex);
        }
        cmdFileName += ".txt";
        
        std::ofstream cmdFile(cmdFileName);
        if (!cmdFile.is_open()) {
            std::cerr << "Error: Cannot create command file: " << cmdFileName << std::endl;
            return false;
        }
        cmdFile << commands;
        cmdFile.close();
        
        // In dryrun mode, only generate the file
        if (options.dryrun) {
            std::cout << "Dry-run mode: Command file generated, skipping execution." << std::endl;
            return true;
        }
        
        // Generate output filename or screen output
        std::stringstream cmd;
        cmd << configManager.getConfig().multiwfnExec << " " << wfnFile << " < " << cmdFileName;
        
        if (!options.screen) {
            std::string outFile = task.moduleName + "_" + wfnBaseName;
            if (task.blockIndex > 0) {
                outFile += "_" + std::to_string(task.blockIndex);
            }
            outFile += ".out";
            cmd << " > " << outFile;
        }
        
        if (cores > 0) {
            cmd << " -np " << cores;
        }
        
        std::cout << "Executing command: " << cmd.str() << std::endl;
        std::cout << "Starting Multiwfn process..." << std::endl;
        
        // Execute command
        int result = system(cmd.str().c_str());
        
        // Clean up command file only if not in dryrun mode
        if (!options.dryrun) {
            remove(cmdFileName.c_str());
        }
        
        if (result == 0) {
            std::cout << "Module " << task.moduleName << " execution completed." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Module " << task.moduleName 
                     << " execution failed with error code " << result << std::endl;
            return false;
        }
    }
    
    // Execute single module Multiwfn task (pipe/interactive mode)
    bool executeModuleTaskPipe(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\nProcessing module: " << task.moduleName << " (interactive mode)" << std::endl;
        
        // In dryrun mode, skip wait tasks
        if (options.dryrun) {
            std::cout << "Dry-run mode: Skipping interactive task." << std::endl;
            return true;
        }
        
        // Generate command script without quit commands
        std::string commands = generateModuleScript(task, false);
        if (commands.empty()) {
            return false;
        }
        
        // Parse commands into individual lines (preserve empty lines!)
        std::vector<std::string> cmdLines;
        std::stringstream ss(commands);
        std::string line;
        while (std::getline(ss, line)) {
            // Don't trim or skip empty lines - they are meaningful inputs
            cmdLines.push_back(line);
        }
        
        // Build pipe command: cross-platform compatible
        std::stringstream cmd;
        
#ifdef PLATFORM_WINDOWS
        // Windows style: cmd /c "(echo cmd1; echo cmd2; ...; type con) | Multiwfn file"
        cmd << "cmd /c \"(";
        
        for (const auto& cmdLine : cmdLines) {
            std::string trimmedLine = trim(cmdLine);
            if (trimmedLine.empty()) {
                // For empty lines, use echo. to produce empty input
                cmd << "echo. & ";
            } else {
                // For non-empty lines, escape quotes and use echo
                std::string escaped = cmdLine;
                // Escape any existing quotes in the command for Windows
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                cmd << "echo " << escaped << " & ";
            }
        }
        
        cmd << "type con) | " << configManager.getConfig().multiwfnExec << " " << wfnFile << "\"";
#else
        // Linux style: (echo cmd1; echo cmd2; ...; cat) | Multiwfn file
        cmd << "(";
        
        for (const auto& cmdLine : cmdLines) {
            std::string trimmedLine = trim(cmdLine);
            if (trimmedLine.empty()) {
                // For empty lines, use echo without arguments to produce empty input
                cmd << "echo; ";
            } else {
                // For non-empty lines, escape quotes and use echo
                std::string escaped = cmdLine;
                // Escape any existing quotes in the command
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                cmd << "echo \"" << escaped << "\"; ";
            }
        }
        
        cmd << "cat) | " << configManager.getConfig().multiwfnExec << " " << wfnFile;
#endif
        
        if (cores > 0) {
            cmd << " -np " << cores;
        }
        
        std::cout << "Executing command: " << cmd.str() << std::endl;
        std::cout << "Starting Multiwfn in interactive mode...\n" << std::endl;
        
        // Execute command
        int result = system(cmd.str().c_str());
        
        if (result == 0) {
            std::cout << "\nModule " << task.moduleName << " session ended." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Module " << task.moduleName 
                     << " execution failed with error code " << result << std::endl;
            return false;
        }
    }
    
    // Execute command block (shell commands)
    bool executeCommandBlock(const ModuleTask& task, const ExecutionOptions& options) {
        if (task.commands.empty()) {
            return true; // No commands to execute
        }
        
        std::cout << "\nExecuting command block for module: " << task.moduleName << std::endl;
        
        // In dryrun mode, only show what would be executed
        if (options.dryrun) {
            std::cout << "Dry-run mode: Would execute the following commands:" << std::endl;
            for (const auto& cmd : task.commands) {
                std::cout << "  " << cmd << std::endl;
            }
            return true;
        }
        
        // Create temporary script file
        std::string scriptFileName = task.moduleName + "_commands";
        if (task.blockIndex > 0) {
            scriptFileName += "_" + std::to_string(task.blockIndex);
        }
        
#ifdef PLATFORM_WINDOWS
        scriptFileName += ".bat";
        std::ofstream scriptFile(scriptFileName);
        if (!scriptFile.is_open()) {
            std::cerr << "Error: Cannot create batch file: " << scriptFileName << std::endl;
            return false;
        }
        
        // Write batch commands
        for (const auto& cmd : task.commands) {
            scriptFile << cmd << std::endl;
        }
        scriptFile.close();
        
        // Execute batch file
        std::stringstream cmd;
        cmd << "cmd /c \"" << scriptFileName << "\"";
        std::cout << "Running script: " << cmd.str() << " ..." << std::endl;
        
        int result = system(cmd.str().c_str());
        
        // Clean up batch file
        remove(scriptFileName.c_str());
        
#else
        scriptFileName += ".sh";
        std::ofstream scriptFile(scriptFileName);
        if (!scriptFile.is_open()) {
            std::cerr << "Error: Cannot create shell script: " << scriptFileName << std::endl;
            return false;
        }
        
        // Write shell script header
        scriptFile << "#!/bin/bash" << std::endl;
        // scriptFile << "set -e" << std::endl; // Exit on error is not necessary
        
        // Write shell commands
        for (const auto& cmd : task.commands) {
            scriptFile << cmd << std::endl;
        }
        scriptFile.close();
        
        // Make script executable
        chmod(scriptFileName.c_str(), 0755);
        
        // Execute shell script
        std::stringstream cmd;
        cmd << "./" << scriptFileName;
        std::cout << "Running script: " << cmd.str() << " ..." << std::endl;
        
        int result = system(cmd.str().c_str());
        
        // Clean up shell script
        remove(scriptFileName.c_str());  // Comment out this line for debugging
#endif
        
        if (result == 0) {
            std::cout << "Command block execution completed." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Command block execution failed with error code " << result << std::endl;
            return false;
        }
    }
    
    // Execute single module task (dispatch to appropriate method)
    bool executeModuleTask(const ModuleTask& task, const std::string& wfnFile, 
                          int cores, const ExecutionOptions& options) {
        bool success = false;
        
        // Support command-only task (no module, only %command block)
        if (task.moduleName.empty()) {
            return executeCommandBlock(task, options);
        }

        if (task.useWait) {
            success = executeModuleTaskPipe(task, wfnFile, cores, options);
        } else {
            success = executeModuleTaskFile(task, wfnFile, cores, options);
        }
        
        // Execute command block if module execution was successful
        if (success) {
            success = executeCommandBlock(task, options);
        }
        
        return success;
    }
    
    // Execute all module tasks
    bool executeAllTasks(const std::string& inpFile, const std::string& wfnFile, 
                        int cores, const ExecutionOptions& options) {
        // Parse inp file, get all tasks, optional wfn file, and core count
        auto parseResult = parseInpFileWithWfnAndCores(inpFile);
        std::vector<ModuleTask> tasks = std::get<0>(parseResult);
        std::string inputWfnFile = std::get<1>(parseResult);
        int inputCores = std::get<2>(parseResult);
        
        // Use wfn file from input file if specified, otherwise use command line argument
        std::string finalWfnFile = inputWfnFile.empty() ? wfnFile : inputWfnFile;
        
        // Use core count from input file if specified and no cores provided via command line
        int finalCores = cores;
        if (cores < 0 && inputCores > 0) {
            finalCores = inputCores;
            std::cout << "Using core count from input file: " << finalCores << std::endl;
        }
        
        // Apply placeholder replacement using the final wavefunction filename
        InputParser::applyPlaceholderReplacement(tasks, finalWfnFile);
        
        if (tasks.empty()) {
            std::cerr << "Error: No modules found in inp file" << std::endl;
            return false;
        }
        
        // Collect all required modules and load configurations
        std::set<std::string> modules;
        for (const auto& task : tasks) {
            if (!task.moduleName.empty()) {
                modules.insert(task.moduleName);
            }
        }
        
        std::cout << "\nRequired modules: ";
        for (const auto& mod : modules) {
            std::cout << mod << " ";
        }
        std::cout << "\n";
        
        if (options.dryrun) {
            std::cout << "\n** DRY-RUN MODE: Only generating command files **\n" << std::endl;
        }
        if (options.screen) {
            std::cout << "\n** SCREEN MODE: Output to screen instead of files **\n" << std::endl;
        }
        
        for (const auto& mod : modules) {
            if (!loadModuleConfig(mod)) {
                std::cerr << "Error: Failed to load module config for " << mod << std::endl;
                return false;
            }
        }
        
        // Execute each module task in sequence
        bool allSuccess = true;
        for (const auto& task : tasks) {
            if (!executeModuleTask(task, finalWfnFile, finalCores, options)) {
                allSuccess = false;
            }
        }
        
        if (allSuccess) {
            std::cout << "\nAll done." << std::endl;
        } else {
            std::cerr << "\nSome modules execution failed" << std::endl;
        }
        
        return allSuccess;
    }
    
    int getCores() const { return configManager.getCores(); }
};

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <input.inp> <molecule.fchk> [options]\n";
    std::cout << "       " << progName << " <input.inp> -w <molecule.fchk> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -c, --cores <num>   Specify the number of CPU cores to use\n";
    std::cout << "  -d, --dryrun        Generate command files only, don't execute (skip wait tasks)\n";
    std::cout << "  -s, --screen        Display output on screen instead of redirecting to files\n";
    std::cout << "  -w, --wfn <file>    Specify wavefunction file (.fchk/.wfn or other supported file)\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << progName << " input.inp molecule.fchk\n";
    std::cout << "  " << progName << " input.inp -w molecule.fchk\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -c 8\n";
    std::cout << "  " << progName << " input.inp -w molecule.fchk --dryrun\n";
    std::cout << "  " << progName << " input.inp molecule.fchk --screen\n";
    std::cout << "  " << progName << " input.inp -w molecule.fchk -d -s -c 8\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Ensure Windows console uses UTF-8 for input/output to avoid garbled ASCII art
    // and other UTF-8 text when double-click launching.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    // Print ASCII logo on startup
    UI::printLogo();
    
    std::string inpFile;
    std::string wfnFile;
    std::string wfnParam;  // Store wfn parameter from -w/--wfn
    int cores = -1;
    ExecutionOptions options;
    
    // Parse command line arguments
    std::vector<std::string> positionalArgs;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-c" || arg == "--cores") {
            if (i + 1 < argc) {
                cores = std::atoi(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Error: -c/--cores requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-d" || arg == "--dryrun") {
            options.dryrun = true;
        } else if (arg == "-s" || arg == "--screen") {
            options.screen = true;
        } else if (arg == "-w" || arg == "--wfn") {
            if (i + 1 < argc) {
                wfnParam = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -w/--wfn requires an argument" << std::endl;
                return 1;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Warning: Unknown option: " << arg << std::endl;
        } else {
            // This is a positional argument
            positionalArgs.push_back(arg);
        }
    }
    
    // Handle positional arguments
    if (positionalArgs.size() >= 1) {
        inpFile = positionalArgs[0];
    } else {
        inpFile = UI::requestInputFile();
    }
    
    // Check if input file contains wfn definition and core setting
    auto parseResult = InputParser::parseInpFileWithWfnAndCores(inpFile);
    std::string inputWfnFile = std::get<1>(parseResult);
    int inputCores = std::get<2>(parseResult);
    
    // Priority order: 1) -w/--wfn parameter, 2) positional argument, 3) input file, 4) interactive input
    if (!wfnParam.empty()) {
        wfnFile = wfnParam;
        std::cout << "Using wavefunction file from -w/--wfn parameter: " << wfnFile << std::endl;
    } else if (positionalArgs.size() >= 2) {
        wfnFile = positionalArgs[1];
    } else if (inputWfnFile.empty()) {
        // Only request wfn file if not defined in input file
        wfnFile = UI::requestWavefunctionFile();
    } else {
        // Use wfn file from input file
        wfnFile = inputWfnFile;
        std::cout << "Using wavefunction file from input: " << wfnFile << std::endl;
    }
    
    MultiwfnScriptGenerator generator;
    
    // Search for banewfn.rc
    std::string configFile = findConfigFile(argv[0]);
    
    if (configFile.empty()) {
        std::cerr << "Error: Could not find banewfn.rc in any of the search locations" << std::endl;
        std::cerr << "Please create the config file in one of the following locations:" << std::endl;
        std::cerr << "  - Current directory: ./banewfn.rc" << std::endl;
        std::cerr << "  - Executable directory: <exe_dir>/banewfn.rc" << std::endl;
        std::cerr << "  - Home directory: ~/.bane/wfn/banewfn.rc" << std::endl;
        return 1;
    }
    
    // Load banewfn.rc
    if (!generator.loadBaneWfnConfig(configFile)) {
        return 1;
    }
    
    // If cores not specified, use input file setting or default value from banewfn.rc
    if (cores < 0) {
        if (inputCores > 0) {
            cores = inputCores;
            std::cout << "Using core count from input file: " << cores << std::endl;
        } else {
            cores = generator.getCores();
        }
    }
    
    // Execute all module tasks
    if (!generator.executeAllTasks(inpFile, wfnFile, cores, options)) {
        return 1;
    }
    
    return 0;
}