#include "doctest.h"

#include "bwpack_support.h"
#include "citation.h"
#include "config.h"
#include "input.h"
#include "test_helpers.h"

#include <iostream>
#include <sstream>
#include <string>

TEST_SUITE("Bwpack citation expansion") {

TEST_CASE("catalog-backed bane.cite blocks become complete inline records") {
    TempDir temp;
    const auto catalogPath = temp.path() / "citations.conf";
    writeTextFile(catalogPath, R"([citation paper]
authors = "Alice Example; Bob Example"
title = "Catalog title"
journal = "Journal of Tests"
year = 2026
volume = 12
issue = 3
pages = "10-20"
doi = "https://doi.org/10.1234/TEST.1"
url = "https://example.test/paper"
)");

    CitationManager catalog;
    REQUIRE(catalog.loadCatalog(catalogPath.string()));

    const std::string input = R"(wfn=test.fchk

bane.cite paper {
    reason = "Used for the test equation"
    title = "Inline title"
}

[weak]
end
)";

    const auto sourceInput = temp.path() / "workflow.bw";
    writeTextFile(sourceInput, input);
    const ParsedInputFile sourceParsed = InputParser::parseInpFileDetailed(sourceInput.string());
    REQUIRE(sourceParsed.loaded);

    const auto expanded = Bwpack::expandCatalogCitations(
        input, catalog, sourceParsed.tasks, "workflow.bw");
    REQUIRE(expanded.success);
    CHECK(expanded.expandedCount == 1);
    CHECK(expanded.content.find("reason = \"Used for the test equation\"") != std::string::npos);
    CHECK(expanded.content.find("authors = \"Alice Example; Bob Example\"") != std::string::npos);
    CHECK(expanded.content.find("title = \"Inline title\"") != std::string::npos);
    CHECK(expanded.content.find("journal = \"Journal of Tests\"") != std::string::npos);
    CHECK(expanded.content.find("doi = \"10.1234/test.1\"") != std::string::npos);

    const auto packedInput = temp.path() / "expanded.bw";
    writeTextFile(packedInput, expanded.content);
    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(packedInput.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 2);
    REQUIRE(parsed.tasks[0].isCitation());
    CHECK(parsed.tasks[0].citationId == "paper");
    CHECK(parsed.tasks[0].params.at("title") == "Inline title");
    CHECK(parsed.tasks[0].params.at("journal") == "Journal of Tests");
}

TEST_CASE("missing catalog records fail only for catalog-dependent citations") {
    CitationManager emptyCatalog;

    const std::string reasonOnly = R"(bane.cite missing {
    reason = "Needed here"
}
)";
    std::ostringstream capturedErrors;
    auto* oldBuffer = std::cerr.rdbuf(capturedErrors.rdbuf());
    ModuleTask missingTask;
    missingTask.kind = TaskKind::Citation;
    missingTask.citationId = "missing";
    missingTask.params["reason"] = "Needed here";
    missingTask.origin = "input:missing.bw:1";
    const std::vector<ModuleTask> missingTasks = {missingTask};
    const auto missing = Bwpack::expandCatalogCitations(
        reasonOnly, emptyCatalog, missingTasks, "missing.bw");
    std::cerr.rdbuf(oldBuffer);

    CHECK_FALSE(missing.success);
    CHECK(missing.expandedCount == 0);
    CHECK(missing.content == reasonOnly);
    CHECK(capturedErrors.str().find("no matching record was found in citations.conf") != std::string::npos);

    const std::string partialInline = R"(bane.cite partial {
    doi = "10.1234/partial"
}
)";
    ModuleTask partialTask;
    partialTask.kind = TaskKind::Citation;
    partialTask.citationId = "partial";
    partialTask.params["doi"] = "10.1234/partial";
    partialTask.origin = "input:partial.bw:1";
    const std::vector<ModuleTask> partialTasks = {partialTask};
    capturedErrors.str("");
    capturedErrors.clear();
    oldBuffer = std::cerr.rdbuf(capturedErrors.rdbuf());
    const auto partial = Bwpack::expandCatalogCitations(
        partialInline, emptyCatalog, partialTasks, "partial.bw");
    std::cerr.rdbuf(oldBuffer);
    CHECK_FALSE(partial.success);
    CHECK(partial.expandedCount == 0);
    CHECK(partial.content == partialInline);

    const std::string selfContained = R"(bane.cite local {
    title = "Already inline"
    year = 2026
}
)";
    ModuleTask localTask;
    localTask.kind = TaskKind::Citation;
    localTask.citationId = "local";
    localTask.params["title"] = "Already inline";
    localTask.params["year"] = "2026";
    localTask.origin = "input:local.bw:1";
    const std::vector<ModuleTask> localTasks = {localTask};
    const auto unchanged = Bwpack::expandCatalogCitations(
        selfContained, emptyCatalog, localTasks, "local.bw");
    CHECK(unchanged.success);
    CHECK(unchanged.expandedCount == 0);
    CHECK(unchanged.content == selfContained);
}


