#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

// Credit to https://github.com/aleckretch/Romaji-to-Japanese-Converter for the original table.
namespace Jisho
{
    namespace detail
    {
        inline const std::unordered_map<std::string, std::string> &RomajiTable()
        {
            static const std::unordered_map<std::string, std::string> table = {
                {"a", "あ"}, {"i", "い"}, {"u", "う"}, {"e", "え"}, {"o", "お"},
                {"ka", "か"}, {"ki", "き"}, {"ku", "く"}, {"ke", "け"}, {"ko", "こ"},
                {"ga", "が"}, {"gi", "ぎ"}, {"gu", "ぐ"}, {"ge", "げ"}, {"go", "ご"},
                {"sa", "さ"}, {"shi", "し"}, {"si", "し"}, {"su", "す"}, {"se", "せ"}, {"so", "そ"},
                {"za", "ざ"}, {"ji", "じ"}, {"zi", "じ"}, {"zu", "ず"}, {"ze", "ぜ"}, {"zo", "ぞ"},
                {"ta", "た"}, {"chi", "ち"}, {"ti", "ち"}, {"tsu", "つ"}, {"tu", "つ"}, {"te", "て"}, {"to", "と"},
                {"da", "だ"}, {"di", "ぢ"}, {"du", "づ"}, {"de", "で"}, {"do", "ど"},
                {"na", "な"}, {"ni", "に"}, {"nu", "ぬ"}, {"ne", "ね"}, {"no", "の"},
                {"ha", "は"}, {"hi", "ひ"}, {"fu", "ふ"}, {"hu", "ふ"}, {"he", "へ"}, {"ho", "ほ"},
                {"ba", "ば"}, {"bi", "び"}, {"bu", "ぶ"}, {"be", "べ"}, {"bo", "ぼ"},
                {"pa", "ぱ"}, {"pi", "ぴ"}, {"pu", "ぷ"}, {"pe", "ぺ"}, {"po", "ぽ"},
                {"ma", "ま"}, {"mi", "み"}, {"mu", "む"}, {"me", "め"}, {"mo", "も"},
                {"ya", "や"}, {"yu", "ゆ"}, {"yo", "よ"},
                {"ra", "ら"}, {"ri", "り"}, {"ru", "る"}, {"re", "れ"}, {"ro", "ろ"},
                {"wa", "わ"}, {"wo", "を"},
                {"kya", "きゃ"}, {"kyu", "きゅ"}, {"kyo", "きょ"},
                {"gya", "ぎゃ"}, {"gyu", "ぎゅ"}, {"gyo", "ぎょ"},
                {"sha", "しゃ"}, {"shu", "しゅ"}, {"she", "しぇ"}, {"sho", "しょ"},
                {"sya", "しゃ"}, {"syu", "しゅ"}, {"syo", "しょ"},
                {"ja", "じゃ"}, {"ju", "じゅ"}, {"je", "じぇ"}, {"jo", "じょ"},
                {"zya", "じゃ"}, {"zyu", "じゅ"}, {"zyo", "じょ"},
                {"jya", "じゃ"}, {"jyu", "じゅ"}, {"jyo", "じょ"},
                {"cha", "ちゃ"}, {"chu", "ちゅ"}, {"che", "ちぇ"}, {"cho", "ちょ"},
                {"tya", "ちゃ"}, {"tyu", "ちゅ"}, {"tyo", "ちょ"},
                {"dya", "ぢゃ"}, {"dyu", "ぢゅ"}, {"dyo", "ぢょ"},
                {"nya", "にゃ"}, {"nyu", "にゅ"}, {"nyo", "にょ"},
                {"hya", "ひゃ"}, {"hyu", "ひゅ"}, {"hyo", "ひょ"},
                {"bya", "びゃ"}, {"byu", "びゅ"}, {"byo", "びょ"},
                {"pya", "ぴゃ"}, {"pyu", "ぴゅ"}, {"pyo", "ぴょ"},
                {"mya", "みゃ"}, {"myu", "みゅ"}, {"myo", "みょ"},
                {"rya", "りゃ"}, {"ryu", "りゅ"}, {"ryo", "りょ"},
                {"fa", "ふぁ"}, {"fi", "ふぃ"}, {"fe", "ふぇ"}, {"fo", "ふぉ"},
                {"vu", "ゔ"},
                {"xtu", "っ"}, {"ltu", "っ"}, {"xtsu", "っ"}, {"ltsu", "っ"},
            };
            return table;
        }

        inline bool IsVowelOrY(char c)
        {
            return c == 'a' || c == 'i' || c == 'u' || c == 'e' || c == 'o' || c == 'y';
        }

        inline bool IsDoublingConsonant(char c)
        {
            return std::isalpha(static_cast<unsigned char>(c)) && c != 'n' && !IsVowelOrY(c);
        }
    }

    // Converts romaji to hiragana. Unknown characters are dropped.
    // Examples: "kanji" -> "かんじ", "kitte" -> "きって", "konnichiha" -> "こんにちは".
    inline std::string RomajiToHiragana(const std::string &input)
    {
        const auto &table = detail::RomajiTable();

        std::string s;
        s.reserve(input.size());
        for (char c : input)
            s.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));

        std::string out;
        size_t i = 0;
        const size_t n = s.size();
        while (i < n)
        {
            // Doubled consonant ("kk", "tt") and "tch" make a small tsu.
            if (i + 1 < n && detail::IsDoublingConsonant(s[i]) && s[i] == s[i + 1])
            {
                out += "っ";
                i += 1;
                continue;
            }
            if (s[i] == 't' && i + 2 < n && s[i + 1] == 'c' && s[i + 2] == 'h')
            {
                out += "っ";
                i += 1;
                continue;
            }

            // Longest match first.
            bool matched = false;
            for (size_t len = std::min<size_t>(4, n - i); len > 0; --len)
            {
                auto it = table.find(s.substr(i, len));
                if (it != table.end())
                {
                    out += it->second;
                    i += len;
                    matched = true;
                    break;
                }
            }
            if (matched)
                continue;

            if (s[i] == 'n')
            {
                // "n" before a consonant, a quote, or the end of the input.
                out += "ん";
                i += 1;
                if (i < n && s[i] == '\'')
                {
                    i += 1;
                }
                else if (i < n && s[i] == 'n' && (i + 1 >= n || !detail::IsVowelOrY(s[i + 1])))
                {
                    // "nn" before a consonant or the end is one ん.
                    i += 1;
                }
                continue;
            }

            // Unknown character.
            i += 1;
        }
        return out;
    }
}
