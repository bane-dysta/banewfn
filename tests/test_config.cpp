#include "doctest.h"

#include "config.h"
#include "test_helpers.h"

#include <map>
#include <vector>

TEST_SUITE("Config") {

TEST_CASE("getBaseName and replacePlaceholders follow the documented rules") {
    CHECK(getBaseName("/tmp/demo/test.fchk") == "test");
    CHECK(getBaseName("plain.wfn") == "plain");

    const std::map<std::string, std::string> params = {
        {"name", "orb"},
        {"grid", "200"},
        {"empty", ""}
    };

    CHECK(replacePlaceholders("run $name ${grid} ${missing:-2} ${empty:-7} $missing", params)
          == "run orb 200 2 7 ");
}

TEST_CASE("loadBaneWfnConfig parses rc values and expands HOME references") {
    TempDir temp;
    EnvVarGuard homeGuard("HOME", temp.path().string());

    const auto rcFile = temp.path() / "banewfn.rc";
    writeTextFile(rcFile,
                  "Multiwfn_exec = ~/bin/Multiwfn # comment\n"
                  "confpath = ${HOME}/conf\n"
                  "cores = 8\n");

    ConfigManager manager;
    REQUIRE(manager.loadBaneWfnConfig(rcFile.string()));

    const auto& cfg = manager.getConfig();
    CHECK(cfg.multiwfnExec == (temp.path() / "bin" / "Multiwfn").string());
    CHECK(cfg.confPath == (temp.path() / "conf").string());
    CHECK(cfg.cores == 8);
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

} // TEST_SUITE("Config")
