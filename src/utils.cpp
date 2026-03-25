#include "utils.h"
#include <fstream>
#include <functional>
#include <sstream>
#include <algorithm>
#include <cctype>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <glob.h>
#include <sys/stat.h>
#endif

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string Utils::trimQuotes(const std::string& str) {
    std::string trimmed = trim(str);
    
    // 如果字符串以引号开始和结束，去除引号
    if (trimmed.length() >= 2) {
        char firstChar = trimmed[0];
        char lastChar = trimmed[trimmed.length() - 1];
        
        if ((firstChar == '"' && lastChar == '"') || 
            (firstChar == '\'' && lastChar == '\'')) {
            return trimmed.substr(1, trimmed.length() - 2);
        }
    }
    
    return trimmed;
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    // For whitespace splitting, collapse consecutive whitespace and naturally
    // support spaces / tabs mixed together. This makes input parsing tolerant of
    // accidental extra spaces typed by hand.
    if (std::isspace(static_cast<unsigned char>(delimiter))) {
        while (ss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}
bool Utils::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

bool Utils::validateFile(const std::string& filepath) {
    return fileExists(filepath);
}

std::string Utils::removeInlineComment(const std::string& str) {
    bool inSingleQuote = false;
    bool inDoubleQuote = false;
    std::string result;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];

        // Support escaping '#' outside quotes via \# -> '#'
        if (!inSingleQuote && !inDoubleQuote && c == '\\') {
            if (i + 1 < str.size() && str[i + 1] == '#') {
                result.push_back('#');
                ++i; // skip '#'
                continue;
            }
            // keep backslash if not escaping '#'
            result.push_back(c);
            continue;
        }

        if (!inDoubleQuote && c == '\'') {
            inSingleQuote = !inSingleQuote;
            result.push_back(c);
            continue;
        }
        if (!inSingleQuote && c == '"') {
            inDoubleQuote = !inDoubleQuote;
            result.push_back(c);
            continue;
        }

        // If we see '#' outside quotes, treat as start of comment
        if (!inSingleQuote && !inDoubleQuote && c == '#') {
            break;
        }

        result.push_back(c);
    }

    return trim(result);
}

std::vector<std::string> Utils::expandWildcard(const std::string& pattern) {
    std::vector<std::string> result;
    
    // 检查是否包含通配符
    bool hasWildcard = (pattern.find('*') != std::string::npos || 
                       pattern.find('?') != std::string::npos ||
                       pattern.find('[') != std::string::npos);
    
    if (!hasWildcard) {
        // 没有通配符，直接检查文件是否存在
        if (fileExists(pattern)) {
            result.push_back(pattern);
        }
        return result;
    }
    
    // 使用 glob 函数展开通配符
#ifdef _WIN32
    // Windows 平台使用 FindFirstFile/FindNextFile
    // 提取目录路径
    size_t lastSlash = pattern.find_last_of("/\\");
    std::string dirPath;
    std::string fileNamePattern;
    if (lastSlash != std::string::npos) {
        dirPath = pattern.substr(0, lastSlash + 1);
        fileNamePattern = pattern.substr(lastSlash + 1);
    } else {
        dirPath = "";
        fileNamePattern = pattern;
    }
    
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(pattern.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                // 组合完整路径
                std::string fullPath = dirPath + findData.cFileName;
                result.push_back(fullPath);
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
    std::sort(result.begin(), result.end());
#else
    glob_t globResult;
    int globRet = glob(pattern.c_str(), GLOB_TILDE | GLOB_BRACE, nullptr, &globResult);
    
    if (globRet == 0) {
        for (size_t i = 0; i < globResult.gl_pathc; i++) {
            // 只添加文件，不添加目录
            struct stat st;
            if (stat(globResult.gl_pathv[i], &st) == 0 && S_ISREG(st.st_mode)) {
                result.push_back(globResult.gl_pathv[i]);
            }
        }
        globfree(&globResult);
    } else if (globRet == GLOB_NOMATCH) {
        // 没有匹配的文件，返回空列表
    } else {
        // glob 错误，返回空列表
    }
    
    // 按字母顺序排序
    std::sort(result.begin(), result.end());
#endif
    
    return result;
}

std::vector<std::string> Utils::parseBashArray(const std::string& value) {
    std::vector<std::string> result;
    std::string trimmed = trim(value);
    
    // 检查是否是数组语法: (value1 value2 value3 ...)
    if (trimmed.length() >= 2 && trimmed[0] == '(' && trimmed[trimmed.length() - 1] == ')') {
        // 提取括号内的内容
        std::string inside = trimmed.substr(1, trimmed.length() - 2);
        inside = trim(inside);
        
        if (!inside.empty()) {
            // 按空格分割数组元素
            std::stringstream ss(inside);
            std::string token;
            while (ss >> token) {
                // 去除每个元素的引号（如果存在）
                token = trimQuotes(token);
                result.push_back(token);
            }
        }
    } else {
        // 不是数组语法，返回单个元素
        result.push_back(trimmed);
    }

    return result;
}

std::vector<std::map<std::string, std::vector<std::string>>> Utils::expandVariableCombinations(
    const std::map<std::string, std::vector<std::string>>& vars) {
    std::vector<std::map<std::string, std::vector<std::string>>> combinations;
    std::vector<std::pair<std::string, const std::vector<std::string>*>> arrayVars;

    for (const auto& var : vars) {
        if (var.second.size() > 1) {
            arrayVars.push_back({var.first, &var.second});
        }
    }

    if (arrayVars.empty()) {
        combinations.push_back(vars);
        return combinations;
    }

    std::map<std::string, std::vector<std::string>> currentVars = vars;

    std::function<void(size_t)> buildCombinations = [&](size_t depth) {
        if (depth >= arrayVars.size()) {
            combinations.push_back(currentVars);
            return;
        }

        const std::string& name = arrayVars[depth].first;
        const std::vector<std::string>& values = *arrayVars[depth].second;
        for (const auto& value : values) {
            currentVars[name] = {value};
            buildCombinations(depth + 1);
        }
    };

    buildCombinations(0);
    return combinations;
}

std::vector<std::string> Utils::parseCommandLineArgs(const std::string& argsStr) {
    std::vector<std::string> result;
    std::string currentArg;
    bool inSingleQuote = false;
    bool inDoubleQuote = false;
    bool escaping = false;

    for (size_t i = 0; i < argsStr.length(); ++i) {
        char c = argsStr[i];

        if (escaping) {
            currentArg += c;
            escaping = false;
            continue;
        }

        if (c == '\\') {
            escaping = true;
            continue;
        }

        if (!inDoubleQuote && c == '\'') {
            inSingleQuote = !inSingleQuote;
            continue;
        }

        if (!inSingleQuote && c == '"') {
            inDoubleQuote = !inDoubleQuote;
            continue;
        }

        if (!inSingleQuote && !inDoubleQuote && (c == ' ' || c == '\t')) {
            if (!currentArg.empty()) {
                result.push_back(currentArg);
                currentArg.clear();
            }
            continue;
        }

        currentArg += c;
    }

    if (!currentArg.empty()) {
        result.push_back(currentArg);
    }

    // 如果解析过程中仍有未闭合的引号，说明输入有误
    if (inSingleQuote || inDoubleQuote) {
        // 可以选择抛出异常或返回错误，但这里简单地返回已解析的部分
        // 或者可以记录警告
    }

    return result;
}
