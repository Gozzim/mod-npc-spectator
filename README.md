# This is a module for  ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## NPC for arena spectating
[![core-build](https://github.com/Gozzim/mod-npc-spectator/actions/workflows/core-build.yml/badge.svg)](https://github.com/Gozzim/mod-npc-spectator)
[![core-version](https://img.shields.io/badge/core--rev-5143872-ff0000.svg?labelColor=2e2c2c&logo=data:image/png;base64,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)](https://github.com/azerothcore/azerothcore-wotlk/commit/5143872aed7e93ee2ff8658c7c8b6efd7702429c)
[![CodeFactor](https://www.codefactor.io/repository/github/gozzim/mod-npc-spectator/badge)](https://www.codefactor.io/repository/github/gozzim/mod-npc-spectator)

## Description
NPC for players to use the arena spectating feature in a more elegant way than using a command.

## Requirements
Requires at least revision [azerothcore-wotlk@5143872](https://github.com/azerothcore/azerothcore-wotlk/commit/5143872aed7e93ee2ff8658c7c8b6efd7702429c) of AzerothCore

## Installation
```
1) Simply place the module under the `modules` directory of your AzerothCore source. 
2) Re-run cmake and launch a clean build of AzerothCore.
```

## Usage

`.npc add 190000`

![image](https://github.com/user-attachments/assets/c6b6b757-183e-4a82-9a8c-8adc5299efb3)


## Credits
- NPC for AzerothCore edited by [Gozzim](https://github.com/Gozzim)
- Original Code by [Flameshot](https://github.com/Flameshot/TrinityCore/tree/Arena-Spectator)
- Originally written for [TrinityCore](https://github.com/TrinityCore/TrinityCore)

## License
This code and content is released under the [GPL 2.0 license](https://github.com/Gozzim/mod-npc-spectator/blob/master/LICENSE).
