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
    // Parse inp file, return all module tasks, optional wfn file, and core count
    static std::tuple<std::vector<ModuleTask>, std::string, int> parseInpFileWithWfnAndCores(const std::string& inpFile);
    // Apply placeholder replacement to all tasks using wavefunction filename
    static void applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile);
    
private:
    // Utility function: split string
    static std::vector<std::string> split(const std::string& str, char delimiter);
    // Replace input file placeholders ($input and ${input}) with wavefunction filename without extension
    static std::string replaceInputPlaceholders(const std::string& text, const std::string& wfnFile);
};

#endif // INPUT_H
