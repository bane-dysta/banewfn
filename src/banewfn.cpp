#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <system_error>
#include <unistd.h>
#include <sys/stat.h>
#include "config.h"
#include "input.h"
#include "inline_conf.h"
#include "ui.h"
#include "utils.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#endif

// Utility function: split string (deprecated - use Utils::split instead)
std::vector<std::string> split(const std::string& str, char delimiter) {
    return Utils::split(str, delimiter);
}

static bool isPlainCustomVarName(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    for (char c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    return true;
}

static bool isValidCustomVarName(const std::string& name) {
    if (isPlainCustomVarName(name)) {
        return true;
    }

    if (name.size() > 1 && name.back() == '*') {
        return isPlainCustomVarName(name.substr(0, name.size() - 1));
    }

    return name.size() > 5 && name.rfind("len(", 0) == 0 && name.back() == ')' &&
           isPlainCustomVarName(name.substr(4, name.size() - 5));
}

namespace {

struct TerminalColorState {
    bool enabled = false;
};

TerminalColorState& terminalColorState() {
    static TerminalColorState state;
    return state;
}

bool isNonEmptyEnvEnabled(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && value[0] != '\0';
}

bool isStdStreamTerminal(FILE* stream) {
#ifdef _WIN32
    return _isatty(_fileno(stream)) != 0;
#else
    return isatty(fileno(stream)) != 0;
#endif
}

#ifdef _WIN32
void enableVirtualTerminalForHandle(DWORD stdHandleId) {
    HANDLE handle = GetStdHandle(stdHandleId);
    if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
        return;
    }

    DWORD mode = 0;
    if (!GetConsoleMode(handle, &mode)) {
        return;
    }

    SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

void initializeColorOutput(const ExecutionOptions& options) {
    bool enabled = !options.noColor && !isNonEmptyEnvEnabled("NO_COLOR");

    // Disable colors whenever output is redirected, so logs/files stay clean.
    if (enabled) {
        enabled = isStdStreamTerminal(stdout) && isStdStreamTerminal(stderr);
    }

#ifdef _WIN32
    if (enabled) {
        enableVirtualTerminalForHandle(STD_OUTPUT_HANDLE);
        enableVirtualTerminalForHandle(STD_ERROR_HANDLE);
    }
#endif

    terminalColorState().enabled = enabled;
}

std::string colorizeText(const std::string& text, const char* ansiCode) {
    if (!terminalColorState().enabled) {
        return text;
    }
    return std::string(ansiCode) + text + "\033[0m";
}

void printSuccessLine(const std::string& text) {
    std::cout << colorizeText(text, "\033[32m") << std::endl;
}

void printFailureLine(const std::string& text) {
    std::cerr << colorizeText(text, "\033[31m") << std::endl;
}

} // namespace

// Replace only the specified placeholder variable in a string.
// Supported forms: $var, ${var}, ${var:-default}
// Any other placeholders are left untouched.
static std::string replaceOnePlaceholderVar(const std::string& text,
                                           const std::string& var,
                                           const std::string& value) {
    std::string result = text;
    size_t pos = 0;

    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        std::string varName;
        std::string defaultValue;

        if (endPos < result.size() && result[endPos] == '{') {
            size_t braceStart = endPos + 1;
            size_t braceEnd = result.find('}', braceStart);
            if (braceEnd == std::string::npos) {
                // Malformed placeholder, skip
                pos = endPos + 1;
                continue;
            }

            std::string inside = result.substr(braceStart, braceEnd - braceStart);
            size_t defaultSep = inside.find(":-");
            if (defaultSep != std::string::npos) {
                varName = inside.substr(0, defaultSep);
                defaultValue = inside.substr(defaultSep + 2);
            } else {
                varName = inside;
            }
            endPos = braceEnd + 1;
        } else {
            while (endPos < result.size() && (isalnum((unsigned char)result[endPos]) || result[endPos] == '_')) {
                endPos++;
            }
            varName = result.substr(pos + 1, endPos - pos - 1);
        }

        if (varName == var) {
            std::string repl = value;
            if (repl.empty() && !defaultValue.empty()) {
                repl = defaultValue;
            }
            result.replace(pos, endPos - pos, repl);
            pos += repl.size();
        } else {
            pos = endPos;
        }
    }

    return result;
}

static std::string buildMultiwfnInvocation(const std::string& multiwfnExec,
                                           const std::string& wfnFile,
                                           int cores,
                                           const ExecutionOptions& options) {
    std::stringstream cmd;
    cmd << multiwfnExec << " " << wfnFile;

    if (cores > 0) {
        cmd << " -nt " << cores;
    }

    if (options.nogui) {
        cmd << " -silent";
    }

    for (const auto& arg : options.extargs) {
        cmd << " " << arg;
    }

    return cmd.str();
}

static void pauseIfWindowsDryRun(bool shouldPause) {
#ifdef _WIN32
    if (shouldPause) {
        std::cout << std::flush;
        system("pause");
    }
#else
    (void)shouldPause;
#endif
}

static bool taskHasMultiwfnScript(const ModuleTask& task) {
    return !task.moduleName.empty() || !task.preRawCommands.empty() || !task.rawCommands.empty();
}

static std::string getTaskDisplayName(const ModuleTask& task) {
    if (task.isBuiltin) {
        std::string name = "bane." + task.builtinName;
        if (!task.builtinId.empty()) {
            name += " " + task.builtinId;
        }
        return name;
    }
    if (!task.moduleName.empty()) {
        return task.moduleName;
    }
    if (!task.preRawCommands.empty()) {
        return "%preraw";
    }
    if (!task.rawCommands.empty()) {
        return "%raw";
    }
    if (!task.commands.empty()) {
        return "%command";
    }
    return "<empty>";
}

static std::string getMultiwfnFileStem(const ModuleTask& task, const std::string& wfnFile) {
    std::string stem = task.moduleName;
    if (stem.empty()) {
        stem = task.preRawCommands.empty() ? "raw" : "preraw";
    }

    std::string wfnBaseName = getBaseName(wfnFile);
    if (!wfnBaseName.empty()) {
        stem += "_" + wfnBaseName;
    }

    if (task.blockIndex > 0) {
        stem += "_" + std::to_string(task.blockIndex);
    }

    return stem;
}

static std::string getCommandScriptStem(const ModuleTask& task) {
    std::string stem;

    if (!task.moduleName.empty()) {
        stem = task.moduleName + "_commands";
    } else if (!task.rawCommands.empty()) {
        stem = "raw_commands";
    } else {
        stem = "commands";
    }

    if (task.blockIndex > 0) {
        stem += "_" + std::to_string(task.blockIndex);
    }

    return stem;
}

static std::string toLowerAsciiLocal(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

static bool parseBoolLike(const std::string& rawValue, bool defaultValue = false) {
    const std::string value = toLowerAsciiLocal(Utils::trim(rawValue));
    if (value.empty()) {
        return defaultValue;
    }
    return value == "1" || value == "true" || value == "yes" || value == "on" || value == "y";
}

static std::string sanitizeFileStem(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());
    for (char c : raw) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc) || c == '_' || c == '-' || c == '.') {
            out.push_back(c);
        } else {
            out.push_back('_');
        }
    }
    return out.empty() ? std::string("builtin") : out;
}

static std::string getBuiltinFileStem(const ModuleTask& task, const std::string& wfnFile) {
    std::string stem = "builtin_" + sanitizeFileStem(task.builtinName);
    if (!task.builtinId.empty()) {
        stem += "_" + sanitizeFileStem(task.builtinId);
    }
    const std::string wfnBase = getBaseName(wfnFile);
    if (!wfnBase.empty()) {
        stem += "_" + sanitizeFileStem(wfnBase);
    }
    if (task.blockIndex > 0) {
        stem += "_" + std::to_string(task.blockIndex);
    }
    return stem;
}

static bool getParamCI(const ModuleTask& task, const std::string& key, std::string& value) {
    const std::string wanted = toLowerAsciiLocal(key);
    for (const auto& kv : task.params) {
        if (toLowerAsciiLocal(kv.first) == wanted) {
            value = kv.second;
            return true;
        }
    }
    return false;
}

static std::string getParamCIOr(const ModuleTask& task, const std::vector<std::string>& keys,
                                const std::string& defaultValue = "") {
    for (const auto& key : keys) {
        std::string value;
        if (getParamCI(task, key, value)) {
            return value;
        }
    }
    return defaultValue;
}

static bool validateBuiltinKeys(const ModuleTask& task, const std::set<std::string>& allowedKeys) {
    bool ok = true;
    for (const auto& kv : task.params) {
        const std::string key = toLowerAsciiLocal(kv.first);
        if (allowedKeys.find(key) == allowedKeys.end()) {
            std::cerr << "Error: Unknown key '" << kv.first << "' in " << getTaskDisplayName(task)
                      << ". This builtin uses strict key checking." << std::endl;
            ok = false;
        }
    }
    return ok;
}

static std::string resolveArtifactOrPath(const std::string& raw,
                                         const std::map<std::string, std::string>& artifacts) {
    std::string value = Utils::trimQuotes(Utils::trim(raw));
    const auto it = artifacts.find(value);
    if (it != artifacts.end()) {
        return it->second;
    }
    return value;
}

struct RealSpaceFieldInfo {
    std::string code;
    std::string canonical;
    std::string defaultCubeFile;
    bool needsOrbitalIndex = false;
};

