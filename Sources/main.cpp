#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include "Helpers/Wrappers.hpp"
#include <string>
#include <vector>
#include "DictSearch.hpp"
#include "Romaji.hpp"

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

    void    InitMenu(PluginMenu &menu)
    {
        menu += new MenuEntry("Jisho", nullptr, [](MenuEntry *entry)
        {
            std::string dictPath = "romfs:/JMdict_smol.txt";
            if (!File::Exists(dictPath)) {
                MessageBox("Missing dict", "Please install JMdict_smol.txt to your memory card root")();
                return;
            }

            const size_t maxResults = 20;

            Keyboard keyboard;
            std::string output = "";
            if (keyboard.Open(output) != 0) {
                return;
            }
            keyboard.Close();

            std::string search = Jisho::RomajiToHiragana(output);
            if (search == "") {
                return;
            }

            File dict;
            if (File::Open(dict, dictPath, File::READ) != File::OPResult::SUCCESS) {
                MessageBox("Error", "Failed to open the dictionary")();
                return;
            }

            auto reader = [&dict](uint64_t offset, char *buffer, size_t length) -> size_t {
                // Seek defaults to CUR (relative). The search needs absolute offsets.
                if (dict.Seek(static_cast<s64>(offset), File::SET) != File::OPResult::SUCCESS) {
                    return 0;
                }
                if (dict.Read(buffer, static_cast<u32>(length)) != File::OPResult::SUCCESS) {
                    return 0;
                }
                return length;
            };
            std::vector<std::string> matches = Jisho::SearchPrefix(reader, dict.GetSize(), search, maxResults);
            dict.Close();

            std::string finding = "";
            for (const auto &match : matches) {
                finding += match + "\n";
            }
            if (finding == "") {
                finding = "No results";
            }
            MessageBox("Results for " + search, finding, DialogType::DialogOk, ClearScreen::Both)();
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
