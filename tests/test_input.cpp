#include "doctest.h"

#include "input.h"
#include "test_helpers.h"

#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

class CinRedirectGuard {
public:
    explicit CinRedirectGuard(std::istream& replacement)
        : oldBuffer_(std::cin.rdbuf(replacement.rdbuf())) {}

    CinRedirectGuard(const CinRedirectGuard&) = delete;
    CinRedirectGuard& operator=(const CinRedirectGuard&) = delete;

    ~CinRedirectGuard() {
        std::cin.rdbuf(oldBuffer_);
    }

private:
    std::streambuf* oldBuffer_;
};

} // namespace

TEST_SUITE("InputParser") {

TEST_CASE("parseInpFileWithWfnAndCoresAndVars parses headers modules rebase and anonymous command blocks") {
    TempDir temp;
    const auto inpFile = temp.path() / "workflow.inp";

    writeTextFile(inpFile, R"(wfn=demo.fchk
core=16
dryrun=on
nogui=flase
prefix=final
state=(1 2 3)

[fmo]
index h
%process
orb grid 3
%raw
200
# raw note
wait

wfn_rebase=${prefix}.fchk

%command
echo before
# keep this comment

echo after
end
)");

    auto [tasks, wfnFile, cores, customVars, dryrun, nogui] =
        InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile.string());

    REQUIRE(tasks.size() == 3);
    CHECK(wfnFile == "demo.fchk");
    CHECK(cores == 16);
    CHECK(dryrun);
    CHECK_FALSE(nogui);

    REQUIRE(customVars.count("prefix") == 1);
    REQUIRE(customVars.count("state") == 1);
    const std::vector<std::string> expectedState = {"1", "2", "3"};
    CHECK(customVars.at("prefix").at(0) == "final");
    CHECK(customVars.at("state") == expectedState);

    const auto& moduleTask = tasks[0];
    CHECK(moduleTask.moduleName == "fmo");
    CHECK(moduleTask.blockIndex == 0);
    CHECK(moduleTask.useWait);
    CHECK(moduleTask.params.at("index") == "h");
    REQUIRE(moduleTask.postProcessSteps.size() == 1);
    CHECK(moduleTask.postProcessSteps[0].first == "orb");
    CHECK(moduleTask.postProcessSteps[0].second.at("grid") == "3");
    const std::vector<std::string> expectedRaw = {"200", "# raw note"};
    CHECK(moduleTask.rawCommands == expectedRaw);

    const auto& rebaseTask = tasks[1];
    CHECK(rebaseTask.isWfnRebase);
    CHECK(rebaseTask.wfnRebaseFile == "${prefix}.fchk");

    const auto& commandTask = tasks[2];
    CHECK(commandTask.moduleName.empty());
    CHECK(commandTask.blockIndex == 0);
    CHECK_FALSE(commandTask.useWait);
    const std::vector<std::string> expectedCommands = {
        "echo before",
        "# keep this comment",
        "",
        "echo after"
    };
    CHECK(commandTask.commands == expectedCommands);
}

TEST_CASE("standalone raw and command blocks preserve blank lines comments and wait semantics") {
    TempDir temp;
    const auto inpFile = temp.path() / "standalone.inp";

    writeTextFile(inpFile, R"(wfn=test.fchk
%raw
1

# raw comment
wait
%command
echo hi

# tail
end
)");

    auto [tasks, wfnFile, cores, customVars, dryrun, nogui] =
        InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile.string());

    (void)cores;
    (void)customVars;
    (void)dryrun;
    (void)nogui;

    REQUIRE(tasks.size() == 2);
    CHECK(wfnFile == "test.fchk");

    const auto& rawTask = tasks[0];
    CHECK(rawTask.moduleName.empty());
    CHECK(rawTask.blockIndex == 0);
    CHECK(rawTask.useWait);
    const std::vector<std::string> expectedRaw = {"1", "", "# raw comment"};
    CHECK(rawTask.rawCommands == expectedRaw);

    const auto& commandTask = tasks[1];
    CHECK(commandTask.moduleName.empty());
    CHECK(commandTask.blockIndex == 1);
    CHECK_FALSE(commandTask.useWait);
    const std::vector<std::string> expectedCommands = {"echo hi", "", "# tail"};
    CHECK(commandTask.commands == expectedCommands);
}

