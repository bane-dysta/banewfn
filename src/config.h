#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <map>
#include <vector>

// Platform detection
#ifdef _WIN32
    #define PLATFORM_WINDOWS
#else
    #define PLATFORM_LINUX
#endif

// Section structure
struct Section {
    std::vector<std::string> commands;
    std::map<std::string, std::string> defaults;
};

// Module configuration structure
struct ModuleConfig {
    std::map<std::string, Section> sections;
    std::vector<std::string> quitCommands;  // Quit command sequence
};

// Global configuration structure
struct BaneWfnConfig {
    std::string multiwfnExec;
    std::string confPath;
    int cores;
};

// Utility functions
std::string trim(const std::string& str);
std::string expandPath(const std::string& path);
std::string getExecutableDir();
bool fileExists(const std::string& filepath);
std::string findConfigFile(const std::string& executablePath);
std::string getBaseName(const std::string& filepath);
std::string replacePlaceholders(const std::string& cmd, 
                               const std::map<std::string, std::string>& params);

// Configuration manager class
class ConfigManager {
private:
    std::map<std::string, ModuleConfig> moduleConfigs;
    BaneWfnConfig config;
    
public:
    // Load banewfn.rc configuration file
    bool loadBaneWfnConfig(const std::string& configFile);
    
    // Load module-specific conf file
    bool loadModuleConfig(const std::string& moduleName);
    
    // Get configuration values
    const BaneWfnConfig& getConfig() const { return config; }
    const ModuleConfig& getModuleConfig(const std::string& moduleName) const;
    bool hasModuleConfig(const std::string& moduleName) const;
    
    // Get cores setting
    int getCores() const { return config.cores; }
};

#endif // CONFIG_H