static bool resolveRealSpaceField(const std::string& rawField, RealSpaceFieldInfo& info,
                                  std::string& errorMessage) {
    std::string field = toLowerAsciiLocal(Utils::trim(rawField));
    field.erase(std::remove(field.begin(), field.end(), '-'), field.end());
    for (char& c : field) {
        if (c == ' ') c = '_';
    }

    if (field.empty()) {
        field = "electron_density";
    }

    auto numeric = [](const std::string& text) {
        if (text.empty()) return false;
        size_t pos = 0;
        if (text[0] == '-' || text[0] == '+') pos = 1;
        if (pos >= text.size()) return false;
        for (; pos < text.size(); ++pos) {
            if (!std::isdigit(static_cast<unsigned char>(text[pos]))) return false;
        }
        return true;
    };

    if (numeric(field)) {
        info = {field, field, "", field == "4" || field == "44"};
        return true;
    }

    static const std::map<std::string, RealSpaceFieldInfo> fields = {
        {"rho", {"1", "electron_density", "density.cub", false}},
        {"density", {"1", "electron_density", "density.cub", false}},
        {"electron", {"1", "electron_density", "density.cub", false}},
        {"electron_density", {"1", "electron_density", "density.cub", false}},
        {"laplacian", {"3", "density_laplacian", "", false}},
        {"density_laplacian", {"3", "density_laplacian", "", false}},
        {"laplacian_density", {"3", "density_laplacian", "", false}},
        {"orbital", {"4", "orbital", "", true}},
        {"mo", {"4", "orbital", "", true}},
        {"orbital_density", {"44", "orbital_density", "", true}},
        {"mo_density", {"44", "orbital_density", "", true}},
        {"spin", {"5", "spin_density", "spindensity.cub", false}},
        {"spin_density", {"5", "spin_density", "spindensity.cub", false}},
        {"esp", {"12", "esp", "totesp.cub", false}},
        {"mep", {"12", "esp", "totesp.cub", false}},
        {"electrostatic_potential", {"12", "esp", "totesp.cub", false}},
        {"elf", {"9", "elf", "ELF.cub", false}},
        {"lol", {"10", "lol", "LOL.cub", false}},
        {"alie", {"18", "alie", "", false}},
        {"average_local_ionization_energy", {"18", "alie", "", false}},
    };

    const auto it = fields.find(field);
    if (it == fields.end()) {
        errorMessage = "unsupported real-space field: " + rawField;
        return false;
    }

    info = it->second;
    return true;
}

static void appendFieldSelection(std::vector<std::string>& commands, const RealSpaceFieldInfo& field,
                                 const ModuleTask& task) {
    commands.push_back(field.code);
    if (field.needsOrbitalIndex) {
        const std::string index = getParamCIOr(task, {"orbital", "orbital_index", "index", "mo"}, "h");
        commands.push_back(index);
    }
}

static std::string normalizeGridQuality(const std::string& rawGrid) {
    std::string grid = toLowerAsciiLocal(Utils::trim(rawGrid));
    if (grid.empty()) return "2";
    if (grid == "low" || grid == "coarse" || grid == "preview") return "1";
    if (grid == "medium" || grid == "med" || grid == "normal") return "2";
    if (grid == "high" || grid == "fine") return "3";
    return Utils::trim(rawGrid);
}

static bool appendCubeGridSpec(std::vector<std::string>& commands, const std::string& rawGrid,
                               const std::map<std::string, std::string>& artifacts,
                               std::string& errorMessage) {
    std::string grid = Utils::trim(rawGrid.empty() ? std::string("medium") : rawGrid);
    const std::string lower = toLowerAsciiLocal(grid);
    if (lower.rfind("like(", 0) == 0 && lower.back() == ')') {
        const size_t open = grid.find('(');
        const size_t close = grid.find_last_of(')');
        std::string ref = Utils::trim(grid.substr(open + 1, close - open - 1));
        ref = resolveArtifactOrPath(ref, artifacts);
        if (ref.empty()) {
            errorMessage = "grid=like(...) needs a previous artifact id or cube file path";
            return false;
        }
        commands.push_back("8");
        commands.push_back(ref);
        return true;
    }

    commands.push_back(normalizeGridQuality(grid));
    return true;
}

static std::string normalizeOperationSpec(const std::string& rawOp, std::string& errorMessage) {
    std::string op = Utils::trimQuotes(Utils::trim(rawOp));
    if (op.empty()) {
        errorMessage = "empty wavefunction operation";
        return "";
    }

    if (op.find(',') != std::string::npos) {
        return op;
    }

    std::istringstream iss(op);
    std::string symbol;
    std::string path;
    iss >> symbol;
    std::getline(iss, path);
    path = Utils::trim(path);

    if (symbol.size() == 1 && std::string("+-*/").find(symbol[0]) != std::string::npos && !path.empty()) {
        return symbol + "," + path;
    }

    if (!op.empty() && std::string("+-*/").find(op[0]) != std::string::npos && op.size() > 1) {
        return std::string(1, op[0]) + "," + Utils::trim(op.substr(1));
    }

    errorMessage = "invalid wavefunction operation '" + rawOp + "' (expected e.g. -,fragA.fchk or - fragA.fchk)";
    return "";
}

static std::vector<std::string> parseBuiltinOperations(const ModuleTask& task, std::string& errorMessage) {
    std::vector<std::string> ops;
    for (const auto& bodyLine : task.builtinBody) {
        const size_t eqPos = bodyLine.find('=');
        if (eqPos == std::string::npos) continue;
        std::string value = Utils::trim(bodyLine.substr(eqPos + 1));
        if (value.empty()) continue;

        std::vector<std::string> values;
        const std::string lowerKey = toLowerAsciiLocal(Utils::trim(bodyLine.substr(0, eqPos)));
        if (lowerKey == "ops" || lowerKey == "operators" || lowerKey == "combines") {
            values = Utils::parseBashArray(value);
        } else {
            values = {value};
        }

        for (const auto& rawOp : values) {
            const std::string normalized = normalizeOperationSpec(rawOp, errorMessage);
            if (normalized.empty()) {
                return {};
            }
            ops.push_back(normalized);
        }
    }
    return ops;
}

static bool appendRealSpaceModePrefix(std::vector<std::string>& commands, const ModuleTask& task,
                                      std::string& errorMessage) {
    const std::vector<std::string> ops = parseBuiltinOperations(task, errorMessage);
    if (!errorMessage.empty()) {
        return false;
    }

    std::string mode = toLowerAsciiLocal(getParamCIOr(task, {"mode"}, ""));
    if (mode == "promol" || mode == "promolecular") {
        if (!ops.empty()) {
            errorMessage = "mode=promolecular cannot be combined with op/operator/combine";
            return false;
        }
        commands.push_back("-1");
        return true;
    }
    if (mode == "def" || mode == "deformation" || mode == "deformation_density") {
        if (!ops.empty()) {
            errorMessage = "mode=deformation cannot be combined with op/operator/combine";
            return false;
        }
        commands.push_back("-2");
        return true;
    }

    if (!ops.empty()) {
        commands.push_back("0");
        commands.push_back(std::to_string(ops.size()));
        for (const auto& op : ops) {
            commands.push_back(op);
        }
    }
    return true;
}

static std::vector<std::string> splitSemicolonList(const std::string& raw) {
    std::vector<std::string> parts;
    std::string current;
    for (char c : raw) {
        if (c == ';') {
            parts.push_back(Utils::trim(current));
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty() || raw.find(';') != std::string::npos) {
        parts.push_back(Utils::trim(current));
    }
    return parts;
}

static bool extractFunctionLikeArg(const std::string& raw, const std::string& funcName, std::string& inside) {
    const std::string trimmed = Utils::trim(raw);
    const std::string lower = toLowerAsciiLocal(trimmed);
    const std::string prefix = funcName + "(";
    if (lower.rfind(prefix, 0) != 0 || trimmed.empty() || trimmed.back() != ')') {
        return false;
    }
    inside = Utils::trim(trimmed.substr(prefix.size(), trimmed.size() - prefix.size() - 1));
    return true;
}

static bool appendLineSpec(std::vector<std::string>& commands, const std::string& rawLine,
                           std::string& errorMessage) {
    std::string spec = Utils::trim(rawLine);
    std::string inside;
    if (extractFunctionLikeArg(spec, "atoms", inside) || extractFunctionLikeArg(spec, "atom", inside)) {
        commands.push_back("1");
        commands.push_back(inside);
        return true;
    }
    if (extractFunctionLikeArg(spec, "points", inside) || extractFunctionLikeArg(spec, "point", inside)) {
        const auto pts = splitSemicolonList(inside);
        if (pts.size() != 2 || pts[0].empty() || pts[1].empty()) {
            errorMessage = "line=points(...) expects two semicolon-separated coordinates";
            return false;
        }
        commands.push_back("2");
        commands.push_back(pts[0]);
        commands.push_back(pts[1]);
        return true;
    }

    if (!spec.empty()) {
        // Compatible shorthand: "1,6" means atoms(1,6).
        commands.push_back("1");
        commands.push_back(spec);
        return true;
    }

    errorMessage = "line.profile needs line=atoms(i,j) or line=points(x,y,z;x,y,z)";
    return false;
}

static std::string normalizePlaneGraphType(const std::string& rawGraph) {
    std::string graph = toLowerAsciiLocal(Utils::trim(rawGraph));
    if (graph.empty() || graph == "color" || graph == "filled" || graph == "color_filled" || graph == "map") return "1";
    if (graph == "contour" || graph == "contour_line") return "2";
    if (graph == "relief") return "3";
    if (graph == "shaded" || graph == "shaded_relief") return "4";
    if (graph == "projection" || graph == "shaded_projection") return "5";
    if (graph == "gradient" || graph == "gradient_line") return "6";
    if (graph == "vector" || graph == "vector_field") return "7";
    return Utils::trim(rawGraph);
}

static bool appendPlaneSpec(std::vector<std::string>& commands, const std::string& rawPlane,
                            std::string& errorMessage) {
    std::string spec = Utils::trim(rawPlane);
    std::string inside;
    if (extractFunctionLikeArg(spec, "atoms", inside) || extractFunctionLikeArg(spec, "atom", inside)) {
        commands.push_back("4");
        commands.push_back(inside);
        return true;
    }
    if (extractFunctionLikeArg(spec, "points", inside) || extractFunctionLikeArg(spec, "point", inside)) {
        const auto pts = splitSemicolonList(inside);
        if (pts.size() != 3 || pts[0].empty() || pts[1].empty() || pts[2].empty()) {
            errorMessage = "plane=points(...) expects three semicolon-separated coordinates";
            return false;
        }
        commands.push_back("5");
        commands.push_back(pts[0]);
        commands.push_back(pts[1]);
        commands.push_back(pts[2]);
        return true;
    }
    if (extractFunctionLikeArg(spec, "xy", inside)) {
        commands.push_back("1");
        commands.push_back(inside);
        return true;
    }
    if (extractFunctionLikeArg(spec, "xz", inside)) {
        commands.push_back("2");
        commands.push_back(inside);
        return true;
    }
    if (extractFunctionLikeArg(spec, "yz", inside)) {
        commands.push_back("3");
        commands.push_back(inside);
        return true;
    }

    if (!spec.empty()) {
        // Compatible shorthand: "1,2,3" means atoms(1,2,3).
        commands.push_back("4");
        commands.push_back(spec);
        return true;
    }

    errorMessage = "plane.map needs plane=atoms(i,j,k), plane=points(...), xy(z), xz(y), or yz(x)";
    return false;
}

static bool moveGeneratedFile(const std::string& source, const std::string& destination,
                              bool overwrite, std::string& errorMessage) {
    namespace fs = std::filesystem;
    if (Utils::trim(destination).empty() || source == destination) {
        return true;
    }

    std::error_code ec;
    const fs::path src(source);
    const fs::path dst(destination);
    if (!fs::exists(src, ec) || !fs::is_regular_file(src, ec)) {
        errorMessage = "expected output file not found: " + source;
        return false;
    }

    if (fs::exists(dst, ec)) {
        if (!overwrite) {
            errorMessage = "destination already exists: " + destination + " (set overwrite=true to replace it)";
            return false;
        }
        fs::remove(dst, ec);
        if (ec) {
            errorMessage = "failed to remove existing destination " + destination + ": " + ec.message();
            return false;
        }
    }

    if (!dst.parent_path().empty()) {
        fs::create_directories(dst.parent_path(), ec);
        if (ec) {
            errorMessage = "failed to create output directory for " + destination + ": " + ec.message();
            return false;
        }
    }

    fs::rename(src, dst, ec);
    if (!ec) {
        return true;
    }

    const std::string renameMessage = ec.message();
    ec.clear();
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        errorMessage = renameMessage + "; copy fallback failed: " + ec.message();
        return false;
    }

    fs::remove(src, ec);
    if (ec) {
        errorMessage = "copied but failed to remove original: " + ec.message();
        return false;
    }
    return true;
}

struct BuiltinExecutionPlan {
    std::vector<std::string> commands;
    bool interactive = false;
    std::string primaryDefaultOutput;
    std::string primaryRequestedOutput;
    std::string secondaryDefaultOutput;
    std::string secondaryRequestedOutput;
    std::string artifactPath;
};

class NewFileCollector {
public:
    using Snapshot = std::set<std::string>;

    bool captureSnapshot(Snapshot& files) const {
        files.clear();

        namespace fs = std::filesystem;
        std::error_code ec;
        const fs::path cwd = fs::current_path(ec);
        if (ec) {
            std::cerr << "Warning: collect cannot read current directory: " << ec.message() << std::endl;
            return false;
        }

        fs::directory_iterator it(cwd, fs::directory_options::skip_permission_denied, ec);
        const fs::directory_iterator end;
        if (ec) {
            std::cerr << "Warning: collect cannot scan current directory: " << ec.message() << std::endl;
            return false;
        }

        for (; it != end; it.increment(ec)) {
            if (ec) {
                std::cerr << "Warning: collect stopped scanning current directory: "
                          << ec.message() << std::endl;
                return false;
            }

            std::error_code typeEc;
            if (it->is_regular_file(typeEc)) {
                files.insert(it->path().filename().string());
            }
        }

        return true;
    }

    bool rememberNewFilesSince(const Snapshot& before, const std::string& taskName) {
        Snapshot after;
        if (!captureSnapshot(after)) {
            return false;
        }

        size_t addedCount = 0;
        for (const auto& fileName : after) {
            if (before.find(fileName) == before.end()) {
                if (pendingFiles.insert(fileName).second) {
                    ++addedCount;
                }
            }
        }

        if (addedCount > 0) {
            std::cout << "collect: remembered " << addedCount << " new file(s) after "
                      << taskName << "." << std::endl;
        }

        return true;
    }

    bool flushToDirectory(const std::string& rawTargetDir, bool dryrun) {
        namespace fs = std::filesystem;

        const std::string targetDir = Utils::trim(rawTargetDir);
        if (targetDir.empty()) {
            std::cerr << "Error: collect target directory is empty" << std::endl;
            return false;
        }

        if (dryrun) {
            std::cout << "Dry-run mode: collect(" << targetDir
                      << ") skipped; no files were moved." << std::endl;
            pendingFiles.clear();
            return true;
        }

        if (pendingFiles.empty()) {
            std::cout << "collect(" << targetDir << "): no new files to move." << std::endl;
            return true;
        }

        std::error_code ec;
        const fs::path cwd = fs::current_path(ec);
        if (ec) {
            std::cerr << "Error: collect cannot read current directory: " << ec.message() << std::endl;
            return false;
        }

        const fs::path targetPath(targetDir);
        ec.clear();
        const bool targetExists = fs::exists(targetPath, ec);
        if (ec) {
            std::cerr << "Error: collect cannot inspect target path " << targetDir
                      << ": " << ec.message() << std::endl;
            return false;
        }

        if (targetExists) {
            if (!fs::is_directory(targetPath, ec)) {
                std::cerr << "Error: collect target exists but is not a directory: "
                          << targetDir << std::endl;
                return false;
            }
        } else {
            fs::create_directories(targetPath, ec);
            if (ec) {
                std::cerr << "Error: collect cannot create target directory " << targetDir
                          << ": " << ec.message() << std::endl;
                return false;
            }
        }

        bool ok = true;
        size_t movedCount = 0;
        std::set<std::string> remaining;

        for (const auto& fileName : pendingFiles) {
            const fs::path sourcePath = cwd / fileName;
            const fs::path destPath = targetPath / sourcePath.filename();

            std::error_code fileEc;
            if (!fs::exists(sourcePath, fileEc) || !fs::is_regular_file(sourcePath, fileEc)) {
                std::cerr << "Warning: collect source file disappeared, skipping: "
                          << fileName << std::endl;
                continue;
            }

            fileEc.clear();
            if (fs::equivalent(sourcePath, destPath, fileEc)) {
                std::cerr << "Warning: collect source and destination are the same, skipping: "
                          << fileName << std::endl;
                continue;
            }

            fileEc.clear();
            if (fs::exists(destPath, fileEc)) {
                std::cerr << "Warning: collect destination already exists, keeping source file: "
                          << destPath.string() << std::endl;
                remaining.insert(fileName);
                ok = false;
                continue;
            }

            std::string errorMessage;
            if (moveFile(sourcePath, destPath, errorMessage)) {
                ++movedCount;
            } else {
                std::cerr << "Warning: collect failed to move " << fileName << " to "
                          << destPath.string() << ": " << errorMessage << std::endl;
                remaining.insert(fileName);
                ok = false;
            }
        }

        pendingFiles.swap(remaining);

        if (movedCount > 0) {
            printSuccessLine("collect(" + targetDir + "): moved " +
                             std::to_string(movedCount) + " file(s).");
        } else if (ok) {
            std::cout << "collect(" << targetDir << "): no files moved." << std::endl;
        }

        return ok;
    }

private:
    static bool moveFile(const std::filesystem::path& sourcePath,
                         const std::filesystem::path& destPath,
                         std::string& errorMessage) {
        namespace fs = std::filesystem;

        std::error_code renameEc;
        fs::rename(sourcePath, destPath, renameEc);
        if (!renameEc) {
            return true;
        }

        std::error_code copyEc;
        fs::copy_file(sourcePath, destPath, fs::copy_options::none, copyEc);
        if (copyEc) {
            errorMessage = renameEc.message() + "; copy fallback failed: " + copyEc.message();
            return false;
        }

        std::error_code removeEc;
        fs::remove(sourcePath, removeEc);
        if (removeEc) {
            errorMessage = "copied but failed to remove original: " + removeEc.message();
            return false;
        }

        return true;
    }

    std::set<std::string> pendingFiles;
};

class MultiwfnScriptGenerator {
private:
    ConfigManager configManager;
    
public:
    // Load banewfn.rc configuration file
    bool loadBaneWfnConfig(const std::string& configFile) {
        return configManager.loadBaneWfnConfig(configFile);
    }
    
    // Load module-specific conf file
    bool loadModuleConfig(const std::string& moduleName) {
        return configManager.loadModuleConfig(moduleName);
    }
    
