#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include "config.h"
#include "input.h"
#include "inline_conf.h"
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
#include <io.h>
#endif

// Utility function: split string (deprecated - use Utils::split instead)
std::vector<std::string> split(const std::string& str, char delimiter) {
    return Utils::split(str, delimiter);
}

static bool isPlainCustomVarName(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    for (char c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    return true;
}

static bool isValidCustomVarName(const std::string& name) {
    if (isPlainCustomVarName(name)) {
        return true;
    }

    if (name.size() > 1 && name.back() == '*') {
        return isPlainCustomVarName(name.substr(0, name.size() - 1));
    }

    return name.size() > 5 && name.rfind("len(", 0) == 0 && name.back() == ')' &&
           isPlainCustomVarName(name.substr(4, name.size() - 5));
}

namespace {

struct TerminalColorState {
    bool enabled = false;
};

TerminalColorState& terminalColorState() {
    static TerminalColorState state;
    return state;
}

bool isNonEmptyEnvEnabled(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && value[0] != '\0';
}

bool isStdStreamTerminal(FILE* stream) {
#ifdef _WIN32
    return _isatty(_fileno(stream)) != 0;
#else
    return isatty(fileno(stream)) != 0;
#endif
}

#ifdef _WIN32
void enableVirtualTerminalForHandle(DWORD stdHandleId) {
    HANDLE handle = GetStdHandle(stdHandleId);
    if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
        return;
    }

    DWORD mode = 0;
    if (!GetConsoleMode(handle, &mode)) {
        return;
    }

    SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

void initializeColorOutput(const ExecutionOptions& options) {
    bool enabled = !options.noColor && !isNonEmptyEnvEnabled("NO_COLOR");

    // Disable colors whenever output is redirected, so logs/files stay clean.
    if (enabled) {
        enabled = isStdStreamTerminal(stdout) && isStdStreamTerminal(stderr);
    }

#ifdef _WIN32
    if (enabled) {
        enableVirtualTerminalForHandle(STD_OUTPUT_HANDLE);
        enableVirtualTerminalForHandle(STD_ERROR_HANDLE);
    }
#endif

    terminalColorState().enabled = enabled;
}

std::string colorizeText(const std::string& text, const char* ansiCode) {
    if (!terminalColorState().enabled) {
        return text;
    }
    return std::string(ansiCode) + text + "\033[0m";
}

void printSuccessLine(const std::string& text) {
    std::cout << colorizeText(text, "\033[32m") << std::endl;
}

void printFailureLine(const std::string& text) {
    std::cerr << colorizeText(text, "\033[31m") << std::endl;
}

} // namespace

// Replace only the specified placeholder variable in a string.
// Supported forms: $var, ${var}, ${var:-default}
// Any other placeholders are left untouched.
static std::string replaceOnePlaceholderVar(const std::string& text,
                                           const std::string& var,
                                           const std::string& value) {
    std::string result = text;
    size_t pos = 0;

    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        std::string varName;
        std::string defaultValue;

        if (endPos < result.size() && result[endPos] == '{') {
            size_t braceStart = endPos + 1;
            size_t braceEnd = result.find('}', braceStart);
            if (braceEnd == std::string::npos) {
                // Malformed placeholder, skip
                pos = endPos + 1;
                continue;
            }

            std::string inside = result.substr(braceStart, braceEnd - braceStart);
            size_t defaultSep = inside.find(":-");
            if (defaultSep != std::string::npos) {
                varName = inside.substr(0, defaultSep);
                defaultValue = inside.substr(defaultSep + 2);
            } else {
                varName = inside;
            }
            endPos = braceEnd + 1;
        } else {
            while (endPos < result.size() && (isalnum((unsigned char)result[endPos]) || result[endPos] == '_')) {
                endPos++;
            }
            varName = result.substr(pos + 1, endPos - pos - 1);
        }

        if (varName == var) {
            std::string repl = value;
            if (repl.empty() && !defaultValue.empty()) {
                repl = defaultValue;
            }
            result.replace(pos, endPos - pos, repl);
            pos += repl.size();
        } else {
            pos = endPos;
        }
    }

    return result;
}

static std::string buildMultiwfnInvocation(const std::string& multiwfnExec,
                                           const std::string& wfnFile,
                                           int cores,
                                           const ExecutionOptions& options) {
    std::stringstream cmd;
    cmd << multiwfnExec << " " << wfnFile;

    if (cores > 0) {
        cmd << " -nt " << cores;
    }

    if (options.nogui) {
        cmd << " -silent";
    }

    for (const auto& arg : options.extargs) {
        cmd << " " << arg;
    }

    return cmd.str();
}

