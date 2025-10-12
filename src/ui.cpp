#include "ui.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

void UI::printLogo() {
    std::cout << "\n";
    std::cout << "  ██████╗  █████╗ ███╗   ██╗███████╗██╗    ██╗███████╗███╗   ██╗\n";
    std::cout << "  ██╔══██╗██╔══██╗████╗  ██║██╔════╝██║    ██║██╔════╝████╗  ██║\n";
    std::cout << "  ██████╔╝███████║██╔██╗ ██║█████╗  ██║ █╗ ██║█████╗  ██╔██╗ ██║\n";
    std::cout << "  ██╔══██╗██╔══██║██║╚██╗██║██╔══╝  ██║███╗██║██╔══╝  ██║╚██╗██║\n";
    std::cout << "  ██████╔╝██║  ██║██║ ╚████║███████╗╚███╔███╔╝██║     ██║ ╚████║\n";
    std::cout << "  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝ ╚═╝     ╚═╝  ╚═══╝\n";
    std::cout << "\n";
    std::cout << "  ==================Multiwfn Script Generator==================\n";
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
    return Utils::validateFile(filepath);
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
    return Utils::trimQuotes(str);
}