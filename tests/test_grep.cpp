#include "doctest.h"

#include "grep_dsl.h"
#include "grep_engine.h"
#include "input.h"
#include "test_helpers.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

TEST_SUITE("GrepDsl") {

TEST_CASE("InputParser attaches grep rules and lets command terminate the grep block") {
    TempDir temp;
    const auto inpFile = temp.path() / "grep_module.inp";

    writeTextFile(inpFile, R"([charge]
%process
RESP2

%grep
  resp2: inside "Center Charge" .. "Sum of charges" last
    | scan "{atom:int}({element:word}) {charge.resp2:num}"
    | expect rows ${natom}
    | emit atomvec to ${input}_RESP2/RESP2.atomvec.kv

%command
echo ${output}
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 1);

    const ModuleTask& task = parsed.tasks.front();
    CHECK(task.moduleName == "charge");
    REQUIRE(task.grepErrors.empty());
    REQUIRE(task.grepRules.size() == 1);
    REQUIRE(task.commands.size() == 1);

    const GrepRule& rule = task.grepRules.front();
    CHECK(rule.name == "resp2");
    CHECK(rule.selector.kind == GrepSelectorKind::Inside);
    CHECK(rule.selector.occurrence.kind == GrepOccurrenceKind::Last);
    REQUIRE(rule.stages.size() == 2);
    CHECK(rule.stages[0].kind == GrepStageKind::Scan);
    CHECK(rule.stages[0].argument == "{atom:int}({element:word}) {charge.resp2:num}");
    CHECK(rule.stages[1].kind == GrepStageKind::ExpectRows);
    CHECK(rule.stages[1].argument == "${natom}");
    CHECK(rule.sink.kind == GrepSinkKind::AtomVec);
    CHECK(rule.sink.path == "${input}_RESP2/RESP2.atomvec.kv");
    CHECK(task.commands[0] == "echo ${output}");
}


TEST_CASE("input placeholder replacement traverses grep rules but defers output") {
    TempDir temp;
    const auto inpFile = temp.path() / "grep_placeholders.inp";
    writeTextFile(inpFile, R"([charge]
%process
RESP2
%grep
  resp2: from ${output} | inside "${start}" .. "${stop}" last
    | scan "{atom:int}({element:word}) {charge.resp2:num}"
    | expect rows ${natom}
    | emit atomvec to ${input}_RESP2/RESP2.atomvec.kv
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.tasks.size() == 1);
    std::vector<ModuleTask> tasks = parsed.tasks;
    std::map<std::string, std::vector<std::string>> vars{
        {"start", {"Center Charge"}},
        {"stop", {"Sum of charges"}},
        {"natom", {"12"}}
    };
    InputParser::applyPlaceholderReplacement(tasks, "Molecule.fchk", vars);

    REQUIRE(tasks[0].grepRules.size() == 1);
    const GrepRule& rule = tasks[0].grepRules[0];
    CHECK(rule.source == "${output}");
    CHECK(rule.selector.first.value == "Center Charge");
    CHECK(rule.selector.second.value == "Sum of charges");
    REQUIRE(rule.stages.size() == 2);
    CHECK(rule.stages[1].argument == "12");
    CHECK(rule.sink.path == "Molecule_RESP2/RESP2.atomvec.kv");
}

TEST_CASE("grep comment removal preserves hash inside regex") {
    const std::string line = R"(match /value#[0-9]+/ | take 1 -> out.txt # trailing comment)";
    CHECK(GrepDsl::removeInlineComment(line) ==
          R"(match /value#[0-9]+/ | take 1 -> out.txt )");
    CHECK(GrepDsl::removeInlineComment(
              R"(from /tmp/result.out | match /value#[0-9]+/ -> out.txt # comment)") ==
          R"(from /tmp/result.out | match /value#[0-9]+/ -> out.txt )");
}

TEST_CASE("invalid grep syntax is retained as a task error") {
    TempDir temp;
    const auto inpFile = temp.path() / "bad_grep.inp";
    writeTextFile(inpFile, R"(%grep
  broken: between "A" "B" -> out.txt
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.tasks.size() == 1);
    CHECK(parsed.tasks[0].grepRules.empty());
    REQUIRE(parsed.tasks[0].grepErrors.size() == 1);
    CHECK(parsed.tasks[0].grepErrors[0].find("expected '..'") != std::string::npos);
}

TEST_CASE("empty grep block is rejected") {
    TempDir temp;
    const auto inpFile = temp.path() / "empty_grep.inp";
    writeTextFile(inpFile, R"(%grep
# no rules here
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.tasks.size() == 1);
    CHECK(parsed.tasks[0].grepRules.empty());
    REQUIRE(parsed.tasks[0].grepErrors.size() == 1);
    CHECK(parsed.tasks[0].grepErrors[0].find("contains no rules") != std::string::npos);
}

TEST_CASE("grep parser keeps hashes in regex and does not confuse bane-prefixed rule names with builtins") {
    TempDir temp;
    const auto inpFile = temp.path() / "regex_rule.inp";
    writeTextFile(inpFile, R"(%grep
  bane.value: from data.out | match /value#[0-9]+/ -> value.txt # comment
end
)");

    ParsedInputFile parsed = InputParser::parseInpFileDetailed(inpFile.string());
    REQUIRE(parsed.tasks.size() == 1);
    REQUIRE(parsed.tasks[0].grepErrors.empty());
    REQUIRE(parsed.tasks[0].grepRules.size() == 1);
    CHECK(parsed.tasks[0].grepRules[0].name == "bane.value");
    CHECK(parsed.tasks[0].grepRules[0].selector.first.kind == GrepPatternKind::Regex);
    CHECK(parsed.tasks[0].grepRules[0].selector.first.value == "value#[0-9]+");
}

} // TEST_SUITE("GrepDsl")

TEST_SUITE("GrepEngine") {

TEST_CASE("last repeated range scans RESP-style rows and emits atomvec") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "resp.out";
    writeTextFile(source, R"(header
   Center       Charge
     1(O )  -0.1000000000
     2(H )   0.1000000000
 Sum of charges   0.0000000000
middle
   Center       Charge
     1(O )  -0.8539237563
     2(H )   0.4065060026
 Sum of charges  -0.4474177537
)");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "resp2: from " + source.string() +
        " | inside \"Center Charge\" .. \"Sum of charges\" last"
        " | scan \"{atom:int}({element:word}) {charge.resp2:num}\""
        " | expect rows 2 | emit atomvec";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionContext context;
    GrepExecutionResult result = GrepEngine::execute({rule}, context);
    CHECK(result.success);
    REQUIRE(result.outputFiles.size() == 1);
    CHECK(result.outputFiles[0] == "charge.resp2.atomvec.kv");

    const std::string output = readTextFile(temp.path() / "charge.resp2.atomvec.kv");
    CHECK(output.find("atom_count=2\n") == 0);
    CHECK(output.find("atoms.1.element=O\n") != std::string::npos);
    CHECK(output.find("atoms.1.charge.resp2=-0.8539237563\n") != std::string::npos);
    CHECK(output.find("atoms.2.charge.resp2=0.4065060026\n") != std::string::npos);
}

TEST_CASE("between includes markers and automatically creates output directories") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "analysis.out";
    writeTextFile(source, "before\n  Begin   section\nvalue 1\nEnd section\nafter\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "copy: from " + source.string() +
        " | between \"Begin section\" .. \"End section\" -> nested/result.txt";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "nested/result.txt") ==
          "  Begin   section\nvalue 1\nEnd section\n");
}

TEST_CASE("optional missing selection warns without creating an empty output") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "analysis.out";
    writeTextFile(source, "no optional result here\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "optional note: from " + source.string() +
        " | match \"Optional analysis result\" -> optional-note.txt";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(result.skippedOptionalRules == 1);
    CHECK_FALSE(std::filesystem::exists(temp.path() / "optional-note.txt"));
}

TEST_CASE("optional missing source is skipped without creating output") {
    TempDir temp;
    temp.chdir();

    GrepRule rule;
    std::string parseError;
    REQUIRE(GrepDsl::parseRule(
        "optional source: from missing.out | match \"anything\" -> missing.txt",
        1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(result.skippedOptionalRules == 1);
    CHECK_FALSE(std::filesystem::exists(temp.path() / "missing.txt"));
}

TEST_CASE("emit kv writes a typed scalar capture") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "hole.out";
    writeTextFile(source, "Excitation energy of this state:    16.036 eV\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "excitation: from " + source.string() +
        " | match \"Excitation energy of this state\""
        " | scan \"Excitation energy of this state: {excitation.energy:num} eV\""
        " | emit kv";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "excitation.kv") == "excitation.energy=16.036\n");
}

TEST_CASE("cols implicitly splits whitespace tables and projects one-based columns") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "conformers.out";
    writeTextFile(source,
                  "Conformer: confA score 0.0010 relative 0.0020\n"
                  "ignored line\n"
                  "Conformer: confB score 0.0030 relative 0.0040\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "table: from " + source.string() +
        " | match \"Conformer:\" | cols conformer=2 rmse=4 rrmse=6 -> conformers.tsv";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "conformers.tsv") ==
          "confA\t0.0010\t0.0020\nconfB\t0.0030\t0.0040\n");
}

TEST_CASE("jsonl preserves decimal lexemes while normalizing JSON-incompatible numeric syntax") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "numbers.out";
    writeTextFile(source, "value +0001.230000000000000000D+02\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "numbers: from " + source.string() +
        " | match \"value\" | scan \"value {precise:num}\" -> precise.jsonl";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "precise.jsonl") ==
          "{\"precise\":1.230000000000000000E+02}\n");
}

TEST_CASE("first range ignores a later dangling start marker") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "ranges.out";
    writeTextFile(source, "A\ncomplete\nB\nnoise\nA\nincomplete\n");

    GrepRule rule;
    std::string parseError;
    const std::string text =
        "first: from " + source.string() +
        " | between \"A\" .. \"B\" first -> first.txt";
    REQUIRE(GrepDsl::parseRule(text, 1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "first.txt") == "A\ncomplete\nB\n");
}

TEST_CASE("runtime output placeholder can be used as an explicit source") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "module.out";
    writeTextFile(source, "target line\n");

    GrepRule rule;
    std::string parseError;
    REQUIRE(GrepDsl::parseRule(
        "copy: from ${output} | match \"target\" -> selected.txt", 1, rule, parseError));

    GrepExecutionContext context;
    context.outputFile = source.string();
    GrepExecutionResult result = GrepEngine::execute({rule}, context);
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "selected.txt") == "target line\n");
}


TEST_CASE("range occurrences and text stages compose without changing source text") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "sections.out";
    writeTextFile(source,
                  "Header\n"
                  "  A     marker\n"
                  "  one  \n"
                  "skip me\n"
                  " two\n"
                  "End\n"
                  "A marker\n"
                  " three \n"
                  "End\n"
                  "TAIL\n");

    std::string parseError;
    GrepRule ranges;
    REQUIRE(GrepDsl::parseRule(
        "ranges: from " + source.string() +
        " | inside \"A marker\" .. \"End\" all"
        " | trim | reject \"skip\" | drop blank -> ranges.txt",
        1, ranges, parseError));

    GrepRule after;
    REQUIRE(GrepDsl::parseRule(
        "after-last: from " + source.string() +
        " | after \"End\" last | take 1 -> after.txt",
        2, after, parseError));

    GrepRule before;
    REQUIRE(GrepDsl::parseRule(
        "before-exact: from " + source.string() +
        " | before exact \"  A     marker\" | tail 1 -> before.txt",
        3, before, parseError));

    GrepRule regex;
    REQUIRE(GrepDsl::parseRule(
        "regex: from " + source.string() +
        " | match /a\\s+marker/i nth 2 -> regex.txt",
        4, regex, parseError));

    GrepExecutionResult result = GrepEngine::execute(
        {ranges, after, before, regex}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "ranges.txt") == "one\ntwo\nthree\n");
    CHECK(readTextFile(temp.path() / "after.txt") == "TAIL\n");
    CHECK(readTextFile(temp.path() / "before.txt") == "Header\n");
    CHECK(readTextFile(temp.path() / "regex.txt") == "A marker\n");
}

TEST_CASE("split cols and field expectations support negative indices") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "table.out";
    writeTextFile(source, "row,a,1\nrow,b,2\n");

    GrepRule rule;
    std::string parseError;
    REQUIRE(GrepDsl::parseRule(
        "table: from " + source.string() +
        " | match /row,/ | split \",\" | cols label=2 value=-1"
        " | expect rows >= 2 | expect unique label"
        " | expect contiguous value | expect finite value -> table.csv",
        1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "table.csv") == "a,1\nb,2\n");
}

TEST_CASE("strict scan failure preserves an existing sink and stops later rules") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "strict.out";
    writeTextFile(source, "1\nbad\n");
    writeTextFile(temp.path() / "keep.txt", "previous\n");

    std::string parseError;
    GrepRule strictRule;
    REQUIRE(GrepDsl::parseRule(
        "strict: from " + source.string() +
        " | match /.+/ | scan strict \"{value:int}\" -> keep.txt",
        1, strictRule, parseError));
    GrepRule laterRule;
    REQUIRE(GrepDsl::parseRule(
        "later: from " + source.string() + " | match \"1\" -> later.txt",
        2, laterRule, parseError));

    GrepExecutionResult result = GrepEngine::execute(
        {strictRule, laterRule}, GrepExecutionContext());
    CHECK_FALSE(result.success);
    CHECK(result.completedRules == 0);
    CHECK(readTextFile(temp.path() / "keep.txt") == "previous\n");
    CHECK_FALSE(std::filesystem::exists(temp.path() / "later.txt"));
}

TEST_CASE("atomvec explicit property accepts value fields and generates atom indices") {
    TempDir temp;
    temp.chdir();
    const auto source = temp.path() / "values.out";
    writeTextFile(source, "O -1.0D-1\nH 1.0D-1\n");

    GrepRule rule;
    std::string parseError;
    REQUIRE(GrepDsl::parseRule(
        "charges: from " + source.string() +
        " | match /.+/ | scan \"{element:word} {value:num}\""
        " | emit atomvec charge.test to generated.atomvec.kv",
        1, rule, parseError));

    GrepExecutionResult result = GrepEngine::execute({rule}, GrepExecutionContext());
    CHECK(result.success);
    CHECK(readTextFile(temp.path() / "generated.atomvec.kv") ==
          "atom_count=2\n"
          "atoms.1.element=O\n"
          "atoms.1.charge.test=-1.0D-1\n"
          "atoms.2.element=H\n"
          "atoms.2.charge.test=1.0D-1\n");
}

} // TEST_SUITE("GrepEngine")