static void pauseIfWindowsDryRun(bool shouldPause) {
#ifdef _WIN32
    if (shouldPause) {
        std::cout << std::flush;
        system("pause");
    }
#else
    (void)shouldPause;
#endif
}

static bool taskHasMultiwfnScript(const ModuleTask& task) {
    return !task.moduleName.empty() || !task.rawCommands.empty();
}

static std::string getTaskDisplayName(const ModuleTask& task) {
    if (!task.moduleName.empty()) {
        return task.moduleName;
    }
    if (!task.rawCommands.empty()) {
        return "%raw";
    }
    if (!task.commands.empty()) {
        return "%command";
    }
    return "<empty>";
}

static std::string getMultiwfnFileStem(const ModuleTask& task, const std::string& wfnFile) {
    std::string stem = task.moduleName.empty() ? "raw" : task.moduleName;

    std::string wfnBaseName = getBaseName(wfnFile);
    if (!wfnBaseName.empty()) {
        stem += "_" + wfnBaseName;
    }

    if (task.blockIndex > 0) {
        stem += "_" + std::to_string(task.blockIndex);
    }

    return stem;
}

static std::string getCommandScriptStem(const ModuleTask& task) {
    std::string stem;

    if (!task.moduleName.empty()) {
        stem = task.moduleName + "_commands";
    } else if (!task.rawCommands.empty()) {
        stem = "raw_commands";
    } else {
        stem = "commands";
    }

    if (task.blockIndex > 0) {
        stem += "_" + std::to_string(task.blockIndex);
    }

    return stem;
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
            auto expanded = replacePlaceholdersExpanded(cmd, finalParams);
            result.insert(result.end(), expanded.begin(), expanded.end());
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
    
    // Generate Multiwfn input script for a single task.
    // For normal module tasks, this includes [main] + %process sections + optional %raw.
    // For raw-only tasks, this is simply the literal %raw content.
    std::string generateModuleScript(const ModuleTask& task, bool includeQuit) {
        std::stringstream output;

        bool hasContent = false;
        const ModuleConfig* modConfig = nullptr;

        if (!task.moduleName.empty()) {
            if (!configManager.hasModuleConfig(task.moduleName)) {
                std::cerr << "Error: Module config not loaded for " << task.moduleName << std::endl;
                return "";
            }

            modConfig = &configManager.getModuleConfig(task.moduleName);

            // Generate main module commands (pre-processing)
            auto commands = generateCommands(task.moduleName, "main", task.params);
            for (const auto& cmd : commands) {
                output << cmd << "\n";
                hasContent = true;
            }

            // Generate post-processing commands
            for (const auto& step : task.postProcessSteps) {
                auto stepCommands = generateCommands(task.moduleName, step.first, step.second);
                for (const auto& cmd : stepCommands) {
                    output << cmd << "\n";
                    hasContent = true;
                }
            }
        }

        // Append literal raw Multiwfn commands after generated module commands.
        for (const auto& rawCmd : task.rawCommands) {
            output << rawCmd << "\n";
            hasContent = true;
        }

        if (!hasContent) {
            return "";
        }

        // Add module-specific quit commands only if requested.
        // Raw-only tasks are treated as already-finalized literal command sequences.
        if (includeQuit && modConfig != nullptr) {
            for (const auto& quitCmd : modConfig->quitCommands) {
                output << quitCmd << "\n";
            }
        }
        
        return output.str();
    }
    
    // Execute single module Multiwfn task (file-based mode)
    bool executeModuleTaskFile(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\n>>> Processing Multiwfn task: " << getTaskDisplayName(task) << std::endl;
        
        // Generate command script with quit commands
        std::string commands = generateModuleScript(task, true);
        if (commands.empty()) {
            return false;
        }
        
        // Create command file
        std::string cmdFileName = getMultiwfnFileStem(task, wfnFile) + ".txt";
        
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
        std::string outFile;
        if (!options.screen) {
            outFile = getMultiwfnFileStem(task, wfnFile) + ".out";
            
            std::ofstream outFileStream(outFile);
            if (outFileStream.is_open()) {
                outFileStream << UI::getLogoString();
                outFileStream.close();
            }
        }
        
        std::stringstream cmd;
        cmd << buildMultiwfnInvocation(configManager.getConfig().multiwfnExec, wfnFile, cores, options)
            << " < " << cmdFileName;

        if (!options.screen) {
            cmd << " >> " << outFile;
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
            printSuccessLine("Module " + task.moduleName + " execution completed.");
            return true;
        } else {
            printFailureLine("Error: Module " + task.moduleName +
                             " execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    // Execute single module Multiwfn task (pipe/interactive mode)
    bool executeModuleTaskPipe(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\nProcessing Multiwfn task: " << getTaskDisplayName(task)
                  << " (interactive mode)" << std::endl;

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

        std::string multiwfnCmd = buildMultiwfnInvocation(
            configManager.getConfig().multiwfnExec, wfnFile, cores, options);

        // In dryrun mode, show the manual Multiwfn invocation for wait blocks.
        if (options.dryrun) {
            std::cout << "Dry-run mode: wait block will not be executed automatically." << std::endl;
            std::cout << "Please launch Multiwfn with:" << std::endl;
            std::cout << "  " << multiwfnCmd << std::endl;
            if (!cmdLines.empty()) {
                std::cout << "Then enter the following inputs manually:" << std::endl;
                for (const auto& cmdLine : cmdLines) {
                    if (cmdLine.empty()) {
                        std::cout << "  [empty line]" << std::endl;
                    } else {
                        std::cout << "  " << cmdLine << std::endl;
                    }
                }
            }
            return true;
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
        
        cmd << "type con) | " << multiwfnCmd << "\"";
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
        
        cmd << "cat) | " << multiwfnCmd;
#endif

        std::cout << "Executing command: " << cmd.str() << std::endl;
        std::cout << "Starting Multiwfn in interactive mode...\n" << std::endl;
        
        // Execute command
        int result = system(cmd.str().c_str());
        
        if (result == 0) {
            printSuccessLine("Module " + task.moduleName + " session ended.");
            return true;
        } else {
            printFailureLine("Error: Module " + task.moduleName +
                             " execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    // Execute command block (shell commands)
    // outFile: the Multiwfn output file produced by the preceding module task.
    //          Computed centrally in executeModuleTask and passed in explicitly
    //          so that ${output} is always resolved correctly.
    bool executeCommandBlock(const ModuleTask& task, const std::string& wfnFile,
                             const ExecutionOptions& options,
                             const std::string& outFile = "") {
        if (task.commands.empty()) {
            return true; // No commands to execute
        }
        
        (void)wfnFile;

        std::cout << "\nExecuting command block for task: " << getTaskDisplayName(task) << std::endl;

        // Replace reserved placeholders in command lines (currently: ${output})
        std::vector<std::string> processedCmds;
        processedCmds.reserve(task.commands.size());
        for (const auto& cmdLine : task.commands) {
            processedCmds.push_back(replaceOnePlaceholderVar(cmdLine, "output", outFile));
        }

        // In dryrun mode, only show what would be executed (after placeholder replacement)
        if (options.dryrun) {
            std::cout << "Dry-run mode: Would execute the following commands:" << std::endl;
            for (const auto& cmd : processedCmds) {
                std::cout << "  " << cmd << std::endl;
            }
            return true;
        }
        
        // Create temporary script file
        std::string scriptFileName = getCommandScriptStem(task);
        
#ifdef PLATFORM_WINDOWS
        int result = 0;
        
        // Check if gitbash_exec is configured and first line is shebang #!/bin/bash
        bool useGitBash = false;
        if (!configManager.getConfig().gitbashExec.empty() && !processedCmds.empty()) {
            std::string firstLine = processedCmds[0];
            // Trim whitespace to check for shebang
            std::string trimmedFirstLine = Utils::trim(firstLine);
            if (trimmedFirstLine == "#!/bin/bash" || trimmedFirstLine.find("#!/bin/bash") == 0) {
                useGitBash = true;
            }
        }
        
        if (useGitBash) {
            // Use Git Bash to execute shell script
            scriptFileName += ".sh";
            std::ofstream scriptFile(scriptFileName);
            if (!scriptFile.is_open()) {
                std::cerr << "Error: Cannot create shell script: " << scriptFileName << std::endl;
                return false;
            }
            
            // Write shell commands (including shebang if present)
            for (const auto& cmd : processedCmds) {
                scriptFile << cmd << std::endl;
            }
            scriptFile.close();
            
            // Execute using Git Bash with -c parameter
            // Convert Windows path to absolute path for bash
            char absPath[MAX_PATH];
            DWORD pathLen = GetFullPathNameA(scriptFileName.c_str(), MAX_PATH, absPath, nullptr);
            std::string bashPath;
            if (pathLen > 0 && pathLen < MAX_PATH) {
                bashPath = absPath;
            } else {
                bashPath = scriptFileName;
            }
            // Convert Windows path separators to forward slashes for bash
            size_t pos = 0;
            while ((pos = bashPath.find('\\', pos)) != std::string::npos) {
                bashPath.replace(pos, 1, "/");
                pos += 1;
            }
            
            // Build command string for display
            std::stringstream cmdDisplay;
            cmdDisplay << "\"" << configManager.getConfig().gitbashExec << "\" -c \"bash " << bashPath << "\"";
            std::cout << "Running script with Git Bash: " << cmdDisplay.str() << " ..." << std::endl;
            
            // Use CreateProcess to avoid cmd.exe quote parsing issues
            std::string gitbashPath = configManager.getConfig().gitbashExec;
            std::string commandArg = "bash " + bashPath;
            
            // Prepare command line for CreateProcess
            // When lpApplicationName is specified, lpCommandLine should start with the program name
            // Format: "program_path" -c "bash script_path"
            std::string cmdLine = "\"" + gitbashPath + "\" -c \"" + commandArg + "\"";
            
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            memset(&si, 0, sizeof(si));
            memset(&pi, 0, sizeof(pi));
            si.cb = sizeof(si);
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            
            // CreateProcess requires mutable string for lpCommandLine
            std::vector<char> cmdLineBuf(cmdLine.begin(), cmdLine.end());
            cmdLineBuf.push_back('\0');
            
            BOOL success = CreateProcessA(
                nullptr,
                cmdLineBuf.data(),
                nullptr,
                nullptr,
                TRUE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi
            );
            
            if (success) {
                // Wait for process to complete
                WaitForSingleObject(pi.hProcess, INFINITE);
                DWORD exitCode = 0;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                result = static_cast<int>(exitCode);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            } else {
                std::cerr << "Error: Failed to start Git Bash process. Error code: " << GetLastError() << std::endl;
                result = 1;
            }
            
            // Clean up shell script
            remove(scriptFileName.c_str());
        } else {
            // Use default batch script
            scriptFileName += ".bat";
            std::ofstream scriptFile(scriptFileName);
            if (!scriptFile.is_open()) {
                std::cerr << "Error: Cannot create batch file: " << scriptFileName << std::endl;
                return false;
            }
            
            // Write batch commands
            for (const auto& cmd : processedCmds) {
                scriptFile << cmd << std::endl;
            }
            scriptFile.close();
            
            // Execute batch file
            std::stringstream cmd;
            cmd << "cmd /c \"" << scriptFileName << "\"";
            std::cout << "Running script: " << cmd.str() << " ..." << std::endl;
            
            result = system(cmd.str().c_str());
            
            // Clean up batch file
            remove(scriptFileName.c_str());
        }
        
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
        for (const auto& cmd : processedCmds) {
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
        remove(scriptFileName.c_str());
#endif
        
        if (result == 0) {
            printSuccessLine("Command block execution completed.");
            return true;
        } else {
            printFailureLine("Error: Command block execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    // Execute single module task (dispatch to appropriate method)
    bool executeModuleTask(const ModuleTask& task, const std::string& wfnFile, 
                          int cores, const ExecutionOptions& options) {
        bool success = false;

        // Support command-only task (no Multiwfn task, only %command block)
        if (!taskHasMultiwfnScript(task)) {
            return executeCommandBlock(task, wfnFile, options, /*outFile=*/"");
        }

        // Compute outFile centrally so that ${output} in %command blocks is
        // always resolved to the same path that executeModuleTaskFile writes.
        // Rules mirror executeModuleTaskFile exactly:
        //   - screen mode  → no output file, outFile = ""
        //   - wait mode    → no output file, outFile = ""
        //   - otherwise    → "<task>_<wfnBase>[_<blockIndex>].out"
        std::string outFile;
        if (!options.screen && !task.useWait) {
            outFile = getMultiwfnFileStem(task, wfnFile) + ".out";
        }

        if (task.useWait) {
            success = executeModuleTaskPipe(task, wfnFile, cores, options);
        } else {
            success = executeModuleTaskFile(task, wfnFile, cores, options);
        }
        
        // Execute command block if module execution was successful
        if (success) {
            success = executeCommandBlock(task, wfnFile, options, outFile);
        }
        
        return success;
    }
    
    // Execute all module tasks
    bool executeAllTasks(const std::string& inpFile, const std::string& wfnFile, 
                        int cores, const ExecutionOptions& options) {
        // Parse inp file, get all tasks, optional wfn file, core count, and custom variables
        auto parseResult = InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile);
        std::vector<ModuleTask> tasks = std::get<0>(parseResult);
        std::string inputWfnFile = std::get<1>(parseResult);
        int inputCores = std::get<2>(parseResult);
        std::map<std::string, std::vector<std::string>> fileVars = std::get<3>(parseResult);
        bool inputDryrun = std::get<4>(parseResult);
        bool inputNogui = std::get<5>(parseResult);

        ExecutionOptions effectiveOptions = options;
        if (inputDryrun) {
            effectiveOptions.dryrun = true;
        }
        if (inputNogui) {
            effectiveOptions.nogui = true;
        }
        
        // Use wfn file from input file if specified, otherwise use command line argument
        std::string wfnPattern;
        if (!wfnFile.empty()) {
            wfnPattern = wfnFile;          // 命令行/位置参数优先
        } else {
            wfnPattern = inputWfnFile;     // 否则回退到输入文件 wfn=
        }
        
        // 展开通配符
        std::vector<std::string> wfnFiles = Utils::expandWildcard(wfnPattern);
        
        if (wfnFiles.empty()) {
            std::cerr << "Error: No matching wavefunction files found for pattern: " << wfnPattern << std::endl;
            return false;
        }
        
        if (wfnFiles.size() > 1) {
            std::cout << "Found " << wfnFiles.size() << " matching files:" << std::endl;
            for (const auto& f : wfnFiles) {
                std::cout << "  - " << f << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Use core count from input file if specified and no cores provided via command line
        int finalCores = cores;
        if (cores < 0 && inputCores > 0) {
            finalCores = inputCores;
            std::cout << "Using core count from input file: " << finalCores << std::endl;
        }
        
        // Merge with command line variables (command line takes precedence)
        std::map<std::string, std::vector<std::string>> allCustomVars = effectiveOptions.customVars;
        for (const auto& var : fileVars) {
            // Only add if not already set by command line
            if (allCustomVars.find(var.first) == allCustomVars.end()) {
                allCustomVars[var.first] = var.second;
            }
        }

        // Interactive variables: allow defining "var=?" at the top of input file
        // (or via -v/--var var=?) to request the value from user at runtime.
        //
        // Special handling:
        // - var*=?      -> collect var1, var2, ... and later expand ${var*} as multiple lines.
        // - len(var)=?  -> if paired with var*=?, prompt this count first and then ask exactly
        //                  that many varN values. Otherwise it behaves like a normal scalar prompt.
        InputParser::resolveInteractiveCustomVars(allCustomVars);
        
        // Collect array variables and expand them as a Cartesian product.
        std::vector<std::string> arrayVarNames;
        for (const auto& var : allCustomVars) {
            if (var.second.size() > 1) {
                arrayVarNames.push_back(var.first);
            }
        }

        const auto variableCombinations = Utils::expandVariableCombinations(allCustomVars);
        const size_t combinationCount = variableCombinations.size();
        
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
        
        if (effectiveOptions.dryrun) {
            std::cout << "\n** DRY-RUN MODE: Only generating command files **\n" << std::endl;
        }
        if (effectiveOptions.screen) {
            std::cout << "\n** SCREEN MODE: Output to screen instead of files **\n" << std::endl;
        }

        // Try to load embedded inline conf blocks (if any)
        std::map<std::string, std::string> inlineConfs = InlineConf::extractInlineConfs(inpFile);
        if (!inlineConfs.empty()) {
            std::cout << "\nDetected inline conf blocks for modules: ";
            for (const auto& kv : inlineConfs) {
                if (!kv.first.empty()) {
                    std::cout << kv.first << " ";
                }
            }
            std::cout << "\n";
        }
        
        for (const auto& mod : modules) {
            auto it = inlineConfs.find(mod);
            if (it != inlineConfs.end() && !it->second.empty()) {
                // Inline conf has higher priority
                if (!configManager.loadModuleConfigFromText(mod, it->second, inpFile + " (inline)")) {
                    std::cerr << "Error: Failed to load inline module config for " << mod << std::endl;
                    return false;
                }
            } else {
                if (!loadModuleConfig(mod)) {
                    std::cerr << "Error: Failed to load module config for " << mod << std::endl;
                    return false;
                }
            }
        }
        
        // 对每个匹配的文件和每个变量组合执行任务
        bool allSuccess = true;
        for (size_t fileIdx = 0; fileIdx < wfnFiles.size(); fileIdx++) {
            std::string finalWfnFile = wfnFiles[fileIdx];

            // 对每个变量组合执行（多个数组变量时取笛卡尔积）
            for (size_t combinationIdx = 0; combinationIdx < combinationCount; combinationIdx++) {
                const auto& currentVars = variableCombinations[combinationIdx];

                // 显示当前执行信息
                if (wfnFiles.size() > 1 || combinationCount > 1) {
                    std::cout << "\n========================================" << std::endl;
                    if (wfnFiles.size() > 1) {
                        std::cout << "Processing file " << (fileIdx + 1) << "/" << wfnFiles.size()
                                  << ": " << finalWfnFile;
                    }
                    if (combinationCount > 1) {
                        if (wfnFiles.size() > 1) std::cout << " | ";
                        std::cout << "Combination " << (combinationIdx + 1) << "/" << combinationCount;
                        if (!arrayVarNames.empty()) {
                            std::cout << " (";
                            for (size_t i = 0; i < arrayVarNames.size(); i++) {
                                const auto& varName = arrayVarNames[i];
                                auto it = currentVars.find(varName);
                                if (i > 0) {
                                    std::cout << ", ";
                                }
                                std::cout << varName << "=";
                                if (it != currentVars.end() && !it->second.empty()) {
                                    std::cout << it->second[0];
                                }
                            }
                            std::cout << ")";
                        }
                    }
                    std::cout << std::endl;
                    std::cout << "========================================\n" << std::endl;
                }

                // 为当前文件和当前变量组合创建任务副本并应用占位符替换
                std::vector<ModuleTask> fileTasks = tasks;
                InputParser::applyPlaceholderReplacement(fileTasks, finalWfnFile, currentVars);

                // Execute each module task in sequence, allowing wfn_rebase directives
                // to switch the file provided to subsequent Multiwfn invocations.
                std::string currentWfnFile = finalWfnFile;
                for (const auto& task : fileTasks) {
                    if (task.isWfnRebase) {
                        std::string target = Utils::trim(task.wfnRebaseFile);
                        if (target.empty()) {
                            // Empty rebase target means reset to the original wfn of this iteration.
                            currentWfnFile = finalWfnFile;
                            std::cout << "\n>>> wfn_rebase: reset Multiwfn input to original: " << currentWfnFile << std::endl;
                        } else {
                            currentWfnFile = target;
                            std::cout << "\n>>> wfn_rebase: switch Multiwfn input to: " << currentWfnFile << std::endl;
                        }

                        // Warn if file does not exist (still continue; Multiwfn will report errors).
                        if (!effectiveOptions.dryrun && !Utils::fileExists(currentWfnFile)) {
                            std::cerr << "Warning: wfn_rebase target file not found: " << currentWfnFile << std::endl;
                        }
                        continue;
                    }

                    if (!executeModuleTask(task, currentWfnFile, finalCores, effectiveOptions)) {
                        allSuccess = false;
                    }
                }
            }
        }
        
        if (allSuccess) {
            std::cout << "\nAll done." << std::endl;
        } else {
            printFailureLine("Some modules execution failed");
        }
        
        return allSuccess;
    }
    
    int getCores() const { return configManager.getCores(); }
};

void printUsage(const char* progName) {
    std::cout << "Hmm... You need some advice? No problem, Bane will help you! :)\n";
    std::cout << "Usage: " << progName << " <input.inp> <molecule.fchk> [options]\n";
    std::cout << "       " << progName << " -w <molecule.fchk> <input.inp> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -l, --list          List available module conf names or show a conf summary\n";
    std::cout << "  -c, --cores <num>   Specify the number of CPU cores to use\n";
    std::cout << "  -d, --dryrun        Generate command files only, don't execute (skip wait tasks)\n";
    std::cout << "  -e, --extargs <args> Pass extra arguments to Multiwfn (use quotes for multiple args)\n";
    std::cout << "      --no-color     Disable colored status output\n";
    std::cout << "  -s, --screen        Display output on screen instead of redirecting to files\n";
    std::cout << "  -n, --nogui         Run Multiwfn in silent mode\n";
    std::cout << "  -w, --wfn <file>    Specify wavefunction file (.fchk/.wfn or other supported file)\n";
    std::cout << "  -v, --var <key=val> Set custom variable for placeholder replacement (can be used multiple times)\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "\nInput header reserved words:\n";
    std::cout << "  wfn=..., core=..., dryrun=on/true, nogui=on/true, wfn_rebase=...\n";
    std::cout << "\nInput block hints:\n";
    std::cout << "  %command ... end    Run shell/batch post-commands\n";
    std::cout << "  %raw ... end/wait   Send literal Multiwfn commands\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << progName << " input.inp molecule.fchk\n";
    std::cout << "  " << progName << " -w molecule.fchk input.inp\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -c 8\n";
    std::cout << "  " << progName << " input.inp -w molecule.fchk --dryrun\n";
    std::cout << "  " << progName << " input.inp molecule.fchk --screen\n";
    std::cout << "  " << progName << " -w molecule.fchk input.inp -d -s -c 8\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -v myvar=value -v other=123\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -e \"-silent -nt 4\"\n";
    std::cout << "  # input.inp header: dryrun=on, nogui=on\n";
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
        } else if (arg == "-l" || arg == "--list") {
            // List available conf files or show a summary for a specific conf
            std::string moduleArg;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                moduleArg = argv[i + 1];
                i++;
            }

            // Find and load banewfn.rc to get confPath
            std::string configFile = findConfigFile(argv[0]);
            if (configFile.empty()) {
                std::cerr << "Error: Could not find banewfn.rc to determine conf path\n";
                return 1;
            }
            ConfigManager tmpCm;
            if (!tmpCm.loadBaneWfnConfig(configFile)) {
                return 1;
            }
            std::string confDir = tmpCm.getConfig().confPath;

            if (moduleArg.empty()) {
                // List all .conf names under confDir
                std::string pattern = confDir;
                if (!pattern.empty() && pattern.back() != '/') pattern += "/";
                pattern += "*.conf";
                std::vector<std::string> files = Utils::expandWildcard(pattern);
                if (files.empty()) {
                    std::cout << "No .conf files found in: " << confDir << std::endl;
                } else {
                    std::cout << "Available module configs in " << confDir << ":\n";
                    for (const auto &f : files) {
                        std::cout << "  " << getBaseName(f) << std::endl;
                    }
                }
                return 0;
            } else {
                // Show a compact summary for the specified conf
                if (!tmpCm.loadModuleConfig(moduleArg)) {
                    std::cerr << "Error: Failed to load module config for: " << moduleArg << std::endl;
                    return 1;
                }
                const ModuleConfig &mc = tmpCm.getModuleConfig(moduleArg);
                std::cout << "[" << moduleArg << ".conf] summary (section -> variables):\n";
                // Helper to extract variable names from a command string
                auto extractVarsFromCommand = [](const std::string &cmd, std::set<std::string> &outVars) {
                    size_t pos = 0;
                    while (pos < cmd.size()) {
                        size_t dollar = cmd.find('$', pos);
                        if (dollar == std::string::npos) break;
                        size_t next = dollar + 1;
                        if (next < cmd.size() && cmd[next] == '{') {
                            size_t braceEnd = cmd.find('}', next + 1);
                            if (braceEnd != std::string::npos) {
                                std::string inside = cmd.substr(next + 1, braceEnd - next - 1);
                                // support ${var:-default} or ${var:default}
                                size_t sep = inside.find(":-");
                                if (sep == std::string::npos) sep = inside.find(':');
                                std::string varName = (sep == std::string::npos) ? inside : inside.substr(0, sep);
                                if (!varName.empty()) outVars.insert(varName);
                                pos = braceEnd + 1;
                                continue;
                            } else {
                                pos = next + 1;
                                continue;
                            }
                        } else {
                            // $var style
                            size_t j = next;
                            while (j < cmd.size() && (isalnum((unsigned char)cmd[j]) || cmd[j] == '_')) j++;
                            if (j > next) {
                                outVars.insert(cmd.substr(next, j - next));
                                pos = j;
                                continue;
                            } else {
                                pos = next;
                            }
                        }
                    }
                };

                // Show defined sections and list variable names discovered from defaults and commands
                for (const auto &secPair : mc.sections) {
                    const std::string &sectionName = secPair.first;
                    const Section &section = secPair.second;
                    std::set<std::string> vars;
                    // add defaults keys
                    for (const auto &d : section.defaults) {
                        vars.insert(d.first);
                    }
                    // scan commands for $ placeholders
                    for (const auto &cmdLine : section.commands) {
                        extractVarsFromCommand(cmdLine, vars);
                    }

                    // Print section and variables aligned in two columns (first column fixed width)
                    const size_t nameWidth = 12;
                    if (sectionName.length() >= nameWidth) {
                        std::cout << "  " << sectionName;
                        // ensure spacing before vars
                        std::cout << std::string(4, ' ');
                    } else {
                        std::cout << "  " << std::left << std::setw((int)nameWidth) << sectionName << " ";
                        // reset formatting
                        std::cout << std::right;
                    }
                    if (!vars.empty()) {
                        size_t shown = 0;
                        for (const auto &v : vars) {
                            if (shown++) std::cout << "; ";
                            std::cout << v;
                        }
                    }
                    std::cout << "\n";
                }
                // Do not display quit commands in -l summary
                return 0;
            }
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
        } else if (arg == "--no-color") {
            options.noColor = true;
        } else if (arg == "-s" || arg == "--screen") {
            options.screen = true;
        } else if (arg == "-n" || arg == "--nogui") {
            options.nogui = true;
        } else if (arg == "-e" || arg == "--extargs") {
            if (i + 1 < argc) {
                std::string extArgsStr = argv[i + 1];
                // Parse the extargs string, supporting quotes
                std::vector<std::string> parsedArgs = Utils::parseCommandLineArgs(extArgsStr);
                options.extargs.insert(options.extargs.end(), parsedArgs.begin(), parsedArgs.end());
                i++;
            } else {
                std::cerr << "Error: -e/--extargs requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-w" || arg == "--wfn") {
            if (i + 1 < argc) {
                wfnParam = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -w/--wfn requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-v" || arg == "--var") {
            if (i + 1 < argc) {
                std::string varArg = argv[i + 1];
                size_t eqPos = varArg.find('=');
                if (eqPos != std::string::npos && eqPos > 0 && eqPos < varArg.length() - 1) {
                    std::string key = Utils::trim(varArg.substr(0, eqPos));
                    std::string value = Utils::trim(varArg.substr(eqPos + 1));
                    
                    if (isValidCustomVarName(key) && !key.empty()) {
                        if (key.size() > 1 && key.back() == '*') {
                            options.customVars[key] = {value};
                        } else if (key.size() > 5 && key.rfind("len(", 0) == 0 && key.back() == ')') {
                            options.customVars[key] = {value};
                        } else {
                            // Parse value as bash array (supports both array and single value)
                            options.customVars[key] = Utils::parseBashArray(value);
                        }
                    } else {
                        std::cerr << "Warning: Invalid variable name: " << key << std::endl;
                    }
                } else {
                    std::cerr << "Error: -v/--var requires format key=value" << std::endl;
                    return 1;
                }
                i++;
            } else {
                std::cerr << "Error: -v/--var requires an argument" << std::endl;
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
    
    // Check if input file contains wfn definition, core setting, and custom variables
    auto parseResult = InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile);
    std::string inputWfnFile = std::get<1>(parseResult);
    int inputCores = std::get<2>(parseResult);
    std::map<std::string, std::vector<std::string>> inputVars = std::get<3>(parseResult);
    bool inputDryrun = std::get<4>(parseResult);
    bool inputNogui = std::get<5>(parseResult);

    if (inputDryrun) {
        options.dryrun = true;
        std::cout << "Dry-run mode enabled by input header." << std::endl;
    }
    if (inputNogui) {
        options.nogui = true;
        std::cout << "No-GUI mode enabled by input header." << std::endl;
    }

    initializeColorOutput(options);

    bool shouldPauseOnExit = options.dryrun;
    
    // Merge input file variables with command line variables (command line takes precedence)
    for (const auto& var : inputVars) {
        if (options.customVars.find(var.first) == options.customVars.end()) {
            options.customVars[var.first] = var.second;
        }
    }
    
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
        pauseIfWindowsDryRun(shouldPauseOnExit);
        return 1;
    }
    
    // Load banewfn.rc
    if (!generator.loadBaneWfnConfig(configFile)) {
        pauseIfWindowsDryRun(shouldPauseOnExit);
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
        pauseIfWindowsDryRun(shouldPauseOnExit);
        return 1;
    }

    pauseIfWindowsDryRun(shouldPauseOnExit);
    return 0;
}