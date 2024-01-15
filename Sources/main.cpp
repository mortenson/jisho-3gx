#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include "Helpers/Wrappers.hpp"
#include <string>
#include <map>
#include <sstream>

namespace CTRPluginFramework
{

    u32         MenuHotkeys = static_cast<u32>(Key::Start);

    // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
    }

    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }

    std::string RomajiToHiragana(std::string romaji)
    {
        std::map<std::string, std::string>romaMap;
        romaMap["a"] = "あ";
        romaMap["i"] = "い";
        romaMap["u"] = "う";
        romaMap["e"] = "え";
        romaMap["o"] ="お";
        romaMap["ka"] = "か";
        romaMap["ki"] = "き";
        romaMap["ku"] = "く";
        romaMap["ke"] = "け";
        romaMap["ko"] = "こ";
        romaMap["ga"] = "が";
        romaMap["gi"] = "ぎ";
        romaMap["gu"] = "ぐ";
        romaMap["ge"] = "げ";
        romaMap["go"] = "ご";
        romaMap["sa"] = "さ";
        romaMap["shi"] = "し";
        romaMap["su"] = "す";
        romaMap["se"] = "せ";
        romaMap["so"] = "そ";
        romaMap["za"] = "ざ";
        romaMap["ji"] = "じ";
        romaMap["zu"] = "ず";
        romaMap["ze"] = "ぜ";
        romaMap["zo"] = "ぞ";
        romaMap["ta"] = "た";
        romaMap["chi"] = "ち";
        romaMap["tsu"] = "つ";
        romaMap["te"] = "て";
        romaMap["to"] = "と";
        romaMap["da"] = "だ";
        romaMap["zu"] = "づ";
        romaMap["de"] = "で";
        romaMap["do"] = "ど";
        romaMap["na"] = "な";
        romaMap["ni"] = "に";
        romaMap["nu"] = "ぬ";
        romaMap["ne"] = "ね";
        romaMap["no"] = "の";
        romaMap["ha"] = "は";
        romaMap["hi"] = "ひ";
        romaMap["fu"] = "ふ";
        romaMap["he"] = "へ";
        romaMap["ho"] = "ほ";
        romaMap["ba"] = "ば";
        romaMap["bi"] = "び";
        romaMap["bu"] = "ぶ";
        romaMap["be"] = "べ";
        romaMap["bo"] = "ぼ";
        romaMap["pa"] = "ぱ";
        romaMap["pi"] = "ぴ";
        romaMap["pu"] = "ぷ";
        romaMap["pe"] = "ぺ";
        romaMap["po"] = "ぽ";
        romaMap["ma"] = "ま";
        romaMap["mi"] = "み";
        romaMap["mu"] = "む";
        romaMap["me"] = "め";
        romaMap["mo"] = "も";
        romaMap["ya"] = "や";
        romaMap["yu"] = "ゆ";
        romaMap["yo"] = "よ";
        romaMap["ra"] = "ら";
        romaMap["ri"] = "り";
        romaMap["ru"] = "る";
        romaMap["re"] = "れ";
        romaMap["ro"] = "ろ";
        romaMap["wa"] = "わ";
        romaMap["wo"] = "を";
        romaMap["n"] = "ん";
        romaMap["kya"] = "きゃ";
        romaMap["kyu"] = "きゅ";
        romaMap["kyo"] = "きょ";
        romaMap["gya"] = "ぎゃ";
        romaMap["gyu"] = "ぎゅ";
        romaMap["gyo"] = "ぎょ";
        romaMap["sha"] = "しゃ";
        romaMap["shu"] = "しゅ";
        romaMap["sho"] = "しょ";
        romaMap["ja"] = "じゃ";
        romaMap["ju"] = "じゅ";
        romaMap["jo"] = "じょ";
        romaMap["cha"] = "ちゃ";
        romaMap["chu"] = "ちゅ";
        romaMap["cho"] = "ちょ";
        romaMap["nya"] = "にゃ";
        romaMap["nyu"] = "にゅ";
        romaMap["nyo"] = "にょ";
        romaMap["hya"] = "ひゃ";
        romaMap["hyu"] = "ひゅ";
        romaMap["hyo"] = "ひょ";
        romaMap["bya"] = "びゃ";
        romaMap["byu"] = "びゅ";
        romaMap["byo"] = "びょ";
        romaMap["pya"] = "ぴゃ";
        romaMap["pyu"] = "ぴゅ";
        romaMap["pyo"] = "ぴょ";
        romaMap["mya"] = "みゃ";
        romaMap["myu"] = "みゅ";
        romaMap["myo"] = "みょ";
        romaMap["rya"] = "りゃ";
        romaMap["ryu"] = "りゅ";
        romaMap["ryo"] = "りょ";
        romaMap["vu"] = "ゔ";
        romaMap["sakuon"] = "っ";
	    std::string resultStr = "";
        int romajiLength = static_cast<int>(romaji.length());
        int i = 0;
        while (i < romajiLength) {
            // n rule
            if (i+2 < romajiLength &&
                romaji[i] == 'n' &&
                romaji[i+1] == 'n' &&
                !romaMap.count(std::string(1, romaji[i+1]) + romaji[i+2])) {
                    resultStr += romaMap["sakuon"];
                    i += 1;
            }
            else {
                int checkLen = std::min(3, romajiLength-i);
                while (checkLen > 0) {
                    std::string checkStr = std::string(&romaji[i], &romaji[i+checkLen]);
                    if (romaMap.count(checkStr)) {
                        resultStr += romaMap[checkStr];
                        i += checkLen;
                        if (i < romajiLength) {
						    if (romaji[i] == 'o' && romaji[i-1] == 'o') {
                                resultStr += romaMap["u"];
                                i += 1;
                            }
                            else if (romaji[i] == 'e' && romaji[i-1] == 'e') {
                                resultStr += romaMap["i"];
                                i += 1;
                            }
                        }
                        break;
                    }
                    else if (checkLen == 1) {
                        if (i+1 < romajiLength) {
                            if (checkStr == std::string(1, romaji[i+1])) {
                                resultStr += romaMap["sakuon"];
                            }
                        }
                        i += 1;
                        break;
                    }
				    checkLen -= 1;
                }
            }
        }
        return resultStr;
    }

    void    InitMenu(PluginMenu &menu)
    {
        menu += new MenuEntry("Jisho", nullptr, [](MenuEntry *entry)
        {
            std::string dictPath = "romfs:/JMdict_smol.txt";
            if (!File::Exists(dictPath)) {
                MessageBox("Missing dict", "Please install " + dictPath + " to your memory card root")();
            } else {
                Keyboard keyboard;
                std::string output = "";
                keyboard.Open(output);
                std::string search = RomajiToHiragana(output);
                keyboard.Close();

                File dict;
                File::Open(dict, dictPath, File::READ);
                int bufferSize = 2000;
                char* buffer = new char[bufferSize];
                int result = dict.Read(buffer, bufferSize);
                int fileSize = dict.GetSize();
                int i = 0;
                std::string finding = "";
                int numMatchingLines = 0;
                std::string lastLine = "";
                while (i < fileSize) {
                    int readAmount = bufferSize;
                    bool lastIteration = false;
                    // Do not read more than the end of the file.
                    if (i + readAmount > fileSize) {
                        readAmount = fileSize - i;
                        lastIteration = true;
                    }
                    // Read into the buffer.
                    result = dict.Read(buffer, readAmount);
                    if (result != File::OPResult::SUCCESS) {
                        // @todo log
                        break;
                    }
                    std::vector<std::string> matchingLines;
                    bool shouldBail = false;
                    auto ss = std::stringstream{lastLine + buffer};
                    for (std::string line; std::getline(ss, line, '\n');) {
                        if (line.rfind(search, 0) == 0) {
                            matchingLines.push_back(line);
                        // If we had a match and this doesn't match, bail.
                        } else if (numMatchingLines > 0) {
                            shouldBail = true;
                        }
                        lastLine = line;
                    }
                    // Ignore the last line, we'll get to it next iteration.
                    if (!matchingLines.empty() && matchingLines.back() == lastLine) {
                        matchingLines.pop_back();
                    }
                    for (auto & matchingLine : matchingLines) {
                        if (numMatchingLines > 10) {
                            shouldBail = true;
                            break;
                        }
                        finding += matchingLine + "\n";
                        numMatchingLines += 1;
                    }
                    if (shouldBail) {
                        break;
                    }
                    i += readAmount;
                }
                dict.Close();
                MessageBox("Results for " + search, finding, DialogType::DialogOk, ClearScreen::Both)();
            }
        });
    }

    int     main(void)
    {
        PluginMenu *menu = new PluginMenu("Jisho", 0, 0, 1, "Search a Japanese dictionary without leaving your game.");

        menu->SynchronizeWithFrame(true);

        InitMenu(*menu);

        menu->Run();

        delete menu;

        return (0);
    }
}
