#include "input.h"
#include "config.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace {

struct ParsedPlaceholder {
    size_t pos;
    size_t endPos;
    std::string varName;
    std::string defaultValue;
    bool usedBraces;
};

std::string toLowerAscii(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

bool tryParseInputBoolFlag(const std::string& rawValue, const std::string& flagName, bool& outValue) {
    std::string value = toLowerAscii(Utils::trim(rawValue));

    if (value == "on" || value == "true" || value == "yes" || value == "1") {
        outValue = true;
        return true;
    }

    if (value == "off" || value == "false" || value == "flase" || value == "no" || value == "0") {
        outValue = false;
        return true;
    }

    std::cerr << "Warning: Invalid " << flagName << " value in input header: " << value
              << " (expected on/true or off/false)" << std::endl;
    return false;
}

bool parseNextPlaceholder(const std::string& text, size_t startPos, ParsedPlaceholder& out) {
    const size_t pos = text.find('$', startPos);
    if (pos == std::string::npos) {
        return false;
    }

    size_t endPos = pos + 1;
    out.pos = pos;
    out.defaultValue.clear();
    out.varName.clear();
    out.usedBraces = false;

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
            out.usedBraces = true;
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

bool isPlainVariableName(const std::string& name) {
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

bool isListVariableName(const std::string& name, std::string* baseName = nullptr) {
    if (name.size() <= 1 || name.back() != '*') {
        return false;
    }

    const std::string base = name.substr(0, name.size() - 1);
    if (!isPlainVariableName(base)) {
        return false;
    }

    if (baseName != nullptr) {
        *baseName = base;
    }
    return true;
}

bool isLenVariableName(const std::string& name, std::string* baseName = nullptr) {
    if (name.size() <= 5 || name.rfind("len(", 0) != 0 || name.back() != ')') {
        return false;
    }

    const std::string base = name.substr(4, name.size() - 5);
    if (!isPlainVariableName(base)) {
        return false;
    }

    if (baseName != nullptr) {
        *baseName = base;
    }
    return true;
}

bool parseIndexedVariableName(const std::string& name, std::string* baseName, int* index) {
    size_t splitPos = name.size();
    while (splitPos > 0 && std::isdigit(static_cast<unsigned char>(name[splitPos - 1]))) {
        splitPos--;
    }

    if (splitPos == 0 || splitPos == name.size()) {
        return false;
    }

    const std::string base = name.substr(0, splitPos);
    const std::string indexText = name.substr(splitPos);
    if (!isPlainVariableName(base) || indexText.empty()) {
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

bool isValidCustomVarName(const std::string& key) {
    return isPlainVariableName(key) || isListVariableName(key) || isLenVariableName(key);
}

bool isInteractiveMarker(const std::vector<std::string>& values) {
    return values.size() == 1 && Utils::trim(values[0]) == "?";
}

std::vector<std::string> parseListLiteral(const std::string& rawValue) {
    const std::string trimmedValue = Utils::trim(rawValue);
    if (trimmedValue.empty()) {
        return {};
    }
    return Utils::parseBashArray(trimmedValue);
}

std::vector<std::string> parseStoredListValues(const std::vector<std::string>& values) {
    if (values.empty()) {
        return {};
    }

    if (values.size() == 1) {
        return parseListLiteral(values.front());
    }

    return values;
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

std::vector<std::string> collectListValues(const std::map<std::string, std::vector<std::string>>& vars,
                                           const std::string& baseName) {
    const auto directIt = vars.find(baseName + "*");
    if (directIt != vars.end()) {
        const std::vector<std::string> directValues = parseStoredListValues(directIt->second);
        if (!directValues.empty()) {
            return directValues;
        }
    }

    std::vector<std::pair<int, std::string>> indexedValues;
    for (const auto& kv : vars) {
        std::string indexedBase;
        int index = 0;
        if (parseIndexedVariableName(kv.first, &indexedBase, &index) && indexedBase == baseName) {
            const std::string value = kv.second.empty() ? std::string("") : kv.second.front();
            indexedValues.push_back({index, value});
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

std::string makeLenKey(const std::string& baseName) {
    return "len(" + baseName + ")";
}

void clearIndexedValues(std::map<std::string, std::vector<std::string>>& vars, const std::string& baseName) {
    std::vector<std::string> keysToErase;
    for (const auto& kv : vars) {
        std::string indexedBase;
        int index = 0;
        if (parseIndexedVariableName(kv.first, &indexedBase, &index) && indexedBase == baseName) {
            keysToErase.push_back(kv.first);
        }
    }

    for (const auto& key : keysToErase) {
        vars.erase(key);
    }
}

void materializeListVariable(std::map<std::string, std::vector<std::string>>& vars,
                             const std::string& listKey,
                             const std::vector<std::string>& values,
                             bool updateLenWhenMissingOnly) {
    std::string baseName;
    if (!isListVariableName(listKey, &baseName)) {
        return;
    }

    vars[listKey] = {serializeListValues(values)};
    clearIndexedValues(vars, baseName);
    for (size_t i = 0; i < values.size(); ++i) {
        vars[baseName + std::to_string(i + 1)] = {values[i]};
    }

    const std::string lenKey = makeLenKey(baseName);
    const auto lenIt = vars.find(lenKey);
    if (lenIt == vars.end() || !updateLenWhenMissingOnly || isInteractiveMarker(lenIt->second) ||
        lenIt->second.empty() || Utils::trim(lenIt->second.front()).empty()) {
        vars[lenKey] = {std::to_string(values.size())};
    }
}

void materializeAllListVariables(std::map<std::string, std::vector<std::string>>& vars) {
    std::vector<std::string> listKeys;
    for (const auto& kv : vars) {
        if (isListVariableName(kv.first) && !isInteractiveMarker(kv.second)) {
            listKeys.push_back(kv.first);
        }
    }

    for (const auto& listKey : listKeys) {
        const auto it = vars.find(listKey);
        if (it == vars.end()) {
            continue;
        }
        materializeListVariable(vars, listKey, parseStoredListValues(it->second), true);
    }
}

bool tryParseNonNegativeInteger(const std::string& text, int& value) {
    const std::string trimmedText = Utils::trim(text);
    if (trimmedText.empty()) {
        value = 0;
        return true;
    }

    for (char c : trimmedText) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    value = std::atoi(trimmedText.c_str());
    return value >= 0;
}

struct InputPlaceholderResolution {
    bool found;
    std::string value;
};

InputPlaceholderResolution resolveInputScalarPlaceholder(const std::string& varName,
                                                         const std::string& defaultValue,
                                                         bool usedBraces,
                                                         const std::string& wfnFile,
                                                         const std::map<std::string, std::vector<std::string>>& customVars) {
    if (varName == "output") {
        return {false, ""};
    }

    const auto customIt = customVars.find(varName);
    if (customIt != customVars.end() && !customIt->second.empty()) {
        if (isListVariableName(varName)) {
            return {true, serializeListValues(collectListValues(customVars, varName.substr(0, varName.size() - 1)))};
        }
        return {true, customIt->second.front()};
    }

    std::string baseName;
    if (isLenVariableName(varName, &baseName)) {
        const std::vector<std::string> values = collectListValues(customVars, baseName);
        if (!values.empty()) {
            return {true, std::to_string(values.size())};
        }
    }

    if (isListVariableName(varName, &baseName)) {
        const std::vector<std::string> values = collectListValues(customVars, baseName);
        if (!values.empty()) {
            return {true, serializeListValues(values)};
        }
    }

    if (varName == "wfn") {
        return {true, wfnFile};
    }

    if (varName == "input") {
        return {true, getBaseName(wfnFile)};
    }

    if (usedBraces && Utils::fileExists(varName)) {
        std::ifstream f(varName);
        if (f.good()) {
            std::stringstream buffer;
            buffer << f.rdbuf();
            return {true, Utils::trim(buffer.str())};
        }
    }

    if (!defaultValue.empty()) {
        if (isListVariableName(varName)) {
            return {true, serializeListValues(parseListLiteral(defaultValue))};
        }
        return {true, defaultValue};
    }

    return {false, ""};
}

} // namespace

// Utility function: split string (deprecated - use Utils::split instead)
std::vector<std::string> InputParser::split(const std::string& str, char delimiter) {
    return Utils::split(str, delimiter);
}

// Replace input file placeholders ($input and ${input}) with wavefunction filename without extension
// Additionally, support ${name} -> read value from file "name" in current directory (trimmed)
// Also support custom variables from command line or file header
// For array variables, use the first element for replacement
std::string InputParser::replaceInputPlaceholders(const std::string& text, const std::string& wfnFile, const std::map<std::string, std::vector<std::string>>& customVars) {
    std::string result = text;

    ParsedPlaceholder placeholder;
    size_t searchPos = 0;
    while (parseNextPlaceholder(result, searchPos, placeholder)) {
        const auto resolved = resolveInputScalarPlaceholder(
            placeholder.varName, placeholder.defaultValue, placeholder.usedBraces, wfnFile, customVars);

        if (resolved.found) {
            result.replace(placeholder.pos, placeholder.endPos - placeholder.pos, resolved.value);
            searchPos = placeholder.pos + resolved.value.length();
        } else {
            searchPos = placeholder.endPos;
        }
    }

    return result;
}

static std::vector<std::string> replaceInputPlaceholdersExpanded(
    const std::string& text,
    const std::string& wfnFile,
    const std::map<std::string, std::vector<std::string>>& customVars) {
    std::vector<std::string> expandedLines = {""};

    size_t cursor = 0;
    ParsedPlaceholder placeholder;
    while (parseNextPlaceholder(text, cursor, placeholder)) {
        const std::string literal = text.substr(cursor, placeholder.pos - cursor);
        for (auto& line : expandedLines) {
            line += literal;
        }

        std::string baseName;
        if (isListVariableName(placeholder.varName, &baseName)) {
            std::vector<std::string> values = collectListValues(customVars, baseName);
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
            const auto resolved = resolveInputScalarPlaceholder(
                placeholder.varName, placeholder.defaultValue, placeholder.usedBraces, wfnFile, customVars);
            const std::string replacement = resolved.found
                ? resolved.value
                : text.substr(placeholder.pos, placeholder.endPos - placeholder.pos);
            for (auto& line : expandedLines) {
                line += replacement;
            }
        }

        cursor = placeholder.endPos;
    }

    const std::string tail = text.substr(cursor);
    for (auto& line : expandedLines) {
        line += tail;
    }

    return expandedLines;
}

// Apply placeholder replacement to all tasks using wavefunction filename and custom variables
void InputParser::applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile, const std::map<std::string, std::vector<std::string>>& customVars) {
    for (auto& task : tasks) {
        // Apply replacement to wfn_rebase directive target file
        if (task.isWfnRebase) {
            task.wfnRebaseFile = replaceInputPlaceholders(task.wfnRebaseFile, wfnFile, customVars);
        }

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

        // Apply replacement to pre-raw Multiwfn commands
        std::vector<std::string> expandedPreRawCommands;
        for (const auto& preRawCommand : task.preRawCommands) {
            const auto expandedLines = replaceInputPlaceholdersExpanded(preRawCommand, wfnFile, customVars);
            expandedPreRawCommands.insert(expandedPreRawCommands.end(), expandedLines.begin(), expandedLines.end());
        }
        task.preRawCommands.swap(expandedPreRawCommands);

        // Apply replacement to raw Multiwfn commands
        std::vector<std::string> expandedRawCommands;
        for (const auto& rawCommand : task.rawCommands) {
            const auto expandedLines = replaceInputPlaceholdersExpanded(rawCommand, wfnFile, customVars);
            expandedRawCommands.insert(expandedRawCommands.end(), expandedLines.begin(), expandedLines.end());
        }
        task.rawCommands.swap(expandedRawCommands);
        
        // Apply replacement to commands
        for (auto& command : task.commands) {
            command = replaceInputPlaceholders(command, wfnFile, customVars);
        }
    }
}

void InputParser::resolveInteractiveCustomVars(std::map<std::string, std::vector<std::string>>& customVars) {
    std::vector<std::string> interactiveListKeys;
    for (const auto& kv : customVars) {
        if (isListVariableName(kv.first) && isInteractiveMarker(kv.second)) {
            interactiveListKeys.push_back(kv.first);
        }
    }

    for (const auto& listKey : interactiveListKeys) {
        std::string baseName;
        if (!isListVariableName(listKey, &baseName)) {
            continue;
        }

        const std::string lenKey = makeLenKey(baseName);
        int requestedCount = -1;

        auto lenIt = customVars.find(lenKey);
        if (lenIt != customVars.end()) {
            if (isInteractiveMarker(lenIt->second)) {
                while (true) {
                    std::cout << "Bane need count for variable list '" << baseName
                              << "' (" << lenKey << ", empty for 0): ";
                    std::string lenInput;
                    std::getline(std::cin, lenInput);

                    int parsedCount = 0;
                    if (tryParseNonNegativeInteger(lenInput, parsedCount)) {
                        requestedCount = parsedCount;
                        lenIt->second = {std::to_string(parsedCount)};
                        break;
                    }

                    std::cout << "Warning: Invalid " << lenKey
                              << " value, please enter a non-negative integer." << std::endl;
                }
            } else if (!lenIt->second.empty()) {
                int parsedCount = 0;
                if (tryParseNonNegativeInteger(lenIt->second.front(), parsedCount)) {
                    requestedCount = parsedCount;
                } else {
                    std::cerr << "Warning: Invalid " << lenKey << " value: "
                              << lenIt->second.front()
                              << ". Falling back to blank-terminated collection for " << listKey
                              << std::endl;
                }
            }
        }

        std::vector<std::string> values;
        if (requestedCount >= 0) {
            values.reserve(static_cast<size_t>(requestedCount));
            for (int i = 1; i <= requestedCount; ++i) {
                std::cout << "Bane need value for variable '" << baseName << i << "': ";
                std::string userInput;
                std::getline(std::cin, userInput);
                values.push_back(Utils::trimQuotes(Utils::trim(userInput)));
            }
        } else {
            for (int i = 1;; ++i) {
                std::cout << "Bane need value for variable '" << baseName << i
                          << "' (empty to finish): ";
                std::string userInput;
                std::getline(std::cin, userInput);
                const std::string cleanedValue = Utils::trimQuotes(Utils::trim(userInput));
                if (cleanedValue.empty()) {
                    break;
                }
                values.push_back(cleanedValue);
            }
        }

        materializeListVariable(customVars, listKey, values, false);
    }

    for (auto& kv : customVars) {
        if (!isInteractiveMarker(kv.second)) {
            continue;
        }
        if (isListVariableName(kv.first)) {
            continue;
        }

        std::cout << "Bane need value for variable '" << kv.first;
        if (isLenVariableName(kv.first)) {
            std::cout << "' (non-negative integer, empty for 0): ";
            while (true) {
                std::string userInput;
                std::getline(std::cin, userInput);
                int parsedCount = 0;
                if (tryParseNonNegativeInteger(userInput, parsedCount)) {
                    kv.second = {std::to_string(parsedCount)};
                    break;
                }
                std::cout << "Warning: Invalid " << kv.first
                          << " value, please enter a non-negative integer: ";
            }
        } else {
            std::cout << "' (supports bash array like (a b c), empty for blank): ";
            std::string userInput;
            std::getline(std::cin, userInput);
            userInput = Utils::trim(userInput);
            kv.second = Utils::parseBashArray(userInput);
            if (kv.second.size() == 1) {
                kv.second[0] = Utils::trimQuotes(kv.second[0]);
            }
        }
    }

    materializeAllListVariables(customVars);
}

// Parse inp file, return named parse result with module tasks, headers, variables, and load status
ParsedInputFile InputParser::parseInpFileDetailed(const std::string& inpFile) {
    ParsedInputFile parsed;
    std::vector<ModuleTask>& tasks = parsed.tasks;
    std::string& wfnFile = parsed.wfnFile;
    int& cores = parsed.cores;  // -1 means not specified
    std::map<std::string, std::vector<std::string>>& customVars = parsed.customVars;
    bool& dryrun = parsed.dryrun;
    bool& nogui = parsed.nogui;

    std::ifstream file(inpFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open inp file: " << inpFile << std::endl;
        return parsed;
    }
    parsed.loaded = true;
    
    std::string line;
    ModuleTask currentTask;
    bool inProcessMode = false;
    bool inCommandMode = false;
    bool inRawMode = false;
    bool inPreRawMode = false;
    std::map<std::string, int> moduleBlockCounters;  // Track block indices for each module name
    int anonymousBlockCounter = 0;

    auto ensureAnonymousTask = [&]() {
        if (currentTask.moduleName.empty() && currentTask.commands.empty() &&
            currentTask.preRawCommands.empty() && currentTask.rawCommands.empty()) {
            currentTask = ModuleTask();
            currentTask.useWait = false;
            currentTask.blockIndex = anonymousBlockCounter++;
        }
    };

    auto hasPendingTask = [&]() {
        return !currentTask.moduleName.empty() || !currentTask.commands.empty() ||
               !currentTask.preRawCommands.empty() || !currentTask.rawCommands.empty();
    };
    
    while (std::getline(file, line)) {
        // 保留前导空白用于模块行"顶格"判断
        std::string noComment = Utils::removeInlineComment(line);
        std::string trimmed = Utils::trim(noComment);

        // In command mode, check for special keywords first (before storing raw line)
        // Special keywords still need to be detected even in command mode
        bool isSpecialKeyword = false;
        
        // Enter command mode
        if (trimmed == "%command") {
            isSpecialKeyword = true;
            // Allow top-level %command without module definition ("裸command")
            ensureAnonymousTask();
            inCommandMode = true;
            inProcessMode = false;
            inPreRawMode = false;
            inRawMode = false;
            continue;
        }

        // Enter pre-raw Multiwfn command mode
        if (trimmed == "%preraw") {
            isSpecialKeyword = true;
            // Primarily used inside module blocks, but allow anonymous blocks for consistency
            ensureAnonymousTask();
            inPreRawMode = true;
            inCommandMode = false;
            inProcessMode = false;
            inRawMode = false;
            continue;
        }

        // Enter raw Multiwfn command mode
        if (trimmed == "%raw") {
            isSpecialKeyword = true;
            // Allow top-level %raw without module definition
            ensureAnonymousTask();
            inRawMode = true;
            inCommandMode = false;
            inPreRawMode = false;
            inProcessMode = false;
            continue;
        }
        
        // End current module with "end"
        if (trimmed == "end") {
            isSpecialKeyword = true;
            if (hasPendingTask()) {
                tasks.push_back(currentTask);
                currentTask = ModuleTask();
            }
            inProcessMode = false;
            inCommandMode = false;
            inPreRawMode = false;
            inRawMode = false;
            continue;
        }
        
        // End current module with "wait" (interactive mode)
        if (trimmed == "wait") {
            isSpecialKeyword = true;
            if (hasPendingTask()) {
                currentTask.useWait = !currentTask.moduleName.empty() ||
                                      !currentTask.preRawCommands.empty() ||
                                      !currentTask.rawCommands.empty();
                tasks.push_back(currentTask);
                currentTask = ModuleTask();
            }
            inProcessMode = false;
            inCommandMode = false;
            inPreRawMode = false;
            inRawMode = false;
            continue;
        }

        // Enter post-processing mode.
        // Keep %process literal inside %command/%raw for backward compatibility,
        // but allow it to terminate %preraw so the block can appear before other module content.
        if (trimmed == "%process" && !inCommandMode && !inRawMode) {
            isSpecialKeyword = true;
            if (currentTask.moduleName.empty()) {
                std::cerr << "Warning: %process without module definition" << std::endl;
                continue;
            }
            inProcessMode = true;
            inCommandMode = false;
            inPreRawMode = false;
            inRawMode = false;
            continue;
        }
        
        // In command mode, store raw line (including comments and empty lines) if not special keyword
        if (inCommandMode && !isSpecialKeyword) {
            // Command mode: store the entire raw line as a command (preserve comments and empty lines)
            currentTask.commands.push_back(line);
            continue;
        }

        // In pre-raw mode, store literal Multiwfn input line (preserve comments and empty lines)
        if (inPreRawMode && !isSpecialKeyword) {
            currentTask.preRawCommands.push_back(line);
            continue;
        }

        // In raw mode, store literal Multiwfn input line (preserve comments and empty lines)
        if (inRawMode && !isSpecialKeyword) {
            currentTask.rawCommands.push_back(line);
            continue;
        }

        // For non-command mode, skip empty lines
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

        // Check for dryrun=on/true format at the beginning of file
        if (trimmed.find("dryrun=") == 0 && tasks.empty() && currentTask.moduleName.empty() && !inProcessMode && !inCommandMode && !inPreRawMode && !inRawMode) {
            tryParseInputBoolFlag(trimmed.substr(7), "dryrun", dryrun);
            continue;
        }

        // Check for nogui=on/true format at the beginning of file
        if (trimmed.find("nogui=") == 0 && tasks.empty() && currentTask.moduleName.empty() && !inProcessMode && !inCommandMode && !inPreRawMode && !inRawMode) {
            tryParseInputBoolFlag(trimmed.substr(6), "nogui", nogui);
            continue;
        }

        // Special directive: wfn_rebase=xxx
        // It can appear between blocks to switch the file provided to subsequent Multiwfn tasks.
        // Only recognized when not inside any module/%process/%command.
        if (trimmed.find("wfn_rebase=") == 0 && currentTask.moduleName.empty() && !inProcessMode && !inCommandMode && !inPreRawMode && !inRawMode) {
            ModuleTask rebaseTask;
            rebaseTask.isWfnRebase = true;
            rebaseTask.wfnRebaseFile = Utils::trim(trimmed.substr(std::string("wfn_rebase=").size()));
            tasks.push_back(rebaseTask);
            continue;
        }
        
        // Check for key=value format at the beginning of file (custom variables)
        // This should come before module definitions, so check if no module is active
        if (currentTask.moduleName.empty() && !inProcessMode && !inCommandMode && !inPreRawMode && !inRawMode) {
            size_t eqPos = trimmed.find('=');
            // Only treat as variable if:
            // 1. Contains exactly one '='
            // 2. Key is not empty
            // 3. Key contains only alphanumeric and underscore (valid variable name)
            // 4. Not a special keyword (wfn, core)
            if (eqPos != std::string::npos && eqPos > 0 && eqPos < trimmed.length() - 1) {
                std::string key = Utils::trim(trimmed.substr(0, eqPos));
                std::string value = Utils::trim(trimmed.substr(eqPos + 1));
                
                // Only accept if key is valid and not a special keyword
                if (isValidCustomVarName(key) && !key.empty() && key != "wfn" && key != "core" &&
                    key != "wfn_rebase" && key != "dryrun" && key != "nogui") {
                    if (isListVariableName(key)) {
                        customVars[key] = {value};
                    } else if (isLenVariableName(key)) {
                        customVars[key] = {value};
                    } else {
                        // Parse value as bash array (supports both array and single value)
                        customVars[key] = Utils::parseBashArray(value);
                    }
                    continue;
                }
            }
        }
        
        // Module start [module_name] 必须顶格
        if (!noComment.empty() && noComment[0] == '[') {
            size_t lastNonWS = noComment.find_last_not_of(" \t\r\n");
            if (lastNonWS != std::string::npos && noComment[lastNonWS] == ']') {
                // If there is an unfinished task, save it
                if (hasPendingTask()) {
                    tasks.push_back(currentTask);
                }
                currentTask = ModuleTask();
                std::string inside = noComment.substr(1, lastNonWS - 1);
                currentTask.moduleName = Utils::trim(inside);
                currentTask.useWait = false;
                currentTask.blockIndex = moduleBlockCounters[currentTask.moduleName]++;
                inProcessMode = false;
                inCommandMode = false;
                inPreRawMode = false;
                inRawMode = false;
                continue;
            }
        }
        
        // Parse parameters, post-processing commands, or shell commands
        if (inCommandMode) {
            // This should not be reached, as command mode lines are handled above
            // But keep as fallback
            currentTask.commands.push_back(line);
        } else if (inPreRawMode) {
            // This should not be reached, as pre-raw mode lines are handled above
            // But keep as fallback
            currentTask.preRawCommands.push_back(line);
        } else if (inRawMode) {
            // This should not be reached, as raw mode lines are handled above
            // But keep as fallback
            currentTask.rawCommands.push_back(line);
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
    if (hasPendingTask()) {
        tasks.push_back(currentTask);
    }
    
    file.close();
    return parsed;
}

// Parse inp file, return all module tasks, optional wfn file, core count, custom variables, and header execution flags (backward compatibility)
std::tuple<std::vector<ModuleTask>, std::string, int,
           std::map<std::string, std::vector<std::string>>, bool, bool>
InputParser::parseInpFileWithWfnAndCoresAndVars(const std::string& inpFile) {
    const ParsedInputFile parsed = parseInpFileDetailed(inpFile);
    return {parsed.tasks, parsed.wfnFile, parsed.cores, parsed.customVars, parsed.dryrun, parsed.nogui};
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