    // Generate command sequence
    std::vector<std::string> generateCommands(const std::string& moduleName,
                                             const std::string& sectionName,
                                             const std::map<std::string, std::string>& params) {
        std::vector<std::string> result;
        
        if (!configManager.hasModuleConfig(moduleName)) {
            std::cerr << "Warning: Module config [" << moduleName << "] not loaded" << std::endl;
            return result;
        }
        
        const ModuleConfig& modConfig = configManager.getModuleConfig(moduleName);
        
        if (modConfig.sections.find(sectionName) == modConfig.sections.end()) {
            std::cerr << "Warning: Section [" << sectionName << "] not found in module " << moduleName << std::endl;
            return result;
        }
        
        const Section& section = modConfig.sections.at(sectionName);
        
        // Merge default values and provided parameters
        // Only override defaults if provided value is non-empty, to mimic ${var:-default}
        std::map<std::string, std::string> finalParams = section.defaults;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                finalParams[param.first] = param.second;
            }
        }
        
        // Generate commands
        for (const auto& cmd : section.commands) {
            auto expanded = replacePlaceholdersExpanded(cmd, finalParams);
            result.insert(result.end(), expanded.begin(), expanded.end());
        }
        
        return result;
    }
    
    // Parse inp file, return all module tasks
    std::vector<ModuleTask> parseInpFile(const std::string& inpFile) {
        return InputParser::parseInpFile(inpFile);
    }
    
    // Parse inp file, return all module tasks and optional wfn file
    std::pair<std::vector<ModuleTask>, std::string> parseInpFileWithWfn(const std::string& inpFile) {
        return InputParser::parseInpFileWithWfn(inpFile);
    }
    
    // Parse inp file, return all module tasks, optional wfn file, and core count
    std::tuple<std::vector<ModuleTask>, std::string, int> parseInpFileWithWfnAndCores(const std::string& inpFile) {
        return InputParser::parseInpFileWithWfnAndCores(inpFile);
    }
    
    // Generate Multiwfn input script for a single task.
    // For normal module tasks, this includes optional %preraw + [main] + %process sections + optional %raw.
    // For raw-only tasks, this is simply the literal %preraw/%raw content.
    std::string generateModuleScript(const ModuleTask& task, bool includeQuit) {
        std::stringstream output;

        bool hasContent = false;
        const ModuleConfig* modConfig = nullptr;

        // Insert literal pre-raw Multiwfn commands before generated module commands.
        for (const auto& preRawCmd : task.preRawCommands) {
            output << preRawCmd << "\n";
            hasContent = true;
        }

        if (!task.moduleName.empty()) {
            if (!configManager.hasModuleConfig(task.moduleName)) {
                std::cerr << "Error: Module config not loaded for " << task.moduleName << std::endl;
                return "";
            }

            modConfig = &configManager.getModuleConfig(task.moduleName);

            // Generate main module commands (pre-processing)
            auto commands = generateCommands(task.moduleName, "main", task.params);
            for (const auto& cmd : commands) {
                output << cmd << "\n";
                hasContent = true;
            }

            // Generate post-processing commands
            for (const auto& step : task.postProcessSteps) {
                auto stepCommands = generateCommands(task.moduleName, step.first, step.second);
                for (const auto& cmd : stepCommands) {
                    output << cmd << "\n";
                    hasContent = true;
                }
            }
        }

        // Append literal raw Multiwfn commands after generated module commands.
        for (const auto& rawCmd : task.rawCommands) {
            output << rawCmd << "\n";
            hasContent = true;
        }

        if (!hasContent) {
            return "";
        }

        // Add module-specific quit commands only if requested.
        // Raw-only tasks are treated as already-finalized literal command sequences.
        if (includeQuit && modConfig != nullptr) {
            for (const auto& quitCmd : modConfig->quitCommands) {
                output << quitCmd << "\n";
            }
        }
        
        return output.str();
    }
    
    // Execute single module Multiwfn task (file-based mode)
    bool executeModuleTaskFile(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\n>>> Processing Multiwfn task: " << getTaskDisplayName(task) << std::endl;
        
        // Generate command script with quit commands
        std::string commands = generateModuleScript(task, true);
        if (commands.empty()) {
            return false;
        }
        
        // Create command file
        std::string cmdFileName = getMultiwfnFileStem(task, wfnFile) + ".txt";
        
        std::ofstream cmdFile(cmdFileName);
        if (!cmdFile.is_open()) {
            std::cerr << "Error: Cannot create command file: " << cmdFileName << std::endl;
            return false;
        }
        cmdFile << commands;
        cmdFile.close();
        
        // In dryrun mode, only generate the file
        if (options.dryrun) {
            std::cout << "Dry-run mode: Command file generated, skipping execution." << std::endl;
            return true;
        }
        
        // Generate output filename or screen output
        std::string outFile;
        if (!options.screen) {
            outFile = getMultiwfnFileStem(task, wfnFile) + ".out";
            
            std::ofstream outFileStream(outFile);
            if (outFileStream.is_open()) {
                outFileStream << UI::getLogoString();
                outFileStream.close();
            }
        }
        
        std::stringstream cmd;
        cmd << buildMultiwfnInvocation(configManager.getConfig().multiwfnExec, wfnFile, cores, options)
            << " < " << cmdFileName;

        if (!options.screen) {
            cmd << " >> " << outFile;
        }

        std::cout << "Executing command: " << cmd.str() << std::endl;
        std::cout << "Starting Multiwfn process..." << std::endl;
        
        // Execute command
        int result = system(cmd.str().c_str());
        
        // Clean up command file only if not in dryrun mode
        if (!options.dryrun) {
            remove(cmdFileName.c_str());
        }
        
        if (result == 0) {
            printSuccessLine("Module " + task.moduleName + " execution completed.");
            return true;
        } else {
            printFailureLine("Error: Module " + task.moduleName +
                             " execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    // Execute single module Multiwfn task (pipe/interactive mode)
    bool executeModuleTaskPipe(const ModuleTask& task, const std::string& wfnFile, 
                               int cores, const ExecutionOptions& options) {
        std::cout << "\nProcessing Multiwfn task: " << getTaskDisplayName(task)
                  << " (interactive mode)" << std::endl;

        // Generate command script without quit commands
        std::string commands = generateModuleScript(task, false);
        if (commands.empty()) {
            return false;
        }
        
        // Parse commands into individual lines (preserve empty lines!)
        std::vector<std::string> cmdLines;
        std::stringstream ss(commands);
        std::string line;
        while (std::getline(ss, line)) {
            // Don't trim or skip empty lines - they are meaningful inputs
            cmdLines.push_back(line);
        }

        std::string multiwfnCmd = buildMultiwfnInvocation(
            configManager.getConfig().multiwfnExec, wfnFile, cores, options);

        // In dryrun mode, show the manual Multiwfn invocation for wait blocks.
        if (options.dryrun) {
            std::cout << "Dry-run mode: wait block will not be executed automatically." << std::endl;
            std::cout << "Please launch Multiwfn with:" << std::endl;
            std::cout << "  " << multiwfnCmd << std::endl;
            if (!cmdLines.empty()) {
                std::cout << "Then enter the following inputs manually:" << std::endl;
                for (const auto& cmdLine : cmdLines) {
                    if (cmdLine.empty()) {
                        std::cout << "  [empty line]" << std::endl;
                    } else {
                        std::cout << "  " << cmdLine << std::endl;
                    }
                }
            }
            return true;
        }
        
        // Build pipe command: cross-platform compatible
        std::stringstream cmd;
        
#ifdef PLATFORM_WINDOWS
        // Windows style: cmd /c "(echo cmd1; echo cmd2; ...; type con) | Multiwfn file"
        cmd << "cmd /c \"(";
        
        for (const auto& cmdLine : cmdLines) {
            std::string trimmedLine = trim(cmdLine);
            if (trimmedLine.empty()) {
                // For empty lines, use echo. to produce empty input
                cmd << "echo. & ";
            } else {
                // For non-empty lines, escape quotes and use echo
                std::string escaped = cmdLine;
                // Escape any existing quotes in the command for Windows
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                cmd << "echo " << escaped << " & ";
            }
        }
        
        cmd << "type con) | " << multiwfnCmd << "\"";
#else
        // Linux style: (echo cmd1; echo cmd2; ...; cat) | Multiwfn file
        cmd << "(";
        
        for (const auto& cmdLine : cmdLines) {
            std::string trimmedLine = trim(cmdLine);
            if (trimmedLine.empty()) {
                // For empty lines, use echo without arguments to produce empty input
                cmd << "echo; ";
            } else {
                // For non-empty lines, escape quotes and use echo
                std::string escaped = cmdLine;
                // Escape any existing quotes in the command
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                cmd << "echo \"" << escaped << "\"; ";
            }
        }
        
        cmd << "cat) | " << multiwfnCmd;
#endif

        std::cout << "Executing command: " << cmd.str() << std::endl;
        std::cout << "Starting Multiwfn in interactive mode...\n" << std::endl;
        
        // Execute command
        int result = system(cmd.str().c_str());
        
        if (result == 0) {
            printSuccessLine("Module " + task.moduleName + " session ended.");
            return true;
        } else {
            printFailureLine("Error: Module " + task.moduleName +
                             " execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    // Execute command block (shell commands)
    // outFile: the Multiwfn output file produced by the preceding module task.
    //          Computed centrally in executeModuleTask and passed in explicitly
    //          so that ${output} is always resolved correctly.
    bool executeCommandBlock(const ModuleTask& task, const std::string& wfnFile,
                             const ExecutionOptions& options,
                             const std::string& outFile = "") {
        if (task.commands.empty()) {
            return true; // No commands to execute
        }
        
        (void)wfnFile;

        std::cout << "\nExecuting command block for task: " << getTaskDisplayName(task) << std::endl;

        // Replace reserved placeholders in command lines (currently: ${output})
        std::vector<std::string> processedCmds;
        processedCmds.reserve(task.commands.size());
        for (const auto& cmdLine : task.commands) {
            processedCmds.push_back(replaceOnePlaceholderVar(cmdLine, "output", outFile));
        }

        // In dryrun mode, only show what would be executed (after placeholder replacement)
        if (options.dryrun) {
            std::cout << "Dry-run mode: Would execute the following commands:" << std::endl;
            for (const auto& cmd : processedCmds) {
                std::cout << "  " << cmd << std::endl;
            }
            return true;
        }
        
        // Create temporary script file
        std::string scriptFileName = getCommandScriptStem(task);
        
#ifdef PLATFORM_WINDOWS
        int result = 0;
        
        // Check if gitbash_exec is configured and first line is shebang #!/bin/bash
        bool useGitBash = false;
        if (!configManager.getConfig().gitbashExec.empty() && !processedCmds.empty()) {
            std::string firstLine = processedCmds[0];
            // Trim whitespace to check for shebang
            std::string trimmedFirstLine = Utils::trim(firstLine);
            if (trimmedFirstLine == "#!/bin/bash" || trimmedFirstLine.find("#!/bin/bash") == 0) {
                useGitBash = true;
            }
        }
        
        if (useGitBash) {
            // Use Git Bash to execute shell script
            scriptFileName += ".sh";
            std::ofstream scriptFile(scriptFileName);
            if (!scriptFile.is_open()) {
                std::cerr << "Error: Cannot create shell script: " << scriptFileName << std::endl;
                return false;
            }
            
            // Write shell commands (including shebang if present)
            for (const auto& cmd : processedCmds) {
                scriptFile << cmd << std::endl;
            }
            scriptFile.close();
            
            // Execute using Git Bash with -c parameter
            // Convert Windows path to absolute path for bash
            char absPath[MAX_PATH];
            DWORD pathLen = GetFullPathNameA(scriptFileName.c_str(), MAX_PATH, absPath, nullptr);
            std::string bashPath;
            if (pathLen > 0 && pathLen < MAX_PATH) {
                bashPath = absPath;
            } else {
                bashPath = scriptFileName;
            }
            // Convert Windows path separators to forward slashes for bash
            size_t pos = 0;
            while ((pos = bashPath.find('\\', pos)) != std::string::npos) {
                bashPath.replace(pos, 1, "/");
                pos += 1;
            }
            
            // Build command string for display
            std::stringstream cmdDisplay;
            cmdDisplay << "\"" << configManager.getConfig().gitbashExec << "\" -c \"bash " << bashPath << "\"";
            std::cout << "Running script with Git Bash: " << cmdDisplay.str() << " ..." << std::endl;
            
            // Use CreateProcess to avoid cmd.exe quote parsing issues
            std::string gitbashPath = configManager.getConfig().gitbashExec;
            std::string commandArg = "bash " + bashPath;
            
            // Prepare command line for CreateProcess
            // When lpApplicationName is specified, lpCommandLine should start with the program name
            // Format: "program_path" -c "bash script_path"
            std::string cmdLine = "\"" + gitbashPath + "\" -c \"" + commandArg + "\"";
            
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            memset(&si, 0, sizeof(si));
            memset(&pi, 0, sizeof(pi));
            si.cb = sizeof(si);
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            
            // CreateProcess requires mutable string for lpCommandLine
            std::vector<char> cmdLineBuf(cmdLine.begin(), cmdLine.end());
            cmdLineBuf.push_back('\0');
            
            BOOL success = CreateProcessA(
                nullptr,
                cmdLineBuf.data(),
                nullptr,
                nullptr,
                TRUE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi
            );
            
            if (success) {
                // Wait for process to complete
                WaitForSingleObject(pi.hProcess, INFINITE);
                DWORD exitCode = 0;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                result = static_cast<int>(exitCode);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            } else {
                std::cerr << "Error: Failed to start Git Bash process. Error code: " << GetLastError() << std::endl;
                result = 1;
            }
            
            // Clean up shell script
            remove(scriptFileName.c_str());
        } else {
            // Use default batch script
            scriptFileName += ".bat";
            std::ofstream scriptFile(scriptFileName);
            if (!scriptFile.is_open()) {
                std::cerr << "Error: Cannot create batch file: " << scriptFileName << std::endl;
                return false;
            }
            
            // Write batch commands
            for (const auto& cmd : processedCmds) {
                scriptFile << cmd << std::endl;
            }
            scriptFile.close();
            
            // Execute batch file
            std::stringstream cmd;
            cmd << "cmd /c \"" << scriptFileName << "\"";
            std::cout << "Running script: " << cmd.str() << " ..." << std::endl;
            
            result = system(cmd.str().c_str());
            
            // Clean up batch file
            remove(scriptFileName.c_str());
        }
        
#else
        scriptFileName += ".sh";
        std::ofstream scriptFile(scriptFileName);
        if (!scriptFile.is_open()) {
            std::cerr << "Error: Cannot create shell script: " << scriptFileName << std::endl;
            return false;
        }
        
        // Write shell script header
        scriptFile << "#!/bin/bash" << std::endl;
        // scriptFile << "set -e" << std::endl; // Exit on error is not necessary
        
        // Write shell commands
        for (const auto& cmd : processedCmds) {
            scriptFile << cmd << std::endl;
        }
        scriptFile.close();
        
        // Make script executable
        chmod(scriptFileName.c_str(), 0755);
        
        // Execute shell script
        std::stringstream cmd;
        cmd << "./" << scriptFileName;
        std::cout << "Running script: " << cmd.str() << " ..." << std::endl;
        
        int result = system(cmd.str().c_str());
        
        // Clean up shell script
        remove(scriptFileName.c_str());
#endif
        
        if (result == 0) {
            printSuccessLine("Command block execution completed.");
            return true;
        } else {
            printFailureLine("Error: Command block execution failed with error code " + std::to_string(result));
            return false;
        }
    }
    
    bool buildBuiltinExecutionPlan(const ModuleTask& task,
                                   const std::map<std::string, std::string>& artifacts,
                                   BuiltinExecutionPlan& plan) {
        plan = BuiltinExecutionPlan();

        const std::string builtinName = toLowerAsciiLocal(Utils::trim(task.builtinName));
        std::string errorMessage;
        RealSpaceFieldInfo field;
        if (!resolveRealSpaceField(getParamCIOr(task, {"field", "type"}, "electron_density"), field, errorMessage)) {
            std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
            return false;
        }

        if (builtinName == "cube.make") {
            const std::set<std::string> allowed = {
                "from", "wfn", "field", "type", "grid", "output", "overwrite", "mode",
                "orbital", "orbital_index", "index", "mo", "wait", "interactive"
            };
            if (!validateBuiltinKeys(task, allowed)) {
                return false;
            }

            const bool forcedInteractive = parseBoolLike(getParamCIOr(task, {"wait", "interactive"}, "false"), false);
            if (forcedInteractive) {
                std::cerr << "Error: bane.cube.make is a batch builtin; use legacy %raw/wait for manual sessions." << std::endl;
                return false;
            }

            plan.commands.push_back("5");
            if (!appendRealSpaceModePrefix(plan.commands, task, errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }
            appendFieldSelection(plan.commands, field, task);
            if (!appendCubeGridSpec(plan.commands, getParamCIOr(task, {"grid"}, "medium"), artifacts, errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }

            plan.primaryDefaultOutput = field.defaultCubeFile;
            plan.primaryRequestedOutput = getParamCIOr(task, {"output"}, "");
            if (plan.primaryDefaultOutput.empty() && !plan.primaryRequestedOutput.empty()) {
                std::cerr << "Error: field " << field.canonical
                          << " does not have a known default cube filename yet; omit output= or use a supported field alias." << std::endl;
                return false;
            }

            if (field.canonical == "esp") {
                // Match the existing grid.conf behaviour: after exporting total ESP cube, convert
                // the value from Hartree/e to kcal/mol via main function 13.
                plan.commands.push_back("2");
                plan.commands.push_back("0");
                plan.commands.push_back("r");
                plan.commands.push_back("totesp.cub");
                plan.commands.push_back("13");
                plan.commands.push_back("11");
                plan.commands.push_back("5");
                plan.commands.push_back("627.51");
                plan.commands.push_back("0");
                plan.commands.push_back("totesp.cub");
                plan.commands.push_back("-1");
                plan.commands.push_back("5");
                plan.commands.push_back("-10");
                plan.commands.push_back("q");
            } else {
                plan.commands.push_back("2");
                plan.commands.push_back("0");
                plan.commands.push_back("5");
                plan.commands.push_back("-10");
                plan.commands.push_back("q");
            }

            if (!plan.primaryRequestedOutput.empty()) {
                plan.artifactPath = plan.primaryRequestedOutput;
            } else if (!plan.primaryDefaultOutput.empty()) {
                plan.artifactPath = plan.primaryDefaultOutput;
            }
            return true;
        }

        if (builtinName == "line.profile") {
            const std::set<std::string> allowed = {
                "from", "wfn", "field", "type", "line", "output", "overwrite", "mode",
                "orbital", "orbital_index", "index", "mo", "wait", "interactive"
            };
            if (!validateBuiltinKeys(task, allowed)) {
                return false;
            }

            plan.commands.push_back("3");
            if (!appendRealSpaceModePrefix(plan.commands, task, errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }
            appendFieldSelection(plan.commands, field, task);
            if (!appendLineSpec(plan.commands, getParamCIOr(task, {"line"}, ""), errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }

            // Main function 3 enters a plotting GUI/menu after calculating the curve; Multiwfn
            // exports line.txt from the right-click/menu workflow. Keep it interactive by design,
            // then rename line.txt if the user exported it during the session.
            plan.interactive = true;
            plan.primaryDefaultOutput = "line.txt";
            plan.primaryRequestedOutput = getParamCIOr(task, {"output"}, "");
            if (!plan.primaryRequestedOutput.empty()) {
                plan.artifactPath = plan.primaryRequestedOutput;
            }
            return true;
        }

        if (builtinName == "plane.map") {
            const std::set<std::string> allowed = {
                "from", "wfn", "field", "type", "plane", "grid", "graph", "graph_type",
                "output", "image", "overwrite", "mode", "orbital", "orbital_index", "index", "mo",
                "wait", "interactive"
            };
            if (!validateBuiltinKeys(task, allowed)) {
                return false;
            }

            plan.commands.push_back("4");
            if (!appendRealSpaceModePrefix(plan.commands, task, errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }
            appendFieldSelection(plan.commands, field, task);
            plan.commands.push_back(normalizePlaneGraphType(getParamCIOr(task, {"graph", "graph_type"}, "color")));
            plan.commands.push_back(getParamCIOr(task, {"grid"}, "200,200"));
            if (!appendPlaneSpec(plan.commands, getParamCIOr(task, {"plane"}, ""), errorMessage)) {
                std::cerr << "Error: " << errorMessage << " in " << getTaskDisplayName(task) << std::endl;
                return false;
            }

            plan.primaryDefaultOutput = "plane.txt";
            plan.primaryRequestedOutput = getParamCIOr(task, {"output"}, "");
            plan.secondaryDefaultOutput = "dislin.png";
            plan.secondaryRequestedOutput = getParamCIOr(task, {"image"}, "");

            const bool requestedInteractive = parseBoolLike(getParamCIOr(task, {"wait", "interactive"}, "false"), false);
            if (requestedInteractive || (plan.primaryRequestedOutput.empty() && plan.secondaryRequestedOutput.empty())) {
                plan.interactive = true;
            } else {
                if (!plan.primaryRequestedOutput.empty()) {
                    plan.commands.push_back("-6");
                }
                if (!plan.secondaryRequestedOutput.empty()) {
                    plan.commands.push_back("0");
                }
                plan.commands.push_back("-10");
                plan.commands.push_back("q");
            }

            if (!plan.primaryRequestedOutput.empty()) {
                plan.artifactPath = plan.primaryRequestedOutput;
            }
            return true;
        }

        std::cerr << "Error: Unknown builtin DSL block bane." << task.builtinName
                  << ". Supported: bane.cube.make, bane.line.profile, bane.plane.map" << std::endl;
        return false;
    }

    std::string generateBuiltinScript(const BuiltinExecutionPlan& plan) const {
        std::stringstream output;
        for (const auto& cmd : plan.commands) {
            output << cmd << "\n";
        }
        return output.str();
    }

    bool executeBuiltinTask(const ModuleTask& task, const std::string& currentWfnFile,
                            int cores, const ExecutionOptions& options,
                            std::map<std::string, std::string>& artifacts) {
        std::cout << "\n>>> Processing builtin task: " << getTaskDisplayName(task) << std::endl;

        std::string sourceWfn = getParamCIOr(task, {"from", "wfn"}, currentWfnFile);
        sourceWfn = resolveArtifactOrPath(sourceWfn, artifacts);
        if (Utils::trim(sourceWfn).empty()) {
            std::cerr << "Error: " << getTaskDisplayName(task)
                      << " has no wavefunction source. Set from=... or wfn=..." << std::endl;
            return false;
        }
        if (!options.dryrun && !Utils::fileExists(sourceWfn)) {
            std::cerr << "Warning: builtin source wavefunction file not found: " << sourceWfn << std::endl;
        }

        BuiltinExecutionPlan plan;
        if (!buildBuiltinExecutionPlan(task, artifacts, plan)) {
            return false;
        }

        const bool overwrite = parseBoolLike(getParamCIOr(task, {"overwrite"}, "false"), false);
        const std::string commandText = generateBuiltinScript(plan);
        if (commandText.empty()) {
            return false;
        }

        if (options.dryrun) {
            std::string cmdFileName = getBuiltinFileStem(task, sourceWfn) + ".txt";
            std::ofstream cmdFile(cmdFileName);
            if (!cmdFile.is_open()) {
                std::cerr << "Error: Cannot create builtin command file: " << cmdFileName << std::endl;
                return false;
            }
            cmdFile << commandText;
            cmdFile.close();

            std::cout << "Dry-run mode: builtin command file generated: " << cmdFileName << std::endl;
            std::cout << "Would run: "
                      << buildMultiwfnInvocation(configManager.getConfig().multiwfnExec, sourceWfn, cores, options)
                      << (plan.interactive ? "  (interactive pipe)" : " < " + cmdFileName) << std::endl;
            if (!task.builtinId.empty() && !plan.artifactPath.empty()) {
                artifacts[task.builtinId] = plan.artifactPath;
            }
            return true;
        }

        bool success = false;
        if (plan.interactive) {
            ModuleTask rawTask;
            rawTask.rawCommands = plan.commands;
            rawTask.useWait = true;
            rawTask.blockIndex = task.blockIndex;
            rawTask.isBuiltin = true;
            rawTask.builtinName = task.builtinName;
            rawTask.builtinId = task.builtinId;
            success = executeModuleTaskPipe(rawTask, sourceWfn, cores, options);
        } else {
            std::string cmdFileName = getBuiltinFileStem(task, sourceWfn) + ".txt";
            std::ofstream cmdFile(cmdFileName);
            if (!cmdFile.is_open()) {
                std::cerr << "Error: Cannot create builtin command file: " << cmdFileName << std::endl;
                return false;
            }
            cmdFile << commandText;
            cmdFile.close();

            std::string outFile;
            if (!options.screen) {
                outFile = getBuiltinFileStem(task, sourceWfn) + ".out";
                std::ofstream outFileStream(outFile);
                if (outFileStream.is_open()) {
                    outFileStream << UI::getLogoString();
                    outFileStream.close();
                }
            }

            std::stringstream cmd;
            cmd << buildMultiwfnInvocation(configManager.getConfig().multiwfnExec, sourceWfn, cores, options)
                << " < " << cmdFileName;
            if (!options.screen) {
                cmd << " >> " << outFile;
            }

            std::cout << "Executing command: " << cmd.str() << std::endl;
            std::cout << "Starting Multiwfn process..." << std::endl;
            int result = system(cmd.str().c_str());
            remove(cmdFileName.c_str());
            success = (result == 0);
            if (success) {
                printSuccessLine("Builtin " + getTaskDisplayName(task) + " execution completed.");
            } else {
                printFailureLine("Error: Builtin " + getTaskDisplayName(task) +
                                 " execution failed with error code " + std::to_string(result));
            }
        }

        if (!success) {
            return false;
        }

        std::string moveError;
        if (!plan.primaryRequestedOutput.empty()) {
            if (!moveGeneratedFile(plan.primaryDefaultOutput, plan.primaryRequestedOutput, overwrite, moveError)) {
                std::cerr << "Error: " << moveError << std::endl;
                return false;
            }
            printSuccessLine("Output written: " + plan.primaryRequestedOutput);
        }
        if (!plan.secondaryRequestedOutput.empty()) {
            moveError.clear();
            if (!moveGeneratedFile(plan.secondaryDefaultOutput, plan.secondaryRequestedOutput, overwrite, moveError)) {
                std::cerr << "Error: " << moveError << std::endl;
                return false;
            }
            printSuccessLine("Output written: " + plan.secondaryRequestedOutput);
        }

        if (!task.builtinId.empty() && !plan.artifactPath.empty()) {
            artifacts[task.builtinId] = plan.artifactPath;
        }
        return true;
    }

    // Execute single module task (dispatch to appropriate method)
    bool executeModuleTask(const ModuleTask& task, const std::string& wfnFile, 
                          int cores, const ExecutionOptions& options) {
        bool success = false;

        // Support command-only task (no Multiwfn task, only %command block)
        if (!taskHasMultiwfnScript(task)) {
            return executeCommandBlock(task, wfnFile, options, /*outFile=*/"");
        }

        // Compute outFile centrally so that ${output} in %command blocks is
        // always resolved to the same path that executeModuleTaskFile writes.
        // Rules mirror executeModuleTaskFile exactly:
        //   - screen mode  → no output file, outFile = ""
        //   - wait mode    → no output file, outFile = ""
        //   - otherwise    → "<task>_<wfnBase>[_<blockIndex>].out"
        std::string outFile;
        if (!options.screen && !task.useWait) {
            outFile = getMultiwfnFileStem(task, wfnFile) + ".out";
        }

        if (task.useWait) {
            success = executeModuleTaskPipe(task, wfnFile, cores, options);
        } else {
            success = executeModuleTaskFile(task, wfnFile, cores, options);
        }
        
        // Execute command block if module execution was successful
        if (success) {
            success = executeCommandBlock(task, wfnFile, options, outFile);
        }
        
        return success;
    }
    
    // Execute all module tasks from an already parsed input file.
    // Keeping the parse result explicit avoids reparsing the same file and makes
    // future input-header additions less error-prone than tuple unpacking.
    bool executeAllTasks(const ParsedInputFile& parsedInput, const std::string& inpFile,
                        const std::string& wfnFile, int cores, const ExecutionOptions& options) {
        if (!parsedInput.loaded) {
            std::cerr << "Error: Input file was not loaded successfully: " << inpFile << std::endl;
            return false;
        }

        std::vector<ModuleTask> tasks = parsedInput.tasks;
        const std::string& inputWfnFile = parsedInput.wfnFile;
        int inputCores = parsedInput.cores;
        const auto& fileVars = parsedInput.customVars;
        bool inputDryrun = parsedInput.dryrun;
        bool inputNogui = parsedInput.nogui;

        ExecutionOptions effectiveOptions = options;
        if (inputDryrun) {
            effectiveOptions.dryrun = true;
        }
        if (inputNogui) {
            effectiveOptions.nogui = true;
        }
        
        // Use wfn file from input file if specified, otherwise use command line argument
        std::string wfnPattern;
        if (!wfnFile.empty()) {
            wfnPattern = wfnFile;          // 命令行/位置参数优先
        } else {
            wfnPattern = inputWfnFile;     // 否则回退到输入文件 wfn=
        }
        
        // 展开通配符
        std::vector<std::string> wfnFiles = Utils::expandWildcard(wfnPattern);
        
        if (wfnFiles.empty()) {
            std::cerr << "Error: No matching wavefunction files found for pattern: " << wfnPattern << std::endl;
            return false;
        }
        
        if (wfnFiles.size() > 1) {
            std::cout << "Found " << wfnFiles.size() << " matching files:" << std::endl;
            for (const auto& f : wfnFiles) {
                std::cout << "  - " << f << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Use core count from input file if specified and no cores provided via command line
        int finalCores = cores;
        if (cores < 0 && inputCores > 0) {
            finalCores = inputCores;
            std::cout << "Using core count from input file: " << finalCores << std::endl;
        }
        
        // Merge with command line variables (command line takes precedence)
        std::map<std::string, std::vector<std::string>> allCustomVars = effectiveOptions.customVars;
        for (const auto& var : fileVars) {
            // Only add if not already set by command line
            if (allCustomVars.find(var.first) == allCustomVars.end()) {
                allCustomVars[var.first] = var.second;
            }
        }

        // Interactive variables: allow defining "var=?" at the top of input file
        // (or via -v/--var var=?) to request the value from user at runtime.
        //
        // Special handling:
        // - var*=?      -> collect var1, var2, ... and later expand ${var*} as multiple lines.
        // - len(var)=?  -> if paired with var*=?, prompt this count first and then ask exactly
        //                  that many varN values. Otherwise it behaves like a normal scalar prompt.
        InputParser::resolveInteractiveCustomVars(allCustomVars);
        
        // Collect array variables and expand them as a Cartesian product.
        std::vector<std::string> arrayVarNames;
        for (const auto& var : allCustomVars) {
            if (var.second.size() > 1) {
                arrayVarNames.push_back(var.first);
            }
        }

        const auto variableCombinations = Utils::expandVariableCombinations(allCustomVars);
        const size_t combinationCount = variableCombinations.size();
        
        if (tasks.empty()) {
            std::cerr << "Error: No modules found in inp file" << std::endl;
            return false;
        }
        
        // Collect all required modules and load configurations
        std::set<std::string> modules;
        for (const auto& task : tasks) {
            if (!task.moduleName.empty()) {
                modules.insert(task.moduleName);
            }
        }
        
        std::cout << "\nRequired modules: ";
        for (const auto& mod : modules) {
            std::cout << mod << " ";
        }
        std::cout << "\n";
        
        if (effectiveOptions.dryrun) {
            std::cout << "\n** DRY-RUN MODE: Only generating command files **\n" << std::endl;
        }
        if (effectiveOptions.screen) {
            std::cout << "\n** SCREEN MODE: Output to screen instead of files **\n" << std::endl;
        }

        // Try to load embedded inline conf blocks (if any)
        std::map<std::string, std::string> inlineConfs = InlineConf::extractInlineConfs(inpFile);
        if (!inlineConfs.empty()) {
            std::cout << "\nDetected inline conf blocks for modules: ";
            for (const auto& kv : inlineConfs) {
                if (!kv.first.empty()) {
                    std::cout << kv.first << " ";
                }
            }
            std::cout << "\n";
        }
        
        for (const auto& mod : modules) {
            auto it = inlineConfs.find(mod);
            if (it != inlineConfs.end() && !it->second.empty()) {
                // Inline conf has higher priority
                if (!configManager.loadModuleConfigFromText(mod, it->second, inpFile + " (inline)")) {
                    std::cerr << "Error: Failed to load inline module config for " << mod << std::endl;
                    return false;
                }
            } else {
                if (!loadModuleConfig(mod)) {
                    std::cerr << "Error: Failed to load module config for " << mod << std::endl;
                    return false;
                }
            }
        }
        
        // 对每个匹配的文件和每个变量组合执行任务
        bool allSuccess = true;
        for (size_t fileIdx = 0; fileIdx < wfnFiles.size(); fileIdx++) {
            std::string finalWfnFile = wfnFiles[fileIdx];

            // 对每个变量组合执行（多个数组变量时取笛卡尔积）
            for (size_t combinationIdx = 0; combinationIdx < combinationCount; combinationIdx++) {
                const auto& currentVars = variableCombinations[combinationIdx];

                // 显示当前执行信息
                if (wfnFiles.size() > 1 || combinationCount > 1) {
                    std::cout << "\n========================================" << std::endl;
                    if (wfnFiles.size() > 1) {
                        std::cout << "Processing file " << (fileIdx + 1) << "/" << wfnFiles.size()
                                  << ": " << finalWfnFile;
                    }
                    if (combinationCount > 1) {
                        if (wfnFiles.size() > 1) std::cout << " | ";
                        std::cout << "Combination " << (combinationIdx + 1) << "/" << combinationCount;
                        if (!arrayVarNames.empty()) {
                            std::cout << " (";
                            for (size_t i = 0; i < arrayVarNames.size(); i++) {
                                const auto& varName = arrayVarNames[i];
                                auto it = currentVars.find(varName);
                                if (i > 0) {
                                    std::cout << ", ";
                                }
                                std::cout << varName << "=";
                                if (it != currentVars.end() && !it->second.empty()) {
                                    std::cout << it->second[0];
                                }
                            }
                            std::cout << ")";
                        }
                    }
                    std::cout << std::endl;
                    std::cout << "========================================\n" << std::endl;
                }

                // 为当前文件和当前变量组合创建任务副本并应用占位符替换
                std::vector<ModuleTask> fileTasks = tasks;
                InputParser::applyPlaceholderReplacement(fileTasks, finalWfnFile, currentVars);

                // Execute each module task in sequence, allowing wfn_rebase directives
                // to switch the file provided to subsequent Multiwfn invocations.
                std::string currentWfnFile = finalWfnFile;
                NewFileCollector collector;
                // Logical artifacts produced by builtin DSL blocks in this file/variable iteration.
                // They allow follow-up blocks such as grid=like(complex_den) to refer to earlier output.
                std::map<std::string, std::string> builtinArtifacts;

                // Track files only for tasks that have a later collect(...)
                // directive. This keeps workflows without collect unchanged and
                // avoids remembering files produced after the final collect.
                std::vector<bool> hasCollectAhead(fileTasks.size(), false);
                bool seenCollectAhead = false;
                for (size_t revIdx = fileTasks.size(); revIdx > 0; --revIdx) {
                    const size_t taskIdx = revIdx - 1;
                    hasCollectAhead[taskIdx] = seenCollectAhead;
                    if (fileTasks[taskIdx].isCollect) {
                        seenCollectAhead = true;
                    }
                }

                for (size_t taskIdx = 0; taskIdx < fileTasks.size(); ++taskIdx) {
                    const auto& task = fileTasks[taskIdx];
                    if (task.isCollect) {
                        if (!collector.flushToDirectory(task.collectDir, effectiveOptions.dryrun)) {
                            allSuccess = false;
                        }
                        continue;
                    }

                    if (task.isWfnRebase) {
                        std::string target = Utils::trim(task.wfnRebaseFile);
                        if (target.empty()) {
                            // Empty rebase target means reset to the original wfn of this iteration.
                            currentWfnFile = finalWfnFile;
                            std::cout << "\n>>> wfn_rebase: reset Multiwfn input to original: " << currentWfnFile << std::endl;
                        } else {
                            currentWfnFile = target;
                            std::cout << "\n>>> wfn_rebase: switch Multiwfn input to: " << currentWfnFile << std::endl;
                        }

                        // Warn if file does not exist (still continue; Multiwfn will report errors).
                        if (!effectiveOptions.dryrun && !Utils::fileExists(currentWfnFile)) {
                            std::cerr << "Warning: wfn_rebase target file not found: " << currentWfnFile << std::endl;
                        }
                        continue;
                    }

                    NewFileCollector::Snapshot beforeFiles;
                    bool shouldTrackNewFiles = hasCollectAhead[taskIdx] && !effectiveOptions.dryrun;
                    if (shouldTrackNewFiles) {
                        shouldTrackNewFiles = collector.captureSnapshot(beforeFiles);
                    }

                    bool taskSuccess = false;
                    if (task.isBuiltin) {
                        taskSuccess = executeBuiltinTask(task, currentWfnFile, finalCores, effectiveOptions, builtinArtifacts);
                    } else {
                        taskSuccess = executeModuleTask(task, currentWfnFile, finalCores, effectiveOptions);
                    }

                    if (!taskSuccess) {
                        allSuccess = false;
                    }

                    if (shouldTrackNewFiles &&
                        !collector.rememberNewFilesSince(beforeFiles, getTaskDisplayName(task))) {
                        allSuccess = false;
                    }
                }
            }
        }
        
        if (allSuccess) {
            std::cout << "\nAll done." << std::endl;
        } else {
            printFailureLine("Some modules execution failed");
        }
        
        return allSuccess;
    }
    
    bool executeAllTasks(const std::string& inpFile, const std::string& wfnFile,
                        int cores, const ExecutionOptions& options) {
        ParsedInputFile parsedInput = InputParser::parseInpFileDetailed(inpFile);
        return executeAllTasks(parsedInput, inpFile, wfnFile, cores, options);
    }

    int getCores() const { return configManager.getCores(); }
};

void printUsage(const char* progName) {
    std::cout << "Hmm... You need some advice? No problem, Bane will help you! :)\n";
    std::cout << "Usage: " << progName << " <input.inp> <molecule.fchk> [options]\n";
    std::cout << "       " << progName << " -w <molecule.fchk> <input.inp> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -l, --list          List available module conf names or show a conf summary\n";
    std::cout << "  -c, --cores <num>   Specify the number of CPU cores to use\n";
    std::cout << "  -d, --dryrun        Generate command files only, don't execute (skip wait tasks)\n";
    std::cout << "  -e, --extargs <args> Pass extra arguments to Multiwfn (use quotes for multiple args)\n";
    std::cout << "      --no-color     Disable colored status output\n";
    std::cout << "  -s, --screen        Display output on screen instead of redirecting to files\n";
    std::cout << "  -n, --nogui         Run Multiwfn in silent mode\n";
    std::cout << "  -w, --wfn <file>    Specify wavefunction file (.fchk/.wfn or other supported file)\n";
    std::cout << "  -v, --var <key=val> Set custom variable for placeholder replacement (can be used multiple times)\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "\nInput header reserved words:\n";
    std::cout << "  wfn=..., core=..., dryrun=on/true, nogui=on/true, wfn_rebase=...\n";
    std::cout << "\nInput block hints:\n";
    std::cout << "  %command ... end    Run shell/batch post-commands\n";
    std::cout << "  %preraw ... end/wait Send literal Multiwfn commands before [main]\n";
    std::cout << "  %raw ... end/wait   Send literal Multiwfn commands\n";
    std::cout << "  collect(dir);       Move files newly created by preceding blocks into dir\n";
    std::cout << "  bane.cube.make id { ... }     Generate cube data via Multiwfn main function 5\n";
    std::cout << "  bane.line.profile id { ... }  Preload an interactive 1D line profile via main function 3\n";
    std::cout << "  bane.plane.map id { ... }     Generate/export a 2D plane map via main function 4\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << progName << " input.inp molecule.fchk\n";
    std::cout << "  " << progName << " -w molecule.fchk input.inp\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -c 8\n";
    std::cout << "  " << progName << " input.inp -w molecule.fchk --dryrun\n";
    std::cout << "  " << progName << " input.inp molecule.fchk --screen\n";
    std::cout << "  " << progName << " -w molecule.fchk input.inp -d -s -c 8\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -v myvar=value -v other=123\n";
    std::cout << "  " << progName << " input.inp molecule.fchk -e \"-silent -nt 4\"\n";
    std::cout << "  # input.inp header: dryrun=on, nogui=on\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Ensure Windows console uses UTF-8 for input/output to avoid garbled ASCII art
    // and other UTF-8 text when double-click launching.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    // Print ASCII logo on startup
    UI::printLogo();
    
    std::string inpFile;
    std::string wfnFile;
    std::string wfnParam;  // Store wfn parameter from -w/--wfn
    int cores = -1;
    ExecutionOptions options;
    
    // Parse command line arguments
    std::vector<std::string> positionalArgs;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-l" || arg == "--list") {
            // List available conf files or show a summary for a specific conf
            std::string moduleArg;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                moduleArg = argv[i + 1];
                i++;
            }

            // Find and load banewfn.rc to get confPath
            std::string configFile = findConfigFile(argv[0]);
            if (configFile.empty()) {
                std::cerr << "Error: Could not find banewfn.rc to determine conf path\n";
                return 1;
            }
            ConfigManager tmpCm;
            if (!tmpCm.loadBaneWfnConfig(configFile)) {
                return 1;
            }
            std::string confDir = tmpCm.getConfig().confPath;

            if (moduleArg.empty()) {
                // List all .conf names under confDir
                std::string pattern = confDir;
                if (!pattern.empty() && pattern.back() != '/') pattern += "/";
                pattern += "*.conf";
                std::vector<std::string> files = Utils::expandWildcard(pattern);
                if (files.empty()) {
                    std::cout << "No .conf files found in: " << confDir << std::endl;
                } else {
                    std::cout << "Available module configs in " << confDir << ":\n";
                    for (const auto &f : files) {
                        std::cout << "  " << getBaseName(f) << std::endl;
                    }
                }
                return 0;
            } else {
                // Show a compact summary for the specified conf
                if (!tmpCm.loadModuleConfig(moduleArg)) {
                    std::cerr << "Error: Failed to load module config for: " << moduleArg << std::endl;
                    return 1;
                }
                const ModuleConfig &mc = tmpCm.getModuleConfig(moduleArg);
                std::cout << "[" << moduleArg << ".conf] summary (section -> variables):\n";
                // Helper to extract variable names from a command string
                auto extractVarsFromCommand = [](const std::string &cmd, std::set<std::string> &outVars) {
                    size_t pos = 0;
                    while (pos < cmd.size()) {
                        size_t dollar = cmd.find('$', pos);
                        if (dollar == std::string::npos) break;
                        size_t next = dollar + 1;
                        if (next < cmd.size() && cmd[next] == '{') {
                            size_t braceEnd = cmd.find('}', next + 1);
                            if (braceEnd != std::string::npos) {
                                std::string inside = cmd.substr(next + 1, braceEnd - next - 1);
                                // support ${var:-default} or ${var:default}
                                size_t sep = inside.find(":-");
                                if (sep == std::string::npos) sep = inside.find(':');
                                std::string varName = (sep == std::string::npos) ? inside : inside.substr(0, sep);
                                if (!varName.empty()) outVars.insert(varName);
                                pos = braceEnd + 1;
                                continue;
                            } else {
                                pos = next + 1;
                                continue;
                            }
                        } else {
                            // $var style
                            size_t j = next;
                            while (j < cmd.size() && (isalnum((unsigned char)cmd[j]) || cmd[j] == '_')) j++;
                            if (j > next) {
                                outVars.insert(cmd.substr(next, j - next));
                                pos = j;
                                continue;
                            } else {
                                pos = next;
                            }
                        }
                    }
                };

                // Show defined sections and list variable names discovered from defaults and commands
                for (const auto &secPair : mc.sections) {
                    const std::string &sectionName = secPair.first;
                    const Section &section = secPair.second;
                    std::set<std::string> vars;
                    // add defaults keys
                    for (const auto &d : section.defaults) {
                        vars.insert(d.first);
                    }
                    // scan commands for $ placeholders
                    for (const auto &cmdLine : section.commands) {
                        extractVarsFromCommand(cmdLine, vars);
                    }

                    // Print section and variables aligned in two columns (first column fixed width)
                    const size_t nameWidth = 12;
                    if (sectionName.length() >= nameWidth) {
                        std::cout << "  " << sectionName;
                        // ensure spacing before vars
                        std::cout << std::string(4, ' ');
                    } else {
                        std::cout << "  " << std::left << std::setw((int)nameWidth) << sectionName << " ";
                        // reset formatting
                        std::cout << std::right;
                    }
                    if (!vars.empty()) {
                        size_t shown = 0;
                        for (const auto &v : vars) {
                            if (shown++) std::cout << "; ";
                            std::cout << v;
                        }
                    }
                    std::cout << "\n";
                }
                // Do not display quit commands in -l summary
                return 0;
            }
        } else if (arg == "-c" || arg == "--cores") {
            if (i + 1 < argc) {
                cores = std::atoi(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Error: -c/--cores requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-d" || arg == "--dryrun") {
            options.dryrun = true;
        } else if (arg == "--no-color") {
            options.noColor = true;
        } else if (arg == "-s" || arg == "--screen") {
            options.screen = true;
        } else if (arg == "-n" || arg == "--nogui") {
            options.nogui = true;
        } else if (arg == "-e" || arg == "--extargs") {
            if (i + 1 < argc) {
                std::string extArgsStr = argv[i + 1];
                // Parse the extargs string, supporting quotes
                std::vector<std::string> parsedArgs = Utils::parseCommandLineArgs(extArgsStr);
                options.extargs.insert(options.extargs.end(), parsedArgs.begin(), parsedArgs.end());
                i++;
            } else {
                std::cerr << "Error: -e/--extargs requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-w" || arg == "--wfn") {
            if (i + 1 < argc) {
                wfnParam = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -w/--wfn requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-v" || arg == "--var") {
            if (i + 1 < argc) {
                std::string varArg = argv[i + 1];
                size_t eqPos = varArg.find('=');
                if (eqPos != std::string::npos && eqPos > 0 && eqPos < varArg.length() - 1) {
                    std::string key = Utils::trim(varArg.substr(0, eqPos));
                    std::string value = Utils::trim(varArg.substr(eqPos + 1));
                    
                    if (isValidCustomVarName(key) && !key.empty()) {
                        if (key.size() > 1 && key.back() == '*') {
                            options.customVars[key] = {value};
                        } else if (key.size() > 5 && key.rfind("len(", 0) == 0 && key.back() == ')') {
                            options.customVars[key] = {value};
                        } else {
                            // Parse value as bash array (supports both array and single value)
                            options.customVars[key] = Utils::parseBashArray(value);
                        }
                    } else {
                        std::cerr << "Warning: Invalid variable name: " << key << std::endl;
                    }
                } else {
                    std::cerr << "Error: -v/--var requires format key=value" << std::endl;
                    return 1;
                }
                i++;
            } else {
                std::cerr << "Error: -v/--var requires an argument" << std::endl;
                return 1;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Warning: Unknown option: " << arg << std::endl;
        } else {
            // This is a positional argument
            positionalArgs.push_back(arg);
        }
    }
    
    // Handle positional arguments
    if (positionalArgs.size() >= 1) {
        inpFile = positionalArgs[0];
    } else {
        inpFile = UI::requestInputFile();
    }
    
    // Check if input file contains wfn definition, core setting, and custom variables
    ParsedInputFile parsedInput = InputParser::parseInpFileDetailed(inpFile);
    if (!parsedInput.loaded) {
        return 1;
    }

    const std::string& inputWfnFile = parsedInput.wfnFile;
    int inputCores = parsedInput.cores;
    const auto& inputVars = parsedInput.customVars;
    bool inputDryrun = parsedInput.dryrun;
    bool inputNogui = parsedInput.nogui;

    if (inputDryrun) {
        options.dryrun = true;
        std::cout << "Dry-run mode enabled by input header." << std::endl;
    }
    if (inputNogui) {
        options.nogui = true;
        std::cout << "No-GUI mode enabled by input header." << std::endl;
    }

    initializeColorOutput(options);

    bool shouldPauseOnExit = options.dryrun;
    
    // Merge input file variables with command line variables (command line takes precedence)
    for (const auto& var : inputVars) {
        if (options.customVars.find(var.first) == options.customVars.end()) {
            options.customVars[var.first] = var.second;
        }
    }
    
    // Priority order: 1) -w/--wfn parameter, 2) positional argument, 3) input file, 4) interactive input
    if (!wfnParam.empty()) {
        wfnFile = wfnParam;
        std::cout << "Using wavefunction file from -w/--wfn parameter: " << wfnFile << std::endl;
    } else if (positionalArgs.size() >= 2) {
        wfnFile = positionalArgs[1];
    } else if (inputWfnFile.empty()) {
        // Only request wfn file if not defined in input file
        wfnFile = UI::requestWavefunctionFile();
    } else {
        // Use wfn file from input file
        wfnFile = inputWfnFile;
        std::cout << "Using wavefunction file from input: " << wfnFile << std::endl;
    }
    
    MultiwfnScriptGenerator generator;
    
    // Search for banewfn.rc
    std::string configFile = findConfigFile(argv[0]);
    
    if (configFile.empty()) {
        std::cerr << "Error: Could not find banewfn.rc in any of the search locations" << std::endl;
        std::cerr << "Please create the config file in one of the following locations:" << std::endl;
        std::cerr << "  - Current directory: ./banewfn.rc" << std::endl;
        std::cerr << "  - Executable directory: <exe_dir>/banewfn.rc" << std::endl;
        std::cerr << "  - Home directory: ~/.bane/wfn/banewfn.rc" << std::endl;
        pauseIfWindowsDryRun(shouldPauseOnExit);
        return 1;
    }
    
    // Load banewfn.rc
    if (!generator.loadBaneWfnConfig(configFile)) {
        pauseIfWindowsDryRun(shouldPauseOnExit);
        return 1;
    }
    
    // If cores not specified, use input file setting or default value from banewfn.rc
    if (cores < 0) {
        if (inputCores > 0) {
            cores = inputCores;
            std::cout << "Using core count from input file: " << cores << std::endl;
        } else {
            cores = generator.getCores();
        }
    }
    
    // Execute all module tasks
    if (!generator.executeAllTasks(parsedInput, inpFile, wfnFile, cores, options)) {
        pauseIfWindowsDryRun(shouldPauseOnExit);
        return 1;
    }

    pauseIfWindowsDryRun(shouldPauseOnExit);
    return 0;
}