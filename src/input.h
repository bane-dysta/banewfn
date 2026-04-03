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
    std::vector<std::string> preRawCommands;  // Commands from %preraw block (inserted before [main])
    std::vector<std::string> rawCommands;  // Commands from %raw block (literal Multiwfn input sequence)
    std::vector<std::string> commands;  // Commands from %command block
    bool useWait;  // Whether to use wait mode (interactive mode)
    std::string wfnFile;  // Reserved: optional per-task wavefunction file (currently unused)
    int blockIndex;  // Unique index for blocks with same module name

    // Special directive: wfn_rebase=xxx
    // If isWfnRebase is true, this task will NOT invoke Multiwfn.
    // Instead, it updates the current file provided to subsequent Multiwfn tasks.
    bool isWfnRebase;
    std::string wfnRebaseFile;

    ModuleTask() : useWait(false), blockIndex(0), isWfnRebase(false) {}
};

// Execution options
struct ExecutionOptions {
    bool dryrun;
    bool screen;
    bool nogui;
    bool noColor;
    std::vector<std::string> extargs;  // Extra arguments to pass to Multiwfn
    std::map<std::string, std::vector<std::string>> customVars;  // Custom variables from command line (supports arrays)

    ExecutionOptions() : dryrun(false), screen(false), nogui(false), noColor(false) {}
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
    // Parse inp file, return all module tasks, optional wfn file, core count,
    // custom variables, and whether header dryrun/nogui modes are enabled.
    static std::tuple<std::vector<ModuleTask>, std::string, int,
                      std::map<std::string, std::vector<std::string>>, bool, bool>
    parseInpFileWithWfnAndCoresAndVars(const std::string& inpFile);
    // Resolve interactive custom variables, including special var* / len(var) prompts.
    static void resolveInteractiveCustomVars(std::map<std::string, std::vector<std::string>>& customVars);
    // Apply placeholder replacement to all tasks using wavefunction filename and custom variables
    static void applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile, const std::map<std::string, std::vector<std::string>>& customVars = std::map<std::string, std::vector<std::string>>());
    
private:
    // Utility function: split string
    static std::vector<std::string> split(const std::string& str, char delimiter);
    // Replace input file placeholders ($input and ${input}) with wavefunction filename without extension
    // Also support custom variables from command line or file header
    // customVars: map from variable name to vector of values (for arrays, use first element for replacement)
    static std::string replaceInputPlaceholders(const std::string& text, const std::string& wfnFile, const std::map<std::string, std::vector<std::string>>& customVars = std::map<std::string, std::vector<std::string>>());
};

#endif // INPUT_H