TEST_CASE("applyPlaceholderReplacement handles input custom file and reserved output placeholders") {
    TempDir temp;
    temp.chdir();
    writeTextFile(temp.path() / "note", "  file note  \n");

    ModuleTask task;
    task.moduleName = "demo";
    task.params["title"] = "$input";
    task.params["from_file"] = "${note}";
    task.rawCommands = {"open ${wfn}", "keep ${output}"};
    task.commands = {"echo ${prefix}", "echo ${missing}"};

    ModuleTask rebaseTask;
    rebaseTask.isWfnRebase = true;
    rebaseTask.wfnRebaseFile = "${prefix}_${input}_${state}.fchk";

    std::vector<ModuleTask> tasks = {task, rebaseTask};
    const std::map<std::string, std::vector<std::string>> customVars = {
        {"prefix", {"final"}},
        {"state", {"1", "2"}}
    };

    InputParser::applyPlaceholderReplacement(tasks, "/tmp/calc/sample.fchk", customVars);

    CHECK(tasks[0].params.at("title") == "sample");
    CHECK(tasks[0].params.at("from_file") == "file note");
    const std::vector<std::string> expectedRaw = {
        "open /tmp/calc/sample.fchk",
        "keep ${output}"
    };
    CHECK(tasks[0].rawCommands == expectedRaw);
    const std::vector<std::string> expectedCommands = {
        "echo final",
        "echo ${missing}"
    };
    CHECK(tasks[0].commands == expectedCommands);
    CHECK(tasks[1].wfnRebaseFile == "final_sample_1.fchk");
}

TEST_CASE("parseInpFileWithWfnAndCoresAndVars accepts var* and len(var) headers") {
    TempDir temp;
    const auto inpFile = temp.path() / "special_vars.inp";

    writeTextFile(inpFile, R"(wfn=demo.fchk
frag*=?
len(frag)=?

[demo]
%raw
${len(frag)}
${frag*}
end
)");

    auto [tasks, wfnFile, cores, customVars, dryrun, nogui] =
        InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile.string());

    (void)cores;
    (void)dryrun;
    (void)nogui;

    REQUIRE(tasks.size() == 1);
    CHECK(wfnFile == "demo.fchk");
    REQUIRE(customVars.count("frag*") == 1);
    REQUIRE(customVars.count("len(frag)") == 1);
    CHECK(customVars.at("frag*").at(0) == "?");
    CHECK(customVars.at("len(frag)").at(0) == "?");
}

TEST_CASE("resolveInteractiveCustomVars handles paired len(var)=? and var*=? prompts") {
    std::map<std::string, std::vector<std::string>> customVars = {
        {"frag*", {"?"}},
        {"len(frag)", {"?"}}
    };

    std::istringstream fakeInput("3\n10\n20\n30\n");
    CinRedirectGuard cinGuard(fakeInput);

    InputParser::resolveInteractiveCustomVars(customVars);

    REQUIRE(customVars.count("frag*") == 1);
    REQUIRE(customVars.count("len(frag)") == 1);
    CHECK(customVars.at("frag*").at(0) == "(10 20 30)");
    CHECK(customVars.at("len(frag)").at(0) == "3");
    CHECK(customVars.at("frag1").at(0) == "10");
    CHECK(customVars.at("frag2").at(0) == "20");
    CHECK(customVars.at("frag3").at(0) == "30");
}

TEST_CASE("applyPlaceholderReplacement expands var* in raw commands and resolves len(var)") {
    ModuleTask task;
    task.moduleName = "demo";
    task.params["frag*"] = "${frag*}";
    task.params["count"] = "${len(frag)}";
    task.rawCommands = {
        "${len(frag)}",
        "${frag*}",
        "show ${frag*}",
        "keep ${output}"
    };

    std::vector<ModuleTask> tasks = {task};
    std::map<std::string, std::vector<std::string>> customVars = {
        {"frag*", {"(1 3 5)"}}
    };
    InputParser::resolveInteractiveCustomVars(customVars);
    InputParser::applyPlaceholderReplacement(tasks, "/tmp/calc/sample.fchk", customVars);

    CHECK(tasks[0].params.at("frag*") == "(1 3 5)");
    CHECK(tasks[0].params.at("count") == "3");
    const std::vector<std::string> expectedRaw = {
        "3",
        "1",
        "3",
        "5",
        "show 1",
        "show 3",
        "show 5",
        "keep ${output}"
    };
    CHECK(tasks[0].rawCommands == expectedRaw);
}

} // TEST_SUITE("InputParser")
