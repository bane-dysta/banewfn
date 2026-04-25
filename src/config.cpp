#include "config.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <cctype>
#include <algorithm>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#endif

namespace {

struct ParsedPlaceholder {
    size_t pos;
    size_t endPos;
    std::string varName;
    std::string defaultValue;
};

bool parseNextPlaceholder(const std::string& text, size_t startPos, ParsedPlaceholder& out) {
    const size_t pos = text.find('$', startPos);
    if (pos == std::string::npos) {
        return false;
    }

    size_t endPos = pos + 1;
    out.pos = pos;
    out.defaultValue.clear();
    out.varName.clear();

    if (endPos < text.length() && text[endPos] == '{') {
        const size_t braceStart = endPos + 1;
        const size_t braceEnd = text.find('}', braceStart);
        if (braceEnd != std::string::npos) {
            const std::string inside = text.substr(braceStart, braceEnd - braceStart);
            const size_t defaultSep = inside.find(":-");
            if (defaultSep != std::string::npos) {
                out.varName = inside.substr(0, defaultSep);
                out.defaultValue = inside.substr(defaultSep + 2);
            } else {
                out.varName = inside;
            }
            out.endPos = braceEnd + 1;
            return true;
        }
    }

    while (endPos < text.length() &&
           (std::isalnum(static_cast<unsigned char>(text[endPos])) || text[endPos] == '_')) {
        endPos++;
    }
    out.varName = text.substr(pos + 1, endPos - pos - 1);
    out.endPos = endPos;
    return true;
}

bool isLenVariableName(const std::string& varName, std::string* baseName = nullptr) {
    if (varName.size() <= 5 || varName.rfind("len(", 0) != 0 || varName.back() != ')') {
        return false;
    }

    const std::string base = varName.substr(4, varName.size() - 5);
    if (base.empty()) {
        return false;
    }

    if (baseName != nullptr) {
        *baseName = base;
    }
    return true;
}

bool isListVariableName(const std::string& varName, std::string* baseName = nullptr) {
    if (varName.size() <= 1 || varName.back() != '*') {
        return false;
    }

    const std::string base = varName.substr(0, varName.size() - 1);
    if (base.empty()) {
        return false;
    }

    if (baseName != nullptr) {
        *baseName = base;
    }
    return true;
}

bool parseIndexedVariableName(const std::string& varName, std::string* baseName, int* index) {
    size_t splitPos = varName.size();
    while (splitPos > 0 && std::isdigit(static_cast<unsigned char>(varName[splitPos - 1]))) {
        splitPos--;
    }

    if (splitPos == 0 || splitPos == varName.size()) {
        return false;
    }

    const std::string base = varName.substr(0, splitPos);
    const std::string indexText = varName.substr(splitPos);
    if (base.empty() || indexText.empty()) {
        return false;
    }

    const int parsedIndex = std::atoi(indexText.c_str());
    if (parsedIndex <= 0) {
        return false;
    }

    if (baseName != nullptr) {
        *baseName = base;
    }
    if (index != nullptr) {
        *index = parsedIndex;
    }
    return true;
}

std::vector<std::string> parseListLiteral(const std::string& rawValue) {
    const std::string trimmedValue = Utils::trim(rawValue);
    if (trimmedValue.empty()) {
        return {};
    }
    return Utils::parseBashArray(trimmedValue);
}

std::string serializeListValues(const std::vector<std::string>& values) {
    if (values.empty()) {
        return "";
    }

    if (values.size() == 1) {
        return values.front();
    }

    std::string result = "(";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            result += " ";
        }
        if (values[i].empty()) {
            result += "\"\"";
        } else {
            result += values[i];
        }
    }
    result += ")";
    return result;
}

std::vector<std::string> collectListValues(const std::map<std::string, std::string>& params,
                                           const std::string& baseName) {
    const auto directIt = params.find(baseName + "*");
    if (directIt != params.end()) {
        const std::vector<std::string> directValues = parseListLiteral(directIt->second);
        if (!directValues.empty()) {
            return directValues;
        }
    }

    std::vector<std::pair<int, std::string>> indexedValues;
    for (const auto& kv : params) {
        std::string indexedBase;
        int index = 0;
        if (parseIndexedVariableName(kv.first, &indexedBase, &index) && indexedBase == baseName) {
            indexedValues.push_back({index, kv.second});
        }
    }

    std::sort(indexedValues.begin(), indexedValues.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

    std::vector<std::string> values;
    for (const auto& item : indexedValues) {
        values.push_back(item.second);
    }
    return values;
}

std::string resolveScalarPlaceholderValue(const std::string& varName,
                                          const std::string& defaultValue,
                                          const std::map<std::string, std::string>& params) {
    const auto it = params.find(varName);
    if (it != params.end() && !it->second.empty()) {
        if (isListVariableName(varName)) {
            const std::string baseName = varName.substr(0, varName.size() - 1);
            const std::vector<std::string> values = collectListValues(params, baseName);
            if (!values.empty()) {
                return serializeListValues(values);
            }
        }
        return it->second;
    }

    std::string baseName;
    if (isLenVariableName(varName, &baseName)) {
        const std::vector<std::string> values = collectListValues(params, baseName);
        if (!values.empty()) {
            return std::to_string(values.size());
        }
    }

    if (isListVariableName(varName, &baseName)) {
        const std::vector<std::string> values = collectListValues(params, baseName);
        if (!values.empty()) {
            return serializeListValues(values);
        }
    }

    if (!defaultValue.empty()) {
        if (isListVariableName(varName)) {
            return serializeListValues(parseListLiteral(defaultValue));
        }
        return defaultValue;
    }

    return "";
}

} // namespace

// Utility function: trim whitespace from string (deprecated - use Utils::trim instead)
std::string trim(const std::string& str) {
    return Utils::trim(str);
}

// Utility function: expand ~ and $HOME/${HOME}
std::string expandPath(const std::string& path) {
    if (path.empty()) {
        return path;
    }

    // Resolve HOME directory
    const char* homeEnv = getenv("HOME");
#ifdef PLATFORM_WINDOWS
    if (!homeEnv) {
        homeEnv = getenv("USERPROFILE");
    }
#endif
    const std::string homeDir = homeEnv ? std::string(homeEnv) : std::string("");

    std::string result = path;

    // Expand leading ~ (e.g., ~/foo or ~)
    if (!homeDir.empty() && result[0] == '~') {
        // Replace only the first character '~'
        result = homeDir + result.substr(1);
    }

    // Expand $HOME occurrences
    if (!homeDir.empty()) {
        // Replace ${HOME}
        std::string keyBraced = "${HOME}";
        size_t pos = 0;
        while ((pos = result.find(keyBraced, pos)) != std::string::npos) {
            result.replace(pos, keyBraced.size(), homeDir);
            pos += homeDir.size();
        }
        // Replace $HOME (avoid overlapping with ${HOME} which is already handled)
        std::string keySimple = "$HOME";
        pos = 0;
        while ((pos = result.find(keySimple, pos)) != std::string::npos) {
            result.replace(pos, keySimple.size(), homeDir);
            pos += homeDir.size();
        }
    }

    return result;
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
    
    // Priority 3: ~/.bane/wfn
    searchPaths.push_back(expandPath("~/.bane/wfn/banewfn.rc"));

#ifndef PLATFORM_WINDOWS
    // Priority 4: system config installed by CMake, plus prefix-relative
    // fallbacks for staged installs and non-default prefixes.
#ifdef BANEWFN_SYSTEM_RC_PATH
    searchPaths.push_back(BANEWFN_SYSTEM_RC_PATH);
#endif
#ifdef BANEWFN_SYSTEM_SHARE_RC_PATH
    searchPaths.push_back(BANEWFN_SYSTEM_SHARE_RC_PATH);
#endif
    if (!execDir.empty()) {
        searchPaths.push_back(execDir + "/../etc/banewfn/banewfn.rc");
        searchPaths.push_back(execDir + "/../share/banewfn/conf/banewfn.rc");
    }
#endif
    
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
bool ConfigManager::loadBaneWfnConfig(const std::string& configFile, bool requireMultiwfnExec) {
    std::ifstream file(expandPath(configFile));
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << configFile << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // 去除行内注释
        line = Utils::removeInlineComment(line);
        
        if (line.empty()) continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key == "Multiwfn_exec") {
                config.multiwfnExec = expandPath(value);
            } else if (key == "confpath") {
                config.confPath = expandPath(value);
            } else if (key == "cores") {
                try {
                    int parsedCores = std::stoi(value);
                    if (parsedCores < 0) {
                        std::cerr << "Warning: Invalid negative cores value in config file: "
                                  << value << ". Keeping default: " << config.cores << std::endl;
                    } else {
                        config.cores = parsedCores;
                    }
                } catch (const std::exception&) {
                    std::cerr << "Warning: Invalid cores value in config file: "
                              << value << ". Keeping default: " << config.cores << std::endl;
                }
            } else if (key == "gitbash_exec") {
#ifdef PLATFORM_WINDOWS
                config.gitbashExec = expandPath(value);
#else
                // Ignore on non-Windows platforms
#endif
            }
        }
    }
    
    file.close();
    
    if (requireMultiwfnExec && config.multiwfnExec.empty()) {
        std::cerr << "Error: Multiwfn_exec not specified in config file" << std::endl;
        return false;
    }
    
    if (config.confPath.empty()) {
        config.confPath = expandPath("~/.bane/wfn");
    }
    
    return true;
}

