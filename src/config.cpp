#include "config.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#endif

// Utility function: trim whitespace from string (deprecated - use Utils::trim instead)
std::string trim(const std::string& str) {
    return Utils::trim(str);
}

// Utility function: expand ~ to HOME path
std::string expandPath(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }
    
    const char* home = getenv("HOME");
    if (!home) {
        return path;
    }
    
    return std::string(home) + path.substr(1);
}

// Utility function: get executable directory path
std::string getExecutableDir() {
#ifdef PLATFORM_WINDOWS
    char result[MAX_PATH];
    DWORD count = GetModuleFileNameA(NULL, result, MAX_PATH);
    if (count != 0) {
        // Extract directory from full path
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        _splitpath_s(result, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        
        std::string fullPath = std::string(drive) + std::string(dir);
        // Remove trailing backslash
        if (!fullPath.empty() && (fullPath.back() == '\\' || fullPath.back() == '/')) {
            fullPath.pop_back();
        }
        return fullPath;
    }
    return "";
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    
    if (count != -1) {
        result[count] = '\0';
        char* dir = dirname(result);
        return std::string(dir);
    }
    
    // Fallback: try to get from argv[0] if /proc/self/exe fails
    return "";
#endif
}

// Utility function: check if file exists (deprecated - use Utils::fileExists instead)
bool fileExists(const std::string& filepath) {
    return Utils::fileExists(filepath);
}

// Utility function: find config file with priority
std::string findConfigFile(const std::string& /*executablePath*/) {
    std::vector<std::string> searchPaths;
    
    // Priority 1: Current directory
    searchPaths.push_back("./banewfn.rc");
    
    // Priority 2: Executable directory
    std::string execDir = getExecutableDir();
    if (!execDir.empty()) {
        searchPaths.push_back(execDir + "/banewfn.rc");
    }
    
    // Priority 3: ~/.bane/multiwfn
    searchPaths.push_back(expandPath("~/.bane/multiwfn/banewfn.rc"));
    
    std::cout << "Searching for banewfn.rc in the following locations:" << std::endl;
    for (const auto& path : searchPaths) {
        std::cout << "  - " << path << std::endl;
    }
    std::cout << std::endl;
    
    for (const auto& path : searchPaths) {
        if (fileExists(path)) {
            std::cout << "Found: " << path << std::endl;
            return path;
        }
    }
    
    return "";
}

// Utility function: extract base filename (without path and extension)
std::string getBaseName(const std::string& filepath) {
    std::string filename = filepath;
    
    // Remove path
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }
    
    // Remove extension
    size_t lastDot = filename.find_last_of('.');
    if (lastDot != std::string::npos) {
        filename = filename.substr(0, lastDot);
    }
    
    return filename;
}

// Load banewfn.rc configuration file
bool ConfigManager::loadBaneWfnConfig(const std::string& configFile) {
    std::ifstream file(expandPath(configFile));
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << configFile << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key == "Multiwfnpath") {
                config.multiwfnPath = expandPath(value);
            } else if (key == "confpath") {
                config.confPath = expandPath(value);
            } else if (key == "cores") {
                config.cores = std::stoi(value);
            }
        }
    }
    
    file.close();
    
    if (config.multiwfnPath.empty()) {
        std::cerr << "Error: Multiwfnpath not specified in config file" << std::endl;
        return false;
    }
    
    if (config.confPath.empty()) {
        config.confPath = expandPath("~/.bane/multiwfn");
    }
    
    return true;
}

// Load module-specific conf file
bool ConfigManager::loadModuleConfig(const std::string& moduleName) {
    // If already loaded, return directly
    if (moduleConfigs.find(moduleName) != moduleConfigs.end()) {
        return true;
    }
    
    std::string confFile = config.confPath + "/" + moduleName + ".conf";
    std::ifstream file(confFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open module config file: " << confFile << std::endl;
        return false;
    }
    
    std::cout << "Loading module configuration: " << confFile << std::endl;
    
    ModuleConfig modConfig;
    std::string line;
    std::string currentSection;
    bool inDefaultBlock = false;
    bool inQuitSection = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        // Section header [section_name]
        if (line[0] == '[' && line[line.length()-1] == ']') {
            currentSection = line.substr(1, line.length() - 2);
            
            // Special handling for quit section
            if (currentSection == "quit") {
                inQuitSection = true;
                inDefaultBlock = false;
            } else {
                modConfig.sections[currentSection] = Section();
                inQuitSection = false;
                inDefaultBlock = false;
            }
            continue;
        }
        
        // Default value block
        if (line == "-default-") {
            if (!inQuitSection) {
                inDefaultBlock = true;
            }
            continue;
        }
        
        // Handle quit section commands
        if (inQuitSection) {
            modConfig.quitCommands.push_back(line);
            continue;
        }
        
        // Handle regular sections
        if (!currentSection.empty()) {
            if (inDefaultBlock) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    if (value.length() >= 2 && value[0] == '"' && value[value.length()-1] == '"') {
                        value = value.substr(1, value.length() - 2);
                    }
                    modConfig.sections[currentSection].defaults[key] = value;
                }
            } else {
                modConfig.sections[currentSection].commands.push_back(line);
            }
        }
    }
    
    file.close();
    
    // If no quit section defined, use default value
    if (modConfig.quitCommands.empty()) {
        std::cout << "Warning: Module " << moduleName << " does not define [quit] section, using default quit command 'q'" << std::endl;
        modConfig.quitCommands.push_back("q");
    }
    
    moduleConfigs[moduleName] = modConfig;
    return true;
}

// Get module configuration
const ModuleConfig& ConfigManager::getModuleConfig(const std::string& moduleName) const {
    static ModuleConfig emptyConfig;
    auto it = moduleConfigs.find(moduleName);
    if (it != moduleConfigs.end()) {
        return it->second;
    }
    return emptyConfig;
}

// Check if module configuration exists
bool ConfigManager::hasModuleConfig(const std::string& moduleName) const {
    return moduleConfigs.find(moduleName) != moduleConfigs.end();
}

// Replace placeholders in command string
std::string replacePlaceholders(const std::string& cmd, 
                               const std::map<std::string, std::string>& params) {
    std::string result = cmd;
    
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        while (endPos < result.length() && 
               (isalnum(result[endPos]) || result[endPos] == '_')) {
            endPos++;
        }
        
        std::string varName = result.substr(pos + 1, endPos - pos - 1);
        std::string value = "";
        
        if (params.find(varName) != params.end()) {
            value = params.at(varName);
        }
        
        result.replace(pos, endPos - pos, value);
        pos += value.length();
    }
    
    return result;
}
