#include "doctest.h"

#include "common/inline_conf.h"
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

TEST_CASE("inline citation catalog round-trips beside module configs") {
    TempDir temp;
    const auto scriptFile = temp.path() / "workflow.bwc";

    const std::string prefix = "wfn=test.fchk\n[weak]\nend\n";
    const std::string catalog =
        "[johnson2010]\n"
        "title = \"NCI method\"\n"
        "doi = \"10.1234/nci\"\n";
    const std::string citationBlock =
        InlineConf::formatInlineCitationCatalogBlock(catalog);
    const std::string weakBlock =
        InlineConf::formatInlineConfBlock("weak", "[main]\n20\n");

    writeTextFile(scriptFile, prefix + citationBlock + weakBlock);

    CHECK(InlineConf::extractInlineCitationCatalog(scriptFile.string()) ==
          "# bundled citation catalog\n" + catalog);
    const auto confs = InlineConf::extractInlineConfs(scriptFile.string());
    REQUIRE(confs.size() == 1);
    CHECK(confs.at("weak") == "# bundled module: weak\n[main]\n20\n");
    CHECK(InlineConf::stripInlineConfsFromFile(scriptFile.string()) == prefix);
}

} // TEST_SUITE("InlineConf")
