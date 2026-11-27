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

#include "ArenaSpectatorNPC.h"
#include "ArenaTeamMgr.h"
#include "Config.h"
#include "ObjectMgr.h"
#include "ScriptedGossip.h"


ArenaSpectatorNPC* ArenaSpectatorNPC::instance()
{
    static ArenaSpectatorNPC instance;
    return &instance;
}

std::string ArenaSpectatorNPC::GetClassIconById(uint8 id) {
    switch (id)
    {
        case CLASS_WARRIOR:
            return "|TInterface\\icons\\inv_sword_27:13|t";
        case CLASS_PALADIN:
            return "|TInterface\\icons\\inv_hammer_01:13|t";
        case CLASS_HUNTER:
            return "|TInterface\\icons\\inv_weapon_bow_07:13|t";
        case CLASS_ROGUE:
            return "|TInterface\\icons\\inv_throwingknife_04:13|t";
        case CLASS_PRIEST:
            return "|TInterface\\icons\\inv_staff_30:13|t";
        case CLASS_DEATH_KNIGHT:
            return "|TInterface\\icons\\spell_deathknight_classicon:13|t";
        case CLASS_SHAMAN:
            return "|TInterface\\icons\\inv_jewelry_talisman_04:13|t";
        case CLASS_MAGE:
            return "|TInterface\\icons\\inv_staff_13:13|t";
        case CLASS_WARLOCK:
            return "|TInterface\\icons\\spell_nature_drowsy:13|t";
        case CLASS_DRUID:
            return "|TInterface\\icons\\inv_misc_monsterclaw_04:13|t";
        default:
            return "";
    }
}


std::string ArenaSpectatorNPC::GetRaceIconById(uint8 id, uint8 gender) {
    const std::string gender_icon = gender == GENDER_MALE ? "male" : "female";
    switch (id) {
        case RACE_HUMAN:
            return "|TInterface/ICONS/achievement_character_human_" + gender_icon + ":13|t";
        case RACE_ORC:
            return "|TInterface/ICONS/achievement_character_orc_" + gender_icon + ":13|t";
        case RACE_DWARF:
            return "|TInterface/ICONS/achievement_character_dwarf_" + gender_icon + ":13|t";
        case RACE_NIGHTELF:
            return "|TInterface/ICONS/achievement_character_nightelf_" + gender_icon + ":13|t";
        case RACE_UNDEAD_PLAYER:
            return "|TInterface/ICONS/achievement_character_undead_" + gender_icon + ":13|t";
        case RACE_TAUREN:
            return "|TInterface/ICONS/achievement_character_tauren_" + gender_icon + ":13|t";
        case RACE_GNOME:
            return "|TInterface/ICONS/achievement_character_gnome_" + gender_icon + ":13|t";
        case RACE_TROLL:
            return "|TInterface/ICONS/achievement_character_troll_" + gender_icon + ":13|t";
        case RACE_BLOODELF:
            return "|TInterface/ICONS/achievement_character_bloodelf_" + gender_icon + ":13|t";
        case RACE_DRAENEI:
            return "|TInterface/ICONS/achievement_character_draenei_" + gender_icon + ":13|t";
        default:
            return "";
    }
}

std::string ArenaSpectatorNPC::GetGamesStringData(Battleground* arena)
{
    std::string icons[PVP_TEAMS_COUNT];
    std::string names[PVP_TEAMS_COUNT];

    const uint32 _ARENA_TYPE_3V3_SOLO = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.ArenaType", 4);

    for (auto const& itr : arena->GetPlayers())
    {
        Player* player = ObjectAccessor::FindPlayer(itr.first);
        if (!player || player->IsSpectator() || player->IsGameMaster())
            continue;

        TeamId const side = player->GetBgTeamId();
        icons[side] += GetClassIconById(player->getClass());
        icons[side] += GetRaceIconById(player->getRace(), player->getGender());

        if (!names[side].empty())
            names[side] += " ";

        names[side] += player->GetName();
    }

    // Rated names, like the ratings, come from the arena's per-side records, so they stay right after players leave
    if (arena->isRated() && arena->GetArenaType() != _ARENA_TYPE_3V3_SOLO)
    {
        for (TeamId const side : { TEAM_ALLIANCE, TEAM_HORDE })
        {
            ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(arena->GetArenaTeamIdForTeam(side));
            names[side] = team ? team->GetName() : "";
        }
    }

    return " " + icons[TEAM_HORDE] + " " + names[TEAM_HORDE] + " ("
        + std::to_string(arena->GetArenaMatchmakerRating(TEAM_HORDE)) + ") vs"
        + " " + icons[TEAM_ALLIANCE] + " " + names[TEAM_ALLIANCE] + " ("
        + std::to_string(arena->GetArenaMatchmakerRating(TEAM_ALLIANCE)) + ")";
}

