#include "doctest.h"

#include "inline_conf.h"
#include "test_helpers.h"

#include <map>
#include <string>

TEST_SUITE("InlineConf") {

TEST_CASE("format extract and strip inline conf blocks round-trip correctly") {
    TempDir temp;
    const auto scriptFile = temp.path() / "workflow.bw";

    const std::string prefix = "wfn=test.fchk\n[fmo]\nend\n";
    const std::string fmoBlock = InlineConf::formatInlineConfBlock("fmo", "[main]\n200\n");
    const std::string planeBlock = InlineConf::formatInlineConfBlock("plane", "[main]\n4\n");

    writeTextFile(scriptFile, prefix + fmoBlock + planeBlock);

    const auto confs = InlineConf::extractInlineConfs(scriptFile.string());
    REQUIRE(confs.size() == 2);
    CHECK(confs.at("fmo") == "# bundled module: fmo\n[main]\n200\n");
    CHECK(confs.at("plane") == "# bundled module: plane\n[main]\n4\n");
    CHECK(InlineConf::stripInlineConfsFromFile(scriptFile.string()) == prefix);
}

} // TEST_SUITE("InlineConf")
