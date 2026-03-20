#include "doctest.h"

#include "test_helpers.h"
#include "utils.h"

#include <filesystem>
#include <vector>

TEST_SUITE("Utils") {

TEST_CASE("trim and trimQuotes strip whitespace and paired quotes") {
    CHECK(Utils::trim(" \t hello \n") == "hello");
    CHECK(Utils::trim("   \t  ").empty());
    CHECK(Utils::trimQuotes("  \"quoted\"  ") == "quoted");
    CHECK(Utils::trimQuotes("  'single'  ") == "single");
    CHECK(Utils::trimQuotes("  no_quotes  ") == "no_quotes");
}

TEST_CASE("split collapses whitespace delimiters and trims explicit separators") {
    const std::vector<std::string> whitespaceExpected = {"alpha", "beta", "gamma"};
    CHECK(Utils::split(" alpha   beta\tgamma ", ' ') == whitespaceExpected);

    const std::vector<std::string> commaExpected = {"one", "two", "three"};
    CHECK(Utils::split(" one, two , three ", ',') == commaExpected);
}

TEST_CASE("removeInlineComment preserves quoted and escaped hash characters") {
    CHECK(Utils::removeInlineComment("value # comment") == "value");
    CHECK(Utils::removeInlineComment("\"a#b\" # trailing") == "\"a#b\"");
    CHECK(Utils::removeInlineComment("'a#b' # trailing") == "'a#b'");
    CHECK(Utils::removeInlineComment(R"(path\#1 # trailing)") == "path#1");
}

TEST_CASE("parseBashArray and parseCommandLineArgs handle simple shell-like syntax") {
    const std::vector<std::string> arrayExpected = {"1", "two", "3"};
    CHECK(Utils::parseBashArray("(1 'two' 3)") == arrayExpected);

    const std::vector<std::string> argsExpected = {
        "--flag",
        "two words",
        "three words",
        "path with spaces"
    };
    CHECK(Utils::parseCommandLineArgs(R"(--flag "two words" 'three words' path\ with\ spaces)") == argsExpected);
}

TEST_CASE("expandWildcard returns sorted regular files and handles literal paths") {
    TempDir temp;
    temp.chdir();

    writeTextFile(temp.path() / "b.fchk", "");
    writeTextFile(temp.path() / "a.fchk", "");
    std::filesystem::create_directory(temp.path() / "subdir");

    const std::vector<std::string> wildcardExpected = {"a.fchk", "b.fchk"};
    CHECK(Utils::expandWildcard("*.fchk") == wildcardExpected);

    const std::vector<std::string> literalExpected = {"a.fchk"};
    CHECK(Utils::expandWildcard("a.fchk") == literalExpected);
    CHECK(Utils::expandWildcard("missing.fchk").empty());
}

} // TEST_SUITE("Utils")
