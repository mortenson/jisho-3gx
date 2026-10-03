// Host tests for the framework-free logic.
// Build and run from the repo root:
//   g++ -std=c++17 -IIncludes tests/host_tests.cpp -o /tmp/jisho_tests && /tmp/jisho_tests JMdict_smol.txt
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "DictSearch.hpp"
#include "Romaji.hpp"

static int failures = 0;

#define CHECK_EQ(actual, expected)                                                         \
    do {                                                                                   \
        auto a_ = (actual);                                                                \
        auto e_ = (expected);                                                              \
        if (!(a_ == e_)) {                                                                 \
            ++failures;                                                                    \
            std::cerr << "FAIL line " << __LINE__ << ": " #actual " = " << a_              \
                      << ", expected " << e_ << "\n";                                      \
        }                                                                                  \
    } while (0)

static void TestRomaji() {
    using Jisho::RomajiToHiragana;
    CHECK_EQ(RomajiToHiragana("kanji"), std::string("かんじ"));
    CHECK_EQ(RomajiToHiragana("kitte"), std::string("きって"));
    CHECK_EQ(RomajiToHiragana("matcha"), std::string("まっちゃ"));
    CHECK_EQ(RomajiToHiragana("konnichiha"), std::string("こんにちは"));
    CHECK_EQ(RomajiToHiragana("ookii"), std::string("おおきい"));
    CHECK_EQ(RomajiToHiragana("toukyou"), std::string("とうきょう"));
    CHECK_EQ(RomajiToHiragana("kon"), std::string("こん"));
    CHECK_EQ(RomajiToHiragana("konn"), std::string("こん"));
    CHECK_EQ(RomajiToHiragana("kon'ya"), std::string("こんや"));
    CHECK_EQ(RomajiToHiragana("zuzu"), std::string("ずず"));
    CHECK_EQ(RomajiToHiragana("tsuzuku"), std::string("つずく"));
    CHECK_EQ(RomajiToHiragana("q1"), std::string(""));  // unknown characters are dropped
    CHECK_EQ(RomajiToHiragana("du"), std::string("づ"));
    CHECK_EQ(RomajiToHiragana("SHIMA"), std::string("しま"));
    CHECK_EQ(RomajiToHiragana("jya"), std::string("じゃ"));
    CHECK_EQ(RomajiToHiragana("xtu"), std::string("っ"));
    CHECK_EQ(RomajiToHiragana(""), std::string(""));
}

struct FileReader {
    std::string data;
    size_t *reads;
    size_t operator()(uint64_t offset, char *buffer, size_t length) const {
        ++*reads;
        if (offset >= data.size()) return 0;
        size_t n = std::min<size_t>(length, data.size() - offset);
        data.copy(buffer, n, offset);
        return n;
    }
};

static std::vector<std::string> BruteForce(const std::string &data, const std::string &key, size_t max) {
    std::vector<std::string> out;
    std::istringstream in(data);
    std::string line;
    std::getline(in, line);  // header
    while (out.size() < max && std::getline(in, line)) {
        if (line.compare(0, key.size(), key) == 0) out.push_back(line);
    }
    return out;
}

static void TestSmall() {
    std::vector<std::string> lines = {"あい|愛|love", "あか|赤|red", "あお|青|blue", "いぬ|犬|dog", "うみ|海|sea"};
    std::sort(lines.begin(), lines.end());
    std::string data = "jisho1\n";
    for (size_t i = 0; i < lines.size(); ++i) data += lines[i] + (i + 1 < lines.size() ? "\n" : "");
    size_t reads = 0;
    FileReader r{data, &reads};
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "あ", 20).size(), size_t(3));
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "うみ", 20).size(), size_t(1));   // last line, no trailing newline
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "あい", 20).size(), size_t(1));
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "え", 20).size(), size_t(0));
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "ん", 20).size(), size_t(0));
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "あ", 2).size(), size_t(2));
    CHECK_EQ(Jisho::SearchPrefix(r, data.size(), "", 20).size(), size_t(0));
    CHECK_EQ(Jisho::SearchPrefix(r, 7, "あ", 20).size(), size_t(0));               // header only
    std::string headerOnly = "jisho1";
    FileReader r2{headerOnly, &reads};
    CHECK_EQ(Jisho::SearchPrefix(r2, headerOnly.size(), "あ", 20).size(), size_t(0));
}

static void TestRealDictionary(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { std::cerr << "skip real dictionary: cannot open " << path << "\n"; return; }
    std::stringstream ss;
    ss << f.rdbuf();
    std::string data = ss.str();

    std::vector<std::string> keys = {"あ", "あい", "あいあい", "か", "かんじ", "きっ", "しゃ", "にほん", "ん", "んん", "を",
                                     "ぁ", "ゔ", "ー", "ろ", "わ", "ゐ", "ぱ", "たべ", "ほ", "ほん"};
    size_t worstReads = 0;
    for (const auto &key : keys) {
        size_t reads = 0;
        FileReader r{data, &reads};
        auto got = Jisho::SearchPrefix(r, data.size(), key, 20);
        auto want = BruteForce(data, key, 20);
        if (got != want) {
            ++failures;
            std::cerr << "FAIL real dictionary mismatch for key '" << key << "': got " << got.size()
                      << ", want " << want.size() << "\n";
        }
        worstReads = std::max(worstReads, reads);
    }
    std::cout << "real dictionary: " << keys.size() << " keys checked, worst case " << worstReads
              << " file reads (1024 byte cache)\n";
}

int main(int argc, char **argv) {
    TestRomaji();
    TestSmall();
    TestRealDictionary(argc > 1 ? argv[1] : "JMdict_smol.txt");
    if (failures == 0) std::cout << "ALL PASS\n";
    else std::cout << failures << " FAILURE(S)\n";
    return failures == 0 ? 0 : 1;
}
