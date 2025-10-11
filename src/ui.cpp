#include "ui.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

void UI::printLogo() {
    std::cout << "\n";
    std::cout << "    ____    ___    _   __________       __________ _   __\n";
    std::cout << "   / __ )  /   |  / | / / ____/ |     / / ____/ /| | / /\n";
    std::cout << "  / __  | / /| | /  |/ / __/  | | /| / / /_  / //  |/ / \n";
    std::cout << " / /_/ / / ___ |/ /|  / /___  | |/ |/ / __/ / // /|  /  \n";
    std::cout << "/_____/ /_/  |_/_/ |_/_____/  |__/|__/_/   /_//_/ |_/   \n";
    std::cout << "\n";
    std::cout << "                    Multiwfn Script Generator v1.0\n";
    std::cout << "                    ===============================\n";
    std::cout << "\n";
}

std::string UI::requestInputFile() {
    std::string inputFile;
    
    while (true) {
        std::cout << "Please enter input file path: ";
        std::getline(std::cin, inputFile);
        
        // Remove quotes and trim whitespace
        std::string cleanedFile = trimQuotes(inputFile);
        
        if (validateFile(cleanedFile)) {
            return cleanedFile;
        } else {
            printFileError(cleanedFile);
            std::cout << "Please re-enter a valid file path.\n";
        }
    }
}

std::string UI::requestWavefunctionFile() {
    std::string wfnFile;
    
    while (true) {
        std::cout << "Please enter wavefunction file path (.fchk/.wfn or other supported file): ";
        std::getline(std::cin, wfnFile);
        
        // Remove quotes and trim whitespace
        std::string cleanedFile = trimQuotes(wfnFile);
        
        if (validateFile(cleanedFile)) {
            return cleanedFile;
        } else {
            printFileError(cleanedFile);
            std::cout << "Please re-enter a valid file path.\n";
        }
    }
}

bool UI::validateFile(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

void UI::printFileError(const std::string& filepath) {
    std::cout << "Error: Cannot access file '" << filepath << "'\n";
}

std::string UI::getUserInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

std::string UI::trimQuotes(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    
    size_t start = 0;
    size_t end = str.length();
    
    // Trim leading whitespace
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    // Trim trailing whitespace
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    // If string is empty after trimming whitespace, return empty
    if (start >= end) {
        return "";
    }
    
    // Check for quotes at the beginning and end
    if ((str[start] == '"' && str[end - 1] == '"') || 
        (str[start] == '\'' && str[end - 1] == '\'')) {
        start++;
        end--;
    }
    
    return str.substr(start, end - start);
}