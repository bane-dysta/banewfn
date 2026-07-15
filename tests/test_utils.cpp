#include "doctest.h"

#include "test_helpers.h"
#include "utils.h"

#include <filesystem>
#include <map>
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

TEST_CASE("expandVariableCombinations builds cartesian products for array variables") {
    const std::map<std::string, std::vector<std::string>> vars = {
        {"prefix", {"job"}},
        {"spin", {"alpha", "beta"}},
        {"state", {"1", "2", "3"}}
    };

    const auto combinations = Utils::expandVariableCombinations(vars);

    REQUIRE(combinations.size() == 6);

    CHECK(combinations[0].at("prefix") == std::vector<std::string>{"job"});
    CHECK(combinations[0].at("spin") == std::vector<std::string>{"alpha"});
    CHECK(combinations[0].at("state") == std::vector<std::string>{"1"});

    CHECK(combinations[2].at("spin") == std::vector<std::string>{"alpha"});
    CHECK(combinations[2].at("state") == std::vector<std::string>{"3"});

    CHECK(combinations[3].at("spin") == std::vector<std::string>{"beta"});
    CHECK(combinations[3].at("state") == std::vector<std::string>{"1"});

    CHECK(combinations[5].at("spin") == std::vector<std::string>{"beta"});
    CHECK(combinations[5].at("state") == std::vector<std::string>{"3"});
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

TEST_CASE("tryParseNonNegativeInt rejects partial values negatives and overflow") {
    int value = -1;
    CHECK(Utils::tryParseNonNegativeInt(" 42 ", value));
    CHECK(value == 42);
    CHECK(Utils::tryParseNonNegativeInt("0", value));
    CHECK(value == 0);
    CHECK(Utils::tryParseNonNegativeInt("+8", value));
    CHECK(value == 8);

    value = 7;
    CHECK_FALSE(Utils::tryParseNonNegativeInt("", value));
    CHECK_FALSE(Utils::tryParseNonNegativeInt("-1", value));
    CHECK_FALSE(Utils::tryParseNonNegativeInt("-0", value));
    CHECK_FALSE(Utils::tryParseNonNegativeInt("12x", value));
    CHECK_FALSE(Utils::tryParseNonNegativeInt("999999999999999999999", value));
    CHECK(value == 7);
}

TEST_CASE("shared variable syntax accepts documented forms and rejects malformed names") {
    CHECK(VariableSyntax::isPlainVariableName("state_2"));
    CHECK_FALSE(VariableSyntax::isPlainVariableName("state-2"));

    std::string baseName;
    int index = 0;
    CHECK(VariableSyntax::isListVariableName("frag*", &baseName));
    CHECK(baseName == "frag");
    CHECK(VariableSyntax::isLengthVariableName("len(frag)", &baseName));
    CHECK(baseName == "frag");
    CHECK(VariableSyntax::parseIndexedVariableName("frag12", &baseName, &index));
    CHECK(baseName == "frag");
    CHECK(index == 12);

    CHECK(VariableSyntax::isValidCustomVariableName("frag*"));
    CHECK(VariableSyntax::isValidCustomVariableName("len(frag)"));
    CHECK_FALSE(VariableSyntax::isValidCustomVariableName("frag-*"));
    CHECK_FALSE(VariableSyntax::parseIndexedVariableName("frag0", nullptr, nullptr));
}

TEST_CASE("shared placeholder parser and list serializer preserve DSL semantics") {
    const std::string text = "run $name ${frag*:- (1 2)} ${len(frag)}";
    VariableSyntax::Placeholder placeholder;

    REQUIRE(VariableSyntax::parseNextPlaceholder(text, 0, placeholder));
    CHECK(placeholder.name == "name");
    CHECK_FALSE(placeholder.braced);

    REQUIRE(VariableSyntax::parseNextPlaceholder(text, placeholder.end, placeholder));
    CHECK(placeholder.name == "frag*");
    CHECK(placeholder.defaultValue == " (1 2)");
    CHECK(placeholder.braced);

    REQUIRE(VariableSyntax::parseNextPlaceholder(text, placeholder.end, placeholder));
    CHECK(placeholder.name == "len(frag)");
    CHECK(placeholder.defaultValue.empty());
    CHECK(placeholder.braced);
    CHECK_FALSE(VariableSyntax::parseNextPlaceholder(text, placeholder.end, placeholder));

    CHECK(VariableSyntax::serializeListValues({}) == "");
    CHECK(VariableSyntax::serializeListValues({"one"}) == "one");
    CHECK(VariableSyntax::serializeListValues({"one", "", "three"}) == "(one \"\" three)");
}

} // TEST_SUITE("Utils")
