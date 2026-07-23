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

TEST_CASE("parseInpFileDetailed exposes named fields and load status") {
    TempDir temp;
    const auto inpFile = temp.path() / "named_result.inp";

    writeTextFile(inpFile, R"(wfn=demo.fchk
core=2

[demo]
value 1
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());

    CHECK(parsed.loaded);
    CHECK(parsed.wfnFile == "demo.fchk");
    CHECK(parsed.cores == 2);
    REQUIRE(parsed.tasks.size() == 1);
    CHECK(parsed.tasks[0].moduleName == "demo");

    ParsedInputFile missing = InputParser::parseInpFileDetailed((temp.path() / "missing.inp").string());
    CHECK_FALSE(missing.loaded);
}


TEST_CASE("parse citations_output workflow override without treating it as a custom variable") {
    TempDir temp;
    const auto inpFile = temp.path() / "citation_output.bw";
    writeTextFile(inpFile, R"(citations_output = "${input}_paper.bib"
prefix=paper

bane.cite demo {
    title = "Demo"
}
)");

    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    CHECK(parsed.citationsOutputSpecified);
    CHECK(parsed.citationsOutput == "${input}_paper.bib");
    CHECK(parsed.customVars.count("citations_output") == 0);
    CHECK(parsed.customVars.at("prefix").at(0) == "paper");
}

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
%preraw
1000
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

collect(${prefix}_files);
)");

    auto [tasks, wfnFile, cores, customVars, dryrun, nogui] =
        InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile.string());

    REQUIRE(tasks.size() == 4);
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
    const std::vector<std::string> expectedPreRaw = {"1000"};
    CHECK(moduleTask.preRawCommands == expectedPreRaw);
    REQUIRE(moduleTask.postProcessSteps.size() == 1);
    CHECK(moduleTask.postProcessSteps[0].first == "orb");
    CHECK(moduleTask.postProcessSteps[0].second.at("grid") == "3");
    const std::vector<std::string> expectedRaw = {"200", "# raw note"};
    CHECK(moduleTask.rawCommands == expectedRaw);

    const auto& rebaseTask = tasks[1];
    CHECK(rebaseTask.isWfnRebase());
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

    const auto& collectTask = tasks[3];
    CHECK(collectTask.isCollect());
    CHECK(collectTask.collectDir == "${prefix}_files");
}

