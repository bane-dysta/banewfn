#include "ui.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

void UI::printLogo() {
    std::cout << "\n";
    std::cout << u8"   ____    _    _   _ _______        _______ _   _ \n";
    std::cout << u8"  | __ )  / \\  | \\ | | ____\\ \\      / /  ___| \\ | |\n";
    std::cout << u8"  |  _ \\ / _ \\ |  \\| |  _|  \\ \\ /\\ / /| |_  |  \\| |\n";
    std::cout << u8"  | |_) / ___ \\| |\\  | |___  \\ V  V / |  _| | |\\  |\n";
    std::cout << u8"  |____/_/   \\_\\_| \\_|_____|  \\_/\\_/  |_|   |_| \\_|\n";
    std::cout << u8"  Author: Bane Dysta\n";
    std::cout << u8"  Feedback: https://github.com/bane-dysta/banewfn\n";
    std::cout << u8"  Tips: \n";
    std::cout << u8"  This program only performs a workflow for Multiwfn, so if you use this program for publication,\n";
    std::cout << u8"    you should cite Multiwfn as Prof. Lu mentioned in its manual.\n";
    std::cout << u8"  If you would like to cite this program as well, you can use:\n";  
    std::cout << u8"    Chiyuan Wei, banewfn, Version 1.0, https://github.com/bane-dysta/banewfn (accessed on day month year)\n";   
    std::cout << "\n";
}

std::string UI::requestInputFile() {
    std::string inputFile;
    
    while (true) {
        std::cout << "Bane need an input file: ";
        std::getline(std::cin, inputFile);
        
        // Remove quotes and trim whitespace
        std::string cleanedFile = trimQuotes(inputFile);
        
        if (validateFile(cleanedFile)) {
            return cleanedFile;
        } else {
            printFileError(cleanedFile);
        }
    }
}

std::string UI::requestWavefunctionFile() {
    std::string wfnFile;
    
    while (true) {
        std::cout << "Bane need a wavefunction file (or other file supported by Multiwfn): ";
        std::getline(std::cin, wfnFile);
        
        // Remove quotes and trim whitespace
        std::string cleanedFile = trimQuotes(wfnFile);
        
        if (validateFile(cleanedFile)) {
            return cleanedFile;
        } else {
            printFileError(cleanedFile);
        }
    }
}

bool UI::validateFile(const std::string& filepath) {
    return Utils::validateFile(filepath);
}

void UI::printFileError(const std::string& filepath) {
    std::cout << "Bane is useless, cannot access file '" << filepath << "' TAT\n";
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