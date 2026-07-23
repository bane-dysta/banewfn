#include "ui.h"
#include "utils.h"
#include <iostream>
#include <string>

void UI::printLogo() {
    std::cout << getLogoString();
}

std::string UI::getLogoString() {
    std::string logo;
    logo += "\n";
    logo += u8"   ____    _    _   _ _______        _______ _   _ \n";
    logo += u8"  | __ )  / \\  | \\ | | ____\\ \\      / /  ___| \\ | |\n";
    logo += u8"  |  _ \\ / _ \\ |  \\| |  _|  \\ \\ /\\ / /| |_  |  \\| |\n";
    logo += u8"  | |_) / ___ \\| |\\  | |___  \\ V  V / |  _| | |\\  |\n";
    logo += u8"  |____/_/   \\_\\_| \\_|_____|  \\_/\\_/  |_|   |_| \\_|\n";
    logo += u8"  Author: Bane Dysta\n";
    logo += u8"  Version: 1.5.5\n";
    logo += u8"  Feedback: https://github.com/bane-dysta/banewfn\n";
    logo += u8"  Tips: \n";
    logo += u8"  Available citation suggestions for BaneWfn, Multiwfn, and declared methods\n";
    logo += u8"    are printed after workflow execution.\n";
    logo += u8"    \nLet's start Multiwfn analysis!\n";   
    logo += "\n";
    return logo;
}

std::string UI::requestInputFile() {
    std::string inputFile;
    
    while (true) {
        std::cout << "Bane need an input file: ";
        std::getline(std::cin, inputFile);
        
        // Remove quotes and trim whitespace
        std::string cleanedFile = Utils::trimQuotes(inputFile);
        
        if (Utils::fileExists(cleanedFile)) {
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
        std::string cleanedFile = Utils::trimQuotes(wfnFile);
        
        if (Utils::fileExists(cleanedFile)) {
            return cleanedFile;
        } else {
            printFileError(cleanedFile);
        }
    }
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
