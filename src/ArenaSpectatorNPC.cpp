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
#include "ScriptedGossip.h"
#include "ObjectMgr.h"
#include "Config.h"


ArenaSpectatorNPC* ArenaSpectatorNPC::instance()
{
    static ArenaSpectatorNPC instance;
    return &instance;
}

std::string ArenaSpectatorNPC::GetClassIconById(uint8 id) {
    switch (id) {
        case CLASS_WARRIOR:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:0:1:0:1|t|r";
        case CLASS_PALADIN:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:0:1:2:3|t|r";
        case CLASS_HUNTER:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:0:1:1:2|t|r";
        case CLASS_ROGUE:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:2:3:0:1|t|r";
        case CLASS_PRIEST:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:2:3:1:2|t|r";
        case CLASS_DEATH_KNIGHT:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:1:2:2:3|t|r";
        case CLASS_SHAMAN:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:1:2:1:2|t|r";
        case CLASS_MAGE:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:1:2:0:1|t|r";
        case CLASS_WARLOCK:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:3:4:1:2|t|r";
        case CLASS_DRUID:
            return "|TInterface\\WorldStateFrame\\Icons-Classes:15:13:0:0:4:4:3:4:0:1|t|r";
        default:
            return "";
    }
}


std::string ArenaSpectatorNPC::GetRaceIconById(uint8 id, uint8 gender) {
    const std::string gender_icon = gender == GENDER_MALE ? "male" : "female";
    switch (id) {
        case RACE_HUMAN:
            return "|TInterface/ICONS/achievement_character_human_" + gender_icon + ":15|t";
        case RACE_ORC:
            return "|TInterface/ICONS/achievement_character_orc_" + gender_icon + ":15|t";
        case RACE_DWARF:
            return "|TInterface/ICONS/achievement_character_dwarf_" + gender_icon + ":15|t";
        case RACE_NIGHTELF:
            return "|TInterface/ICONS/achievement_character_nightelf_" + gender_icon + ":15|t";
        case RACE_UNDEAD_PLAYER:
            return "|TInterface/ICONS/achievement_character_undead_" + gender_icon + ":15|t";
        case RACE_TAUREN:
            return "|TInterface/ICONS/achievement_character_tauren_" + gender_icon + ":15|t";
        case RACE_GNOME:
            return "|TInterface/ICONS/achievement_character_gnome_" + gender_icon + ":15|t";
        case RACE_TROLL:
            return "|TInterface/ICONS/achievement_character_troll_" + gender_icon + ":15|t";
        case RACE_BLOODELF:
            return "|TInterface/ICONS/achievement_character_bloodelf_" + gender_icon + ":15|t";
        case RACE_DRAENEI:
            return "|TInterface/ICONS/achievement_character_draenei_" + gender_icon + ":15|t";
        default:
            return "";
    }
}

std::string ArenaSpectatorNPC::GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo, std::string firstTeamName, std::string secondTeamName) {
    std::string teamsMember[PVP_TEAMS_COUNT];
    uint32 firstTeamId = 0;

    const uint32 _ARENA_TYPE_3V3_SOLO = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.ArenaType", 4);
    std::string tmpTeamName[PVP_TEAMS_COUNT];

    for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
        if (Player * player = ObjectAccessor::FindPlayer(itr->first)) {
            if (player->IsSpectator())
                continue;

            if (player->IsGameMaster())
                continue;

            uint32 team = itr->second->GetBgTeamId();

            if (!firstTeamId)
                firstTeamId = team;

            const auto idx = team == 0 ? 0 : 1;
            teamsMember[idx] += GetClassIconById(player->getClass());
            teamsMember[idx] += GetRaceIconById(player->getRace(), player->getGender());

            tmpTeamName[idx] += player->GetName() + " ";
        }

    // override team names only for unrated and 3v3soloQ
    if (!team->isRated() || team->GetArenaType() == _ARENA_TYPE_3V3_SOLO) {
        firstTeamName = tmpTeamName[0];
        secondTeamName = tmpTeamName[1];
    }

    std::string data = " " + teamsMember[1] + " " + secondTeamName + " (";
    std::stringstream ss;
    std::stringstream sstwo;
    ss << mmr;
    sstwo << mmrTwo;
    data += sstwo.str();
    data += ") vs";
    data += " " + teamsMember[0] + " " + firstTeamName + " (";
    data += ss.str();
    data += ")";
    return data;
}

ObjectGuid ArenaSpectatorNPC::GetFirstPlayerGuid(Battleground* team) {
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin();
         itr != team->GetPlayers().end(); ++itr)
        if (Player * player = ObjectAccessor::FindPlayer(itr->first))
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