TEST_CASE("collect directive follows explicit ended blocks") {
    TempDir temp;
    const auto inpFile = temp.path() / "collect_process.inp";

    writeTextFile(inpFile, R"(wfn=test.fchk
[excit]
%process
    nto state 1
end

%command
#!/bin/bash
echo abc > TEST.txt
end

wfn_rebase=${input}_NTO1.fch

[fmo]
%process
    orb index h
    orb index l
end

collect(NTOs);
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());

    REQUIRE(parsed.tasks.size() == 5);
    CHECK(parsed.tasks[0].moduleName == "excit");
    REQUIRE(parsed.tasks[0].postProcessSteps.size() == 1);
    CHECK(parsed.tasks[0].postProcessSteps[0].first == "nto");

    CHECK(parsed.tasks[1].moduleName.empty());
    REQUIRE(parsed.tasks[1].commands.size() == 2);
    CHECK(parsed.tasks[1].commands[0] == "#!/bin/bash");
    CHECK(parsed.tasks[1].commands[1] == "echo abc > TEST.txt");

    CHECK(parsed.tasks[2].isWfnRebase());

    CHECK(parsed.tasks[3].moduleName == "fmo");
    REQUIRE(parsed.tasks[3].postProcessSteps.size() == 2);
    CHECK(parsed.tasks[3].postProcessSteps[0].first == "orb");
    CHECK(parsed.tasks[3].postProcessSteps[0].second.at("index") == "h");
    CHECK(parsed.tasks[3].postProcessSteps[1].first == "orb");
    CHECK(parsed.tasks[3].postProcessSteps[1].second.at("index") == "l");

    CHECK(parsed.tasks[4].isCollect());
    CHECK(parsed.tasks[4].collectDir == "NTOs");
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
    task.preRawCommands = {"before ${wfn}", "prefix ${prefix}"};
    task.rawCommands = {"open ${wfn}", "keep ${output}"};
    task.commands = {"echo ${prefix}", "echo ${missing}"};

    ModuleTask rebaseTask;
    rebaseTask.kind = TaskKind::WfnRebase;
    rebaseTask.wfnRebaseFile = "${prefix}_${input}_${state}.fchk";

    std::vector<ModuleTask> tasks = {task, rebaseTask};
    ModuleTask collectTask;
    collectTask.kind = TaskKind::Collect;
    collectTask.collectDir = "${prefix}_${input}_dir";
    tasks.push_back(collectTask);
    const std::map<std::string, std::vector<std::string>> customVars = {
        {"prefix", {"final"}},
        {"state", {"1", "2"}}
    };

    InputParser::applyPlaceholderReplacement(tasks, "/tmp/calc/sample.fchk", customVars);

    CHECK(tasks[0].params.at("title") == "sample");
    CHECK(tasks[0].params.at("from_file") == "file note");
    const std::vector<std::string> expectedPreRaw = {
        "before /tmp/calc/sample.fchk",
        "prefix final"
    };
    CHECK(tasks[0].preRawCommands == expectedPreRaw);
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
    CHECK(tasks[2].collectDir == "final_sample_dir");
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
    task.preRawCommands = {
        "pre ${frag*}",
        "count ${len(frag)}"
    };
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
    const std::vector<std::string> expectedPreRaw = {
        "pre 1",
        "pre 3",
        "pre 5",
        "count 3"
    };
    CHECK(tasks[0].preRawCommands == expectedPreRaw);
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


TEST_CASE("parse builtin bane real-space DSL blocks") {
    TempDir temp;
    const auto inpFile = temp.path() / "builtins.inp";

    writeTextFile(inpFile, R"(wfn=complex.fchk
prefix=case1

bane.cube.make complex_den {
    from = ${wfn}
    field = electron_density
    grid = medium
    output = ${prefix}_${input}_den.cub
}

bane.cube.make diff_den {
    from = ${wfn}
    field = rho
    grid = like(complex_den)
    op = -,fragA.fchk
    combine = - fragB.fchk
    output = ${prefix}_${input}_diff.cub
}

bane.line.profile bond_rho {
    field = electron_density
    line = atoms(1,2)
    output = ${prefix}_${input}_bond.txt
}

bane.plane.map ring_elf {
    field = elf
    plane = atoms(1,2,3)
    grid = 160,160
    output = ${prefix}_${input}_plane.txt
}
)");

    auto [tasks, wfnFile, cores, customVars, dryrun, nogui] =
        InputParser::parseInpFileWithWfnAndCoresAndVars(inpFile.string());

    (void)cores;
    (void)dryrun;
    (void)nogui;

    REQUIRE(tasks.size() == 4);
    CHECK(wfnFile == "complex.fchk");
    REQUIRE(customVars.count("prefix") == 1);
    CHECK(customVars.at("prefix").at(0) == "case1");

    CHECK(tasks[0].isBuiltin());
    CHECK(tasks[0].builtinName == "cube.make");
    CHECK(tasks[0].builtinId == "complex_den");
    CHECK(tasks[0].params.at("from") == "${wfn}");
    CHECK(tasks[0].params.at("field") == "electron_density");

    CHECK(tasks[1].isBuiltin());
    CHECK(tasks[1].builtinName == "cube.make");
    CHECK(tasks[1].builtinId == "diff_den");
    REQUIRE(tasks[1].builtinBody.size() == 2);
    CHECK(tasks[1].builtinBody[0] == "op=-,fragA.fchk");
    CHECK(tasks[1].builtinBody[1] == "combine=- fragB.fchk");

    CHECK(tasks[2].isBuiltin());
    CHECK(tasks[2].builtinName == "line.profile");
    CHECK(tasks[2].params.at("line") == "atoms(1,2)");

    CHECK(tasks[3].isBuiltin());
    CHECK(tasks[3].builtinName == "plane.map");
    CHECK(tasks[3].params.at("plane") == "atoms(1,2,3)");

    InputParser::applyPlaceholderReplacement(tasks, "complex.fchk", customVars);
    CHECK(tasks[0].params.at("from") == "complex.fchk");
    CHECK(tasks[0].params.at("output") == "case1_complex_den.cub");
    CHECK(tasks[1].params.at("output") == "case1_complex_diff.cub");
    CHECK(tasks[2].params.at("output") == "case1_complex_bond.txt");
    CHECK(tasks[3].params.at("output") == "case1_complex_plane.txt");
}

TEST_CASE("parse citation declarations and citation output tasks") {
    TempDir temp;
    const auto inpFile = temp.path() / "citations.bw";
    writeTextFile(inpFile, R"(prefix=paper

bane.cite ${prefix}2019 {
    reason = "Equation for ${input}"
    authors = "Alpha One; Beta Two"
    title = "A Citation"
    year = 2019
    doi = "https://doi.org/10.1000/TEST"
}

bane.citations.write refs {
    output = ${input}_references.bib
    format = bibtex
}
)"
    );

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 2);

    CHECK(parsed.tasks[0].isCitation());
    CHECK(parsed.tasks[0].citationId == "${prefix}2019");
    CHECK(parsed.tasks[0].params.at("reason") == "Equation for ${input}");
    CHECK(parsed.tasks[0].origin.find("input:") == 0);

    CHECK(parsed.tasks[1].isCitationOutput());
    CHECK(parsed.tasks[1].citationOutputName == "refs");
    CHECK(parsed.tasks[1].params.at("format") == "bibtex");

    InputParser::applyPlaceholderReplacement(parsed.tasks, "sample.fchk", parsed.customVars);
    CHECK(parsed.tasks[0].citationId == "paper2019");
    CHECK(parsed.tasks[0].params.at("reason") == "Equation for sample");
    CHECK(parsed.tasks[1].params.at("output") == "sample_references.bib");
}


