# jisho-3gx

<img src="https://github.com/mortenson/jisho-3gx/assets/2091002/8668bdd6-a247-4bf3-ac46-ac67cc447c1d" alt="A screenshot of the Jisho plugin" width="500px">

A Luma3DS plugin that lets you search a Japanese dictionary while playing 3DS games.

Useful for people learning Japanese by playing Japanese 3DS games.

Note that words using katakana are not included in the dictionary for space and UX reasons.

This plugin is built with https://gitlab.com/thepixellizeross/ctrpluginframework

# Installation

A recent release of Luma3DS is required to use this plugin.

1. Download artifacts from the latest release, latest workflow run, or run a build
2. Copy `Jisho.3gx` to `luma/plugins/default.3gx` or `luma/plugins/<title id>.3gx`, depending on when you want it loaded
3. Copy `JMdict_smol.txt` to the root of your SD card
4. Enable the plugin loader using Rosalina
5. Launch the relevant game you want to use the dictionary with
6. While the game is launched, press Select (you can change this in the CTRPluginFramework settings later)
7. Press A and enter search for a word using romaji, then tap the enter button
8. Scroll through results using the up and down arrows
9. Press Select to close the plugin

The plugin might crash some games, feel free to disable the plugin loader when needed if using it as a default plugin.

# Development (Linux)

Note that I don't know C++ very well, and know very little about 3DS development. As a result, code may seem strange and wrong.

1. Install `devkitpro`
2. Run `sudo dkp-pacman -S 3ds-dev libctrpf`
3. Run `make clean && make all`
4. Copy `Jisho.3gx` to the destination of your choice. I use the Citra emulator for testing

# Building a new dictionary

1. Install Python 3
2. Install the `xmltodict` module if not already installed
3. Download `JMdict_e.xml` from some EDICT mirror, for example http://ftp.edrdg.org/pub/Nihongo/JMdict_e
4. Run `python3 simplify.py`
