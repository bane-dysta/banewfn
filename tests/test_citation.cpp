#include "doctest.h"

#include "common/citation.h"
#include "test_helpers.h"

#include <iostream>
#include <sstream>

TEST_SUITE("Citation") {

TEST_CASE("citation catalog parses structured records and DOI prefixes") {
    TempDir temp;
    const auto catalog = temp.path() / "citations.conf";
    writeTextFile(catalog, R"(
[citation paper]
authors = "Alpha One; Beta Two"
title = "Catalog Paper"
journal = "Test Journal"
year = 2024
doi = "https://doi.org/10.1234/ABC.DEF"

[text-only]
text = "A complete preformatted reference."
)"
    );

    CitationManager manager;
    REQUIRE(manager.loadCatalog(catalog.string()));
    CHECK(manager.hasRecord("paper"));
    CHECK(manager.hasRecord("PAPER"));
    CHECK(manager.hasRecord("text-only"));
    CHECK(CitationManager::normalizeDoi("doi:HTTPS://DOI.ORG/10.1234/ABC.DEF")
          == "10.1234/abc.def");
}

TEST_CASE("embedded catalogs override external catalogs and input records remain highest priority") {
    TempDir temp;
    const auto catalog = temp.path() / "citations.conf";
    writeTextFile(catalog, R"([paper]
title = "External title"
doi = "10.1234/priority"
)");

    CitationManager manager;
    REQUIRE(manager.loadCatalog(catalog.string()));
    REQUIRE(manager.loadCatalogFromText(R"([paper]
title = "Bundled title"
doi = "10.1234/priority"
)", "workflow.bwc"));

    CitationRecord record;
    REQUIRE(manager.getRecord("paper", record));
    CHECK(record.title == "Bundled title");

    CitationRecord inputRecord = record;
    inputRecord.title = "Input title";
    REQUIRE(manager.addOrOverride(inputRecord, "input:workflow.bw:4"));
    REQUIRE(manager.getRecord("paper", record));
    CHECK(record.title == "Input title");
}

TEST_CASE("citation manager deduplicates normalized DOI and merges use reasons") {
    CitationManager manager;

    CitationRecord first;
    first.id = "first";
    first.authors = {"Alpha One"};
    first.title = "Shared Paper";
    first.doi = "doi:10.1000/XYZ";
    REQUIRE(manager.addOrOverride(first, "input:a.bw:1"));
    REQUIRE(manager.use("first", "First method", "module:first"));

    CitationRecord second = first;
    second.id = "second";
    second.doi = "https://doi.org/10.1000/xyz";
    REQUIRE(manager.addOrOverride(second, "input:a.bw:8"));
    REQUIRE(manager.use("second", "Second method", "module:second"));

    CHECK(manager.usedCitationCount() == 1);
    std::ostringstream summary;
    manager.printSummary(summary);
    const std::string text = summary.str();
    CHECK(text.find("[1]") != std::string::npos);
    CHECK(text.find("[2]") == std::string::npos);
    CHECK(text.find("First method") != std::string::npos);
    CHECK(text.find("Second method") != std::string::npos);
}

TEST_CASE("inline citation fields override catalog fields while equal-priority conflicts keep first") {
    TempDir temp;
    const auto catalog = temp.path() / "citations.conf";
    writeTextFile(catalog, R"(
[paper]
title = "Catalog Title"
year = 2020
doi = "10.2000/conflict"
)"
    );

    CitationManager manager;
    REQUIRE(manager.loadCatalog(catalog.string()));

    CitationRecord inlineRecord;
    inlineRecord.id = "paper-inline";
    inlineRecord.title = "Inline Title";
    inlineRecord.year = "2021";
    inlineRecord.doi = "10.2000/conflict";
    REQUIRE(manager.addOrOverride(inlineRecord, "input:workflow.bw:4"));

    CitationRecord conflicting = inlineRecord;
    conflicting.id = "paper-later";
    conflicting.title = "Later Title";

    std::ostringstream capturedErrors;
    std::streambuf* oldErrors = std::cerr.rdbuf(capturedErrors.rdbuf());
    const bool added = manager.addOrOverride(conflicting, "input:workflow.bw:12");
    std::cerr.rdbuf(oldErrors);
    REQUIRE(added);
    CHECK(capturedErrors.str().find("Citation conflict") != std::string::npos);

    REQUIRE(manager.use("paper-later", "Conflict test", "module:test"));
    std::ostringstream summary;
    manager.printSummary(summary);
    CHECK(summary.str().find("Inline Title") != std::string::npos);
    CHECK(summary.str().find("Later Title") == std::string::npos);
    CHECK(summary.str().find("2021") != std::string::npos);
}

