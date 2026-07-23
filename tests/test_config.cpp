#include "doctest.h"

#include "config.h"
#include "test_helpers.h"
#include "utils.h"

#include <map>
#include <vector>

TEST_SUITE("Config") {

TEST_CASE("getBaseName and replacePlaceholders follow the documented rules") {
    CHECK(Utils::getBaseName("/tmp/demo/test.fchk") == "test");
    CHECK(Utils::getBaseName("plain.wfn") == "plain");

    const std::map<std::string, std::string> params = {
        {"name", "orb"},
        {"grid", "200"},
        {"empty", ""}
    };

    CHECK(replacePlaceholders("run $name ${grid} ${missing:-2} ${empty:-7} $missing", params)
          == "run orb 200 2 7 ");
}

TEST_CASE("replacePlaceholdersExpanded supports var* aggregation and len(var)") {
    const std::map<std::string, std::string> indexedParams = {
        {"frag2", "5"},
        {"frag1", "2"},
        {"frag3", "9"}
    };

    const std::vector<std::string> expectedIndexed = {"2", "5", "9"};
    CHECK(replacePlaceholdersExpanded("${frag*}", indexedParams) == expectedIndexed);
    CHECK(replacePlaceholders("count=${len(frag)}", indexedParams) == "count=3");

    const std::map<std::string, std::string> directParams = {
        {"frag*", "(7 8)"}
    };
    const std::vector<std::string> expectedDirect = {"item 7", "item 8"};
    CHECK(replacePlaceholdersExpanded("item ${frag*}", directParams) == expectedDirect);
    CHECK(replacePlaceholders("${len(frag)}", directParams) == "2");

    const std::vector<std::string> expectedDefault = {"1", "4"};
    CHECK(replacePlaceholdersExpanded("${frag*:- (1 4)}", {}) == expectedDefault);
}

TEST_CASE("loadBaneWfnConfig parses rc values and expands HOME references") {
    TempDir temp;
    EnvVarGuard homeGuard("HOME", temp.path().string());

    const auto rcFile = temp.path() / "banewfn.rc";
    writeTextFile(rcFile,
                  "Multiwfn_exec = ~/bin/Multiwfn # comment\n"
                  "confpath = ${HOME}/conf\n"
                  "cores = 8\n"
                  "citations_output = \"~/exports/${input}_refs.bib\"\n");

    ConfigManager manager;
    REQUIRE(manager.loadBaneWfnConfig(rcFile.string()));

    const auto& cfg = manager.getConfig();
    CHECK(cfg.multiwfnExec == (temp.path() / "bin" / "Multiwfn").string());
    CHECK(cfg.confPath == (temp.path() / "conf").string());
    CHECK(cfg.cores == 8);
    CHECK(cfg.citationsOutput ==
          (temp.path() / "exports" / "${input}_refs.bib").string());
}

TEST_CASE("loadBaneWfnConfig ignores invalid cores instead of throwing") {
    TempDir temp;

    const auto rcFile = temp.path() / "banewfn.rc";
    writeTextFile(rcFile,
                  "Multiwfn_exec = Multiwfn\n"
                  "cores = not-a-number\n");

    ConfigManager manager;
    REQUIRE(manager.loadBaneWfnConfig(rcFile.string()));

    CHECK(manager.getCores() == 0);
}

TEST_CASE("loadBaneWfnConfig defaults optional cores to zero") {
    TempDir temp;

    const auto rcFile = temp.path() / "banewfn.rc";
    writeTextFile(rcFile,
                  "Multiwfn_exec = Multiwfn\n"
                  "confpath = ~/.bane/wfn\n");

    ConfigManager manager;
    REQUIRE(manager.loadBaneWfnConfig(rcFile.string()));

    CHECK(manager.getConfig().cores == 0);
    CHECK(manager.getCores() == 0);
}

TEST_CASE("loadModuleConfigFromText parses sections commands defaults and quit block") {
    const std::string confText = R"(
# module comment
[orb]
3
${index}
${grid}
-default-
index = "h"
grid = 2 # comment
[quit]
0
q
)";

    ConfigManager manager;
    REQUIRE(manager.loadModuleConfigFromText("fmo", confText, "unit-test"));
    REQUIRE(manager.hasModuleConfig("fmo"));

    const auto& mod = manager.getModuleConfig("fmo");
    REQUIRE(mod.sections.count("orb") == 1);

    const auto& orb = mod.sections.at("orb");
    const std::vector<std::string> expectedCommands = {"3", "${index}", "${grid}"};
    const std::vector<std::string> expectedQuit = {"0", "q"};

    CHECK(orb.commands == expectedCommands);
    CHECK(orb.defaults.at("index") == "h");
    CHECK(orb.defaults.at("grid") == "2");
    CHECK(mod.quitCommands == expectedQuit);
}

TEST_CASE("missing quit section falls back to a single q command") {
    const std::string confText = R"(
[main]
200
)";

    ConfigManager manager;
    REQUIRE(manager.loadModuleConfigFromText("plane", confText, "unit-test"));

    const auto& mod = manager.getModuleConfig("plane");
    const std::vector<std::string> expectedQuit = {"q"};
    CHECK(mod.quitCommands == expectedQuit);
}

TEST_CASE("citations section binds catalog ids to executable sections") {
    const std::string confText = R"(
[main]
20

[nci]
1
${grid:-2}
-default-
grid = 2

[rdg]
2

[citations]
weak-common @main reason="Weak-interaction analysis framework"
johnson2010 @nci @rdg reason="NCI analysis with grid ${grid:-2}"

[quit]
0
q
)";

    ConfigManager manager;
    REQUIRE(manager.loadModuleConfigFromText("weak", confText, "weak.conf"));

    const auto& mod = manager.getModuleConfig("weak");
    CHECK(mod.origin == "weak.conf");
    CHECK(mod.sections.count("citations") == 0);
    REQUIRE(mod.citationBindings.size() == 2);

    CHECK(mod.citationBindings[0].id == "weak-common");
    CHECK(mod.citationBindings[0].targets == std::vector<std::string>{"main"});
    CHECK(mod.citationBindings[0].reason == "Weak-interaction analysis framework");

    CHECK(mod.citationBindings[1].id == "johnson2010");
    CHECK(mod.citationBindings[1].targets ==
          std::vector<std::string>{"nci", "rdg"});
    CHECK(mod.citationBindings[1].reason ==
          "NCI analysis with grid ${grid:-2}");
}

TEST_CASE("citations section rejects malformed and unknown targets") {
    ConfigManager malformed;
    CHECK_FALSE(malformed.loadModuleConfigFromText(
        "weak",
        "[main]\n20\n[citations]\npaper reason=missing-target\n[quit]\nq\n",
        "malformed.conf"));
    CHECK_FALSE(malformed.hasModuleConfig("weak"));

    ConfigManager unknownTarget;
    CHECK_FALSE(unknownTarget.loadModuleConfigFromText(
        "weak",
        "[main]\n20\n[citations]\npaper @nci\n[quit]\nq\n",
        "unknown.conf"));
    CHECK_FALSE(unknownTarget.hasModuleConfig("weak"));

    ConfigManager quoted;
    CHECK_FALSE(quoted.loadModuleConfigFromText(
        "weak",
        "[main]\n20\n[citations]\npaper @main reason=\"unterminated\n[quit]\nq\n",
        "quoted.conf"));
}

} // TEST_SUITE("Config")