ObjectGuid ArenaSpectatorNPC::GetFirstPlayerGuid(Battleground* team) {
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin();
         itr != team->GetPlayers().end(); ++itr)
        if (ObjectAccessor::FindPlayer(itr->first))
            return itr->first;
    return ObjectGuid::Empty;
}

bool isReplay(Battleground* arena) {
    Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
    for (; citr != arena->GetPlayers().end(); ++citr)
    {
        if (Player * plrs = ObjectAccessor::FindPlayer(citr->first)) {
            if (!plrs->IsSpectator()) {
                return false;
            }
        }
    }

    return true;
}

std::string ArenaSpectatorNPC::GetMatchCount(uint8 type) {
    uint16 i = 0;

    for (auto& itr : _bgMap)
    {
        Battleground* bg = itr.second;
        if (BattlegroundMgr::IsArenaType(bg->GetBgTypeID()) && bg->GetArenaType() == type && !isReplay(bg))
        {
            if (sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false))
                i++;
            else if (bg->isRated())
                i++;
        }
    }

    return std::to_string(i);
}

// Gossip action of the games list of the given arena type, 0 when the type has no list.
// The 1v1 and 3v3 soloQ types are configurable, so they are matched before the stock ones.
static uint32 GetGamesListAction(uint32 arenaType) {
    if (arenaType == sConfigMgr->GetOption<uint32>("NpcArenaSpectator.1v1.ArenaType", 1))
        return NPC_SPECTATOR_ACTION_1v1_GAMES;

    if (arenaType == sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.ArenaType", 4))
        return NPC_SPECTATOR_ACTION_3V3SOLO_GAMES;

    switch (arenaType)
    {
        case ARENA_TYPE_2v2:
            return NPC_SPECTATOR_ACTION_2V2_GAMES;
        case ARENA_TYPE_3v3:
            return NPC_SPECTATOR_ACTION_3V3_GAMES;
        case ARENA_TYPE_5v5:
            return NPC_SPECTATOR_ACTION_5V5_GAMES;
        default:
            return 0;
    }
}

void ArenaSpectatorNPC::ShowPage(Player* player, uint16 page, uint32 arenaType) {
    uint16 games = 0;
    bool hasNextPage = false;

    const uint32 gamesListAction = GetGamesListAction(arenaType);

    if (!gamesListAction || _bgMap.empty())
        return;

    const uint32 currentPage = gamesListAction + page;

    for (auto& itr : _bgMap) {
        Battleground* arena = itr.second;

        if (arena->GetArenaType() != arenaType)
            continue;

        if (isReplay(arena))
            continue;

        if (!arena->GetPlayersSize())
            continue;

        if (!sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false) && !arena->isRated()) {
            continue;
        }

        games++;

        if (games > (page + 1) * GamesOnPage) {
            hasNextPage = true;
            break;
        }

        if (games < page * GamesOnPage)
            continue;

        const std::string prefix = arena->isRated() && sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false) ? "|TInterface\\icons\\achievement_arena_2v2_7:13|t" : "";

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena), GOSSIP_SENDER_MAIN,
                         NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
    }

    if (page > 0)
    {
        AddGossipItemFor(player, 7, "<- Back", GOSSIP_SENDER_MAIN, currentPage - 1);
    }

    if (hasNextPage)
    {
        AddGossipItemFor(player, 7, "Next ->", GOSSIP_SENDER_MAIN, currentPage + 1);
    }
}

void ArenaSpectatorNPC::AddBGToMap(Battleground* bg)
{
    if (!bg)
        return;

    _bgMap[bg->GetInstanceID()] = bg;
}

void ArenaSpectatorNPC::RemoveBGFromMap(Battleground* bg)
{
    _bgMap.erase(bg->GetInstanceID());
}

void ArenaSpectatorNPC::ClearBGMap()
{
    _bgMap.clear();
}