TEST_CASE("citation priority is tracked per field") {
    TempDir temp;
    const auto catalog = temp.path() / "citations.conf";
    writeTextFile(catalog, R"(
[paper]
title = "Stable Title"
url = "https://catalog.invalid/paper"
doi = "10.2500/field-priority"
)"
    );

    CitationManager manager;
    REQUIRE(manager.loadCatalog(catalog.string()));

    CitationRecord firstInline;
    firstInline.id = "first-inline";
    firstInline.title = "Stable Title";
    firstInline.doi = "10.2500/field-priority";
    REQUIRE(manager.addOrOverride(firstInline, "input:workflow.bw:2"));

    CitationRecord secondInline;
    secondInline.id = "second-inline";
    secondInline.title = "Later Title";
    secondInline.url = "https://inline.invalid/paper";
    secondInline.doi = "10.2500/field-priority";
    REQUIRE(manager.addOrOverride(secondInline, "input:workflow.bw:8"));
    REQUIRE(manager.use(secondInline.id, "Field priority", "module:test"));

    const auto bibPath = temp.path() / "references.bib";
    REQUIRE(manager.writeBibTeX(bibPath.string()));
    const std::string bib = readTextFile(bibPath);
    CHECK(bib.find("title = {Stable Title}") != std::string::npos);
    CHECK(bib.find("Later Title") == std::string::npos);
    CHECK(bib.find("url = {https://inline.invalid/paper}") != std::string::npos);
    CHECK(bib.find("catalog.invalid") == std::string::npos);
}

TEST_CASE("an id-only record is merged when it is later linked to an existing DOI") {
    CitationManager manager;

    CitationRecord idOnly;
    idOnly.id = "paper-a";
    idOnly.title = "Shared Paper";
    REQUIRE(manager.addOrOverride(idOnly, "input:first.bw:1"));
    REQUIRE(manager.use(idOnly.id, "First use", "module:first"));

    CitationRecord doiRecord;
    doiRecord.id = "paper-b";
    doiRecord.title = "Shared Paper";
    doiRecord.doi = "10.2600/shared";
    REQUIRE(manager.addOrOverride(doiRecord, "input:second.bw:1"));
    REQUIRE(manager.use(doiRecord.id, "Second use", "module:second"));

    CitationRecord link = doiRecord;
    link.id = idOnly.id;
    REQUIRE(manager.addOrOverride(link, "input:third.bw:1"));

    CHECK(manager.usedCitationCount() == 1);
    std::ostringstream summary;
    manager.printSummary(summary);
    CHECK(summary.str().find("[2]") == std::string::npos);
    CHECK(summary.str().find("First use") != std::string::npos);
    CHECK(summary.str().find("Second use") != std::string::npos);
}

TEST_CASE("one citation id cannot silently change to a different DOI") {
    CitationManager manager;

    CitationRecord first;
    first.id = "paper";
    first.title = "First Paper";
    first.doi = "10.3000/first";
    REQUIRE(manager.addOrOverride(first, "input:first.bw:1"));

    CitationRecord second = first;
    second.title = "Second Paper";
    second.doi = "10.3000/second";

    std::ostringstream capturedErrors;
    std::streambuf* oldErrors = std::cerr.rdbuf(capturedErrors.rdbuf());
    const bool added = manager.addOrOverride(second, "input:second.bw:1");
    std::cerr.rdbuf(oldErrors);

    CHECK_FALSE(added);
    CHECK(capturedErrors.str().find("conflicts with DOI") != std::string::npos);
}

TEST_CASE("plain and BibTeX writers export only used citations") {
    TempDir temp;
    CitationManager manager;

    CitationRecord used;
    used.id = "emamian2019";
    used.authors = {"Saeedreza Emamian", "Tian Lu"};
    used.title = "Exploring Nature and Predicting Strength of Hydrogen Bonds";
    used.journal = "Journal of Computational Chemistry";
    used.year = "2019";
    used.volume = "40";
    used.issue = "32";
    used.pages = "2868-2881";
    used.doi = "10.1002/JCC.26068";
    REQUIRE(manager.addOrOverride(used));
    REQUIRE(manager.use(used.id, "Hydrogen-bond energy equation", "input:test.bw:1"));

    CitationRecord unused;
    unused.id = "unused";
    unused.title = "Unused Paper";
    REQUIRE(manager.addOrOverride(unused));

    const auto plainPath = temp.path() / "out" / "CITATIONS.txt";
    const auto bibPath = temp.path() / "out" / "references.bib";
    REQUIRE(manager.writePlain(plainPath.string()));
    REQUIRE(manager.writeBibTeX(bibPath.string()));

    const std::string plain = readTextFile(plainPath);
    CHECK(plain.find("emamian") == std::string::npos);
    CHECK(plain.find("Exploring Nature") != std::string::npos);
    CHECK(plain.find("Hydrogen-bond energy equation") != std::string::npos);
    CHECK(plain.find("Unused Paper") == std::string::npos);

    const std::string bib = readTextFile(bibPath);
    CHECK(bib.find("@article{emamian2019") != std::string::npos);
    CHECK(bib.find("author = {Saeedreza Emamian and Tian Lu}") != std::string::npos);
    CHECK(bib.find("doi = {10.1002/jcc.26068}") != std::string::npos);
    CHECK(bib.find("Unused Paper") == std::string::npos);
}


TEST_CASE("BibTeX writer merges prior BaneWfn exports and preserves workflow reasons") {
    TempDir temp;
    const auto bibPath = temp.path() / "references.bib";

    CitationManager firstRun;
    CitationRecord esp;
    esp.id = "esp-paper";
    esp.title = "ESP Paper";
    esp.doi = "10.1000/esp";
    REQUIRE(firstRun.addOrOverride(esp));
    REQUIRE(firstRun.use(esp.id, "ESP analysis", "input:first.bwc:1"));

    CitationRecord banewfn;
    banewfn.id = "banewfn";
    banewfn.title = "BaneWfn";
    banewfn.doi = "10.1000/banewfn";
    REQUIRE(firstRun.addOrOverride(banewfn));
    REQUIRE(firstRun.use(banewfn.id, "Workflow execution", "software:banewfn"));
    REQUIRE(firstRun.writeBibTeX(bibPath.string()));

    CitationManager secondRun;
    CitationRecord espAgain = esp;
    espAgain.doi = "https://doi.org/10.1000/ESP";
    REQUIRE(secondRun.addOrOverride(espAgain));
    REQUIRE(secondRun.use(espAgain.id, "ESP follow-up", "input:second.bwc:1"));

    CitationRecord igmh;
    igmh.id = "igmh-paper";
    igmh.title = "IGMH Paper";
    igmh.doi = "https://doi.org/10.1000/igmh";
    REQUIRE(secondRun.addOrOverride(igmh));
    REQUIRE(secondRun.use(igmh.id, "IGMH", "input:second.bwc:1"));
    REQUIRE(secondRun.writeBibTeX(bibPath.string()));

    const std::string bib = readTextFile(bibPath);
    CHECK(bib.find("baneid = {esp-paper}") != std::string::npos);
    CHECK(bib.find("baneid = {igmh-paper}") != std::string::npos);
    CHECK(bib.find("baneid = {banewfn}") != std::string::npos);
    CHECK(bib.find("banereason1 = {ESP analysis}") != std::string::npos);
    CHECK(bib.find("banereason2 = {ESP follow-up}") != std::string::npos);
    CHECK(bib.find("banereason1 = {IGMH}") != std::string::npos);
    CHECK(bib.find("Workflow execution") == std::string::npos);
    CHECK(bib.find("doi = {10.1000/igmh}") != std::string::npos);
    const std::size_t espDoi = bib.find("doi = {10.1000/esp}");
    REQUIRE(espDoi != std::string::npos);
    CHECK(bib.find("doi = {10.1000/esp}", espDoi + 1) == std::string::npos);
}

TEST_CASE("BibTeX merge refuses malformed prior output without overwriting it") {
    TempDir temp;
    const auto bibPath = temp.path() / "references.bib";
    const std::string malformed = "@article{broken,\n  title = {Unterminated\n";
    writeTextFile(bibPath, malformed);

    CitationManager manager;
    CitationRecord record;
    record.id = "valid";
    record.title = "Valid Paper";
    REQUIRE(manager.addOrOverride(record));
    REQUIRE(manager.use(record.id, "Validation", "input:test.bwc:1"));

    std::ostringstream capturedErrors;
    std::streambuf* oldErrors = std::cerr.rdbuf(capturedErrors.rdbuf());
    const bool written = manager.writeBibTeX(bibPath.string());
    std::cerr.rdbuf(oldErrors);

    CHECK_FALSE(written);
    CHECK(capturedErrors.str().find("Cannot merge existing BibTeX file") != std::string::npos);
    CHECK(readTextFile(bibPath) == malformed);
}


} // TEST_SUITE("Citation")
