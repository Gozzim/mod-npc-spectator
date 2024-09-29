/*
 *  Module for AzerothCore edited by Gozzim (https://github.com/Gozzim)
 *  Original code:
 *      https://github.com/Flameshot/TrinityCore/tree/Arena-Spectator
 *      Committed by Flameshot (https://github.com/Flameshot)
 *      Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *      Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _ArenaSpectatorNPC_H_
#define _ArenaSpectatorNPC_H_

#include "BattlegroundMgr.h"
#include "Common.h"
#include "Player.h"
#include "Config.h"
#include "WorldScript.h"

#define NPC_TEXT_id_SPECTATOR 190017

enum NpcSpectatorActions {
    NPC_SPECTATOR_ACTION_MAIN_MENU = 100,
    NPC_SPECTATOR_ACTION_HELP = 200,
    NPC_SPECTATOR_ACTION_1v1_GAMES = 900,
    NPC_SPECTATOR_ACTION_2V2_GAMES = 1000,
    NPC_SPECTATOR_ACTION_3V3_GAMES = 2000,
    NPC_SPECTATOR_ACTION_5V5_GAMES = 3000,
    NPC_SPECTATOR_ACTION_3V3SOLO_GAMES = 4000,
    NPC_SPECTATOR_ACTION_SPECIFIC = 500,
    NPC_SPECTATOR_ACTION_SELECTED_PLAYER = 5000
};

class ConfigLoaderNpcSpectator : public WorldScript
{
public:
    ConfigLoaderNpcSpectator() : WorldScript("config_loader_npc_spectator") { }
};

class ArenaSpectatorNPC
{
public:
    static ArenaSpectatorNPC* instance();

    std::string GetClassIconById(uint8 id);
    std::string GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo, std::string firstTeamName, std::string secondTeamName);
    ObjectGuid GetFirstPlayerGuid(Battleground* team);
    std::string GetMatchCount(uint8 type);
    void GetMatchInformation(Battleground* arena, Player* target, uint32& firstTeamId, std::string& firstTeamName, std::string& secondTeamName, uint16& mmr, uint16& mmrTwo);
    void ShowPage(Player* player, uint16 page, uint32 IsTop);
    void AddBGToMap(Battleground* bg);
    void RemoveBGFromMap(Battleground* bg);
    void ClearBGMap();

private:
    const uint8 GamesOnPage = 14;

    typedef std::unordered_map<uint32, Battleground*> BGMap;
    BGMap _bgMap;
};

#define sArenaSpectatorNPC ArenaSpectatorNPC::instance()

#endif
