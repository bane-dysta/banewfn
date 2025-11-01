#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
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