// Shared parser for module config (from file or inline text)
bool ConfigManager::parseModuleConfigStream(std::istream& in, const std::string& moduleName, const std::string& origin) {
    ModuleConfig modConfig;
    std::string line;
    std::string currentSection;
    bool inDefaultBlock = false;
    bool inQuitSection = false;

    while (std::getline(in, line)) {
        line = trim(line);

        // 去除行内注释
        line = Utils::removeInlineComment(line);

        if (line.empty()) continue;

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

    // If no quit section defined, use default value
    if (modConfig.quitCommands.empty()) {
        std::cout << "Warning: Module " << moduleName << " does not define [quit] section (" << origin << ")." << std::endl;
        modConfig.quitCommands.push_back("q");
    }

    moduleConfigs[moduleName] = modConfig;
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

    bool ok = parseModuleConfigStream(file, moduleName, confFile);
    file.close();
    return ok;
}

bool ConfigManager::loadModuleConfigFromText(const std::string& moduleName, const std::string& confText, const std::string& origin) {
    // If already loaded, return directly
    if (moduleConfigs.find(moduleName) != moduleConfigs.end()) {
        return true;
    }

    std::istringstream iss(confText);
    std::cout << "Loading module configuration for " << moduleName << " from " << origin << std::endl;
    return parseModuleConfigStream(iss, moduleName, origin);
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

    ParsedPlaceholder placeholder;
    size_t searchPos = 0;
    while (parseNextPlaceholder(result, searchPos, placeholder)) {
        const std::string value = resolveScalarPlaceholderValue(
            placeholder.varName, placeholder.defaultValue, params);
        result.replace(placeholder.pos, placeholder.endPos - placeholder.pos, value);
        searchPos = placeholder.pos + value.length();
    }

    return result;
}

std::vector<std::string> replacePlaceholdersExpanded(const std::string& cmd,
                                                     const std::map<std::string, std::string>& params) {
    std::vector<std::string> expandedLines = {""};

    size_t cursor = 0;
    ParsedPlaceholder placeholder;
    while (parseNextPlaceholder(cmd, cursor, placeholder)) {
        const std::string literal = cmd.substr(cursor, placeholder.pos - cursor);
        for (auto& line : expandedLines) {
            line += literal;
        }

        std::string baseName;
        if (isListVariableName(placeholder.varName, &baseName)) {
            std::vector<std::string> values = collectListValues(params, baseName);
            if (values.empty() && !placeholder.defaultValue.empty()) {
                values = parseListLiteral(placeholder.defaultValue);
            }

            if (values.empty()) {
                return {};
            }

            std::vector<std::string> nextLines;
            nextLines.reserve(expandedLines.size() * values.size());
            for (const auto& prefix : expandedLines) {
                for (const auto& value : values) {
                    nextLines.push_back(prefix + value);
                }
            }
            expandedLines.swap(nextLines);
        } else {
            const std::string replacement = resolveScalarPlaceholderValue(
                placeholder.varName, placeholder.defaultValue, params);
            for (auto& line : expandedLines) {
                line += replacement;
            }
        }

        cursor = placeholder.endPos;
    }

    const std::string tail = cmd.substr(cursor);
    for (auto& line : expandedLines) {
        line += tail;
    }

    return expandedLines;
}