TEST_CASE("citation-like text inside command blocks is not rewritten") {
    TempDir temp;
    const auto catalogPath = temp.path() / "citations.conf";
    writeTextFile(catalogPath, R"([citation paper]
title = "Catalog title"
year = 2026
)");

    CitationManager catalog;
    REQUIRE(catalog.loadCatalog(catalogPath.string()));

    const std::string input = R"(%command
bane.cite paper {
    reason = "This is shell text"
}
end
)";
    const auto inputPath = temp.path() / "command.bw";
    writeTextFile(inputPath, input);
    const ParsedInputFile parsed = InputParser::parseInpFileDetailed(inputPath.string());
    REQUIRE(parsed.loaded);
    REQUIRE(parsed.tasks.size() == 1);
    CHECK_FALSE(parsed.tasks[0].isCitation());

    const auto expanded = Bwpack::expandCatalogCitations(
        input, catalog, parsed.tasks, "command.bw");
    CHECK(expanded.success);
    CHECK(expanded.expandedCount == 0);
    CHECK(expanded.content == input);
}

TEST_CASE("inline DOI conflicts are rejected during expansion") {
    TempDir temp;
    const auto catalogPath = temp.path() / "citations.conf";
    writeTextFile(catalogPath, R"([citation paper]
title = "Catalog title"
doi = "10.1234/catalog"
)");

    CitationManager catalog;
    REQUIRE(catalog.loadCatalog(catalogPath.string()));

    const std::string input = R"(bane.cite paper {
    reason = "Conflict test"
    doi = "10.1234/inline"
}
)";
    std::ostringstream capturedErrors;
    auto* oldBuffer = std::cerr.rdbuf(capturedErrors.rdbuf());
    ModuleTask conflictTask;
    conflictTask.kind = TaskKind::Citation;
    conflictTask.citationId = "paper";
    conflictTask.params["reason"] = "Conflict test";
    conflictTask.params["doi"] = "10.1234/inline";
    conflictTask.origin = "input:conflict.bw:1";
    const std::vector<ModuleTask> conflictTasks = {conflictTask};
    const auto expanded = Bwpack::expandCatalogCitations(
        input, catalog, conflictTasks, "conflict.bw");
    std::cerr.rdbuf(oldBuffer);

    CHECK_FALSE(expanded.success);
    CHECK(expanded.expandedCount == 0);
    CHECK(expanded.content == input);
    CHECK(capturedErrors.str().find("inline DOI conflicts") != std::string::npos);
}

TEST_CASE("module citation catalog contains only bindings selected by the workflow") {
    TempDir temp;
    const auto catalogPath = temp.path() / "citations.conf";
    writeTextFile(catalogPath, R"([weak-common]
title = "Weak-interaction framework"
year = 2020

[johnson2010]
title = "NCI method"
doi = "10.1234/nci"

)");

    CitationManager catalog;
    REQUIRE(catalog.loadCatalog(catalogPath.string()));

    ConfigManager configs;
    REQUIRE(configs.loadModuleConfigFromText("weak", R"([main]
20
[nci]
1
[rdg]
2
[iri]
4
[citations]
weak-common @main reason="Shared framework"
johnson2010 @nci @rdg reason="NCI analysis"
iri2021 @iri reason="IRI analysis"
[quit]
q
)", "weak.conf"));

    ModuleTask task;
    task.moduleName = "weak";
    task.postProcessSteps.push_back({"nci", {{"grid", "3"}}});

    const auto result = Bwpack::buildConfiguredCitationCatalog(
        catalog, configs, {task}, "workflow.bw");
    REQUIRE(result.success);
    CHECK(result.recordCount == 2);
    CHECK(result.catalogText.find("[weak-common]") != std::string::npos);
    CHECK(result.catalogText.find("[johnson2010]") != std::string::npos);
    CHECK(result.catalogText.find("[iri2021]") == std::string::npos);
    CHECK(result.catalogText.find("doi = \"10.1234/nci\"") != std::string::npos);

    CitationManager embedded;
    REQUIRE(embedded.loadCatalogFromText(result.catalogText, "packed.bwc"));
    CHECK(embedded.hasRecord("weak-common"));
    CHECK(embedded.hasRecord("johnson2010"));
    CHECK_FALSE(embedded.hasRecord("iri2021"));
}

TEST_CASE("module citation catalog rejects unresolved selected records") {
    CitationManager emptyCatalog;
    ConfigManager configs;
    REQUIRE(configs.loadModuleConfigFromText(
        "weak",
        "[main]\n20\n[citations]\nmissing @main\n[quit]\nq\n",
        "weak.conf"));

    ModuleTask task;
    task.moduleName = "weak";

    std::ostringstream capturedErrors;
    auto* oldBuffer = std::cerr.rdbuf(capturedErrors.rdbuf());
    const auto result = Bwpack::buildConfiguredCitationCatalog(
        emptyCatalog, configs, {task}, "workflow.bw");
    std::cerr.rdbuf(oldBuffer);

    CHECK_FALSE(result.success);
    CHECK(result.recordCount == 0);
    CHECK(capturedErrors.str().find("selected by a module [citations] binding") !=
          std::string::npos);
}

} // TEST_SUITE("Bwpack citation expansion")
