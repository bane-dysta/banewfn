#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>

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
    size_t commentPos = str.find('#');
    if (commentPos != std::string::npos) {
        return trim(str.substr(0, commentPos));
    }
    return str;
}