void ArenaSpectatorNPC::GetMatchInformation(Battleground* arena, Player* target, uint32& firstTeamId, std::string& firstTeamName, std::string& secondTeamName, uint16& mmr, uint16& mmrTwo) {
    uint8 slot;

    const uint32 ARENA_1v1_SLOT_ID = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.1v1.SlotID", 3);
    const uint32 ARENA_3V3_SOLO_SLOT_ID = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.SlotID", 4);
    const uint32 _ARENA_TYPE_1v1 = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.1v1.ArenaType", 1);
    const uint32 _ARENA_TYPE_3V3_SOLO = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.ArenaType", 4);

    const auto arenaType = arena->GetArenaType();

    if (arenaType == _ARENA_TYPE_1v1) {
        slot = ARENA_1v1_SLOT_ID;
    } else if (arenaType == _ARENA_TYPE_3V3_SOLO) {
        slot = ARENA_3V3_SOLO_SLOT_ID;
    } else {
        switch (arenaType)
        {
            case ARENA_TYPE_2v2:
                slot = 0;
                break;
            case ARENA_TYPE_3v3:
                slot = 1;
                break;
            case ARENA_TYPE_5v5:
                slot = 2;
                break;
            default:
                return;
        }
    }

    if (arena->isRated())
    {
        firstTeamId = target->GetArenaTeamId(slot);
        firstTeamName = (sArenaTeamMgr->GetArenaTeamById(firstTeamId))->GetName();

        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
        for (; citr != arena->GetPlayers().end(); ++citr)
        {
            if (Player * plrs = ObjectAccessor::FindPlayer(citr->first)) {
                if (plrs->GetArenaTeamId(slot) != firstTeamId) {
                    mmrTwo = arena->GetArenaMatchmakerRating(citr->second->GetBgTeamId());
                    secondTeamName = (sArenaTeamMgr->GetArenaTeamById(plrs->GetArenaTeamId(slot)))->GetName();
                } else if (plrs->GetArenaTeamId(slot) == firstTeamId) {
                    mmr = arena->GetArenaMatchmakerRating(citr->second->GetBgTeamId());
                }
            }
        }
    }
    else if (sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false))
    {
        firstTeamId = target->GetArenaTeamId(slot);
        firstTeamName = "";
        secondTeamName = "";

        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
        for (; citr != arena->GetPlayers().end(); ++citr)
        {
            if (Player * plrs = ObjectAccessor::FindPlayer(citr->first)) {
                if (plrs->GetArenaTeamId(slot) != firstTeamId) {
                    if (secondTeamName != "")
                        secondTeamName += ", ";
                    secondTeamName += plrs->GetName();

                } else if (plrs->GetArenaTeamId(slot) == firstTeamId) {
                    if (firstTeamName != "")
                        firstTeamName += ", ";
                    firstTeamName += plrs->GetName();
                }
            }
        }
    }
}

void ArenaSpectatorNPC::ShowPage(Player* player, uint16 page, uint32 arenaType) {
    uint32 firstTeamId = 0;
    uint16 TypeOne = 0;
    uint16 TypeTwo = 0;
    uint16 TypeThree = 0;
    uint16 TypeFour = 0;
    uint16 Type1v1 = 0;
    uint16 mmr = 0;
    uint16 mmrTwo = 0;
    std::string firstTeamName = "";
    std::string secondTeamName = "";
    bool hasNextPage = false;
    uint16 currentPage;

    const uint32 _ARENA_TYPE_1v1 = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.1v1.ArenaType", 1);
    const uint32 _ARENA_TYPE_3V3_SOLO = sConfigMgr->GetOption<uint32>("NpcArenaSpectator.3v3soloQ.ArenaType", 4);

    if (_bgMap.empty())
        return;

    for (auto& itr : _bgMap) {
        Battleground* arena = itr.second;
        Player *target = ObjectAccessor::FindPlayer(GetFirstPlayerGuid(arena));

        if (isReplay(arena))
            continue;

        if (!arena->GetPlayersSize())
            continue;

        if (!sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false) && !arena->isRated()) {
            continue;
        }

        GetMatchInformation(arena, target, firstTeamId, firstTeamName, secondTeamName, mmr, mmrTwo);

        const std::string prefix = arena->isRated() && sConfigMgr->GetOption<bool>("NpcArenaSpectator.ShowUnrated", false) ? "|TInterface\\icons\\achievement_arena_2v2_7:13|t" : "";

        if (arenaType == ARENA_TYPE_2v2 && arena->GetArenaType() == ARENA_TYPE_2v2) {
            TypeOne++;
            if (TypeOne > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeOne >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }
        else if (arenaType == ARENA_TYPE_3v3 && arena->GetArenaType() == ARENA_TYPE_3v3) {
            TypeTwo++;
            if (TypeTwo > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeTwo >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }
        else if (arenaType == _ARENA_TYPE_3V3_SOLO && arena->GetArenaType() == _ARENA_TYPE_3V3_SOLO)
        {
            TypeFour++;
            if (TypeFour > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeFour >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }
        else if (arenaType == _ARENA_TYPE_1v1 && arena->GetArenaType() == _ARENA_TYPE_1v1) {
            Type1v1++;
            if (Type1v1 > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (Type1v1 >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }
        else if (arenaType == ARENA_TYPE_5v5 && arena->GetArenaType() == ARENA_TYPE_5v5) {
            TypeThree++;
            if (TypeThree > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeThree >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, prefix + GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }

    }

    if (arenaType == _ARENA_TYPE_1v1) {
        currentPage = NPC_SPECTATOR_ACTION_1v1_GAMES + page;
    } else if (arenaType == _ARENA_TYPE_3V3_SOLO) {
        currentPage = NPC_SPECTATOR_ACTION_3V3SOLO_GAMES + page;
    } else {
        switch (arenaType)
        {
            case ARENA_TYPE_2v2:
                currentPage = NPC_SPECTATOR_ACTION_2V2_GAMES + page;
                break;
            case ARENA_TYPE_3v3:
                currentPage = NPC_SPECTATOR_ACTION_3V3_GAMES + page;
                break;
            case ARENA_TYPE_5v5:
                currentPage = NPC_SPECTATOR_ACTION_5V5_GAMES + page;
                break;
            default:
                return;
        }
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