TEST_CASE("citation blocks keep builtin list-like names as ordinary fields") {
    TempDir temp;
    const auto inpFile = temp.path() / "citation_fields.bw";
    writeTextFile(inpFile, R"(bane.cite paper {
    title = "Paper"
    operator = "not a builtin operator"
}
)"
    );

    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 1);
    CHECK(parsed.tasks[0].isCitation());
    CHECK(parsed.tasks[0].builtinBody.empty());
    CHECK(parsed.tasks[0].params.at("operator") == "not a builtin operator");
}

TEST_CASE("invalid core header is ignored instead of silently becoming zero") {
    TempDir temp;
    const auto inpFile = temp.path() / "invalid_core.bw";
    writeTextFile(inpFile, "core=not-a-number\n%raw\nq\nend\n");

    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    CHECK(parsed.cores == -1);
}

TEST_CASE("ModuleTask kind represents exactly one execution category") {
    ModuleTask task;
    CHECK(task.isWorkflow());
    CHECK_FALSE(task.isWfnRebase());
    CHECK_FALSE(task.isCollect());
    CHECK_FALSE(task.isBuiltin());
    CHECK_FALSE(task.isCitation());
    CHECK_FALSE(task.isCitationOutput());

    task.kind = TaskKind::Collect;
    CHECK_FALSE(task.isWorkflow());
    CHECK(task.isCollect());
    CHECK_FALSE(task.isWfnRebase());
    CHECK_FALSE(task.isBuiltin());
    CHECK_FALSE(task.isCitation());
    CHECK_FALSE(task.isCitationOutput());
}


TEST_CASE("only bane.cite creates citation metadata") {
    TempDir temp;
    const auto inpFile = temp.path() / "misspelled_citation.bwc";
    writeTextFile(inpFile, R"(bane.site igmh-paper {
    title = "IGMH Paper"
    reason = "IGMH"
}
)");

    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 1);
    CHECK_FALSE(parsed.tasks[0].isCitation());
    CHECK(parsed.tasks[0].kind == TaskKind::Builtin);
    CHECK(parsed.tasks[0].builtinName == "site");
}

} // TEST_SUITE("InputParser")
