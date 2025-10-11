#ifndef UI_H
#define UI_H

#include <string>

class UI {
public:
    static void printLogo();
    static std::string requestInputFile();
    static std::string requestWavefunctionFile();
    static std::string getUserInput(const std::string& prompt);
    
private:
    static bool validateFile(const std::string& filepath);
    static void printFileError(const std::string& filepath);
    static std::string trimQuotes(const std::string& str);
};

#endif // UI_H
