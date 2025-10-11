#ifndef INPUT_H
#define INPUT_H
#include <string>
#include <map>
#include <vector>

// Single module task information
struct ModuleTask {
    std::string moduleName;
    std::map<std::string, std::string> params;
    std::vector<std::pair<std::string, std::map<std::string, std::string>>> postProcessSteps;
    std::vector<std::string> commands;  // Commands from %command block
    bool useWait;  // Whether to use wait mode (interactive mode)
    std::string wfnFile;  // Wavefunction file path (optional, from input file header)
    int blockIndex;  // Unique index for blocks with same module name
};

// Execution options
struct ExecutionOptions {
    bool dryrun;
    bool screen;
    
    ExecutionOptions() : dryrun(false), screen(false) {}
};

// Input parser class
class InputParser {
public:
    // Parse inp file, return all module tasks
    static std::vector<ModuleTask> parseInpFile(const std::string& inpFile);
    // Parse inp file, return all module tasks and optional wfn file
    static std::pair<std::vector<ModuleTask>, std::string> parseInpFileWithWfn(const std::string& inpFile);
    
private:
    // Utility function: split string
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif // INPUT_H
