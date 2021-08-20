#include "ArenaSpectatorNPC.h"
#include "ArenaTeamMgr.h"
#include "ScriptedGossip.h"
#include "ObjectMgr.h"

ArenaSpectatorNPC* ArenaSpectatorNPC::instance()
{
    static ArenaSpectatorNPC instance;
    return &instance;
}

std::string ArenaSpectatorNPC::GetClassIconById(uint8 id) {
    std::string sClass = "";
    switch (id) {
        case CLASS_WARRIOR:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:0:1:0:1|t|r";
            break;
        case CLASS_PALADIN:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:0:1:2:3|t|r";
            break;
        case CLASS_HUNTER:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:0:1:1:2|t|r";
            break;
        case CLASS_ROGUE:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:2:3:0:1|t|r";
            break;
        case CLASS_PRIEST:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:2:3:1:2|t|r";
            break;
        case CLASS_DEATH_KNIGHT:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:1:2:2:3|t|r";
            break;
        case CLASS_SHAMAN:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:1:2:1:2|t|r";
            break;
        case CLASS_MAGE:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:1:2:0:1|t|r";
            break;
        case CLASS_WARLOCK:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:3:4:1:2|t|r";
            break;
        case CLASS_DRUID:
            sClass = "|TInterface\\WorldStateFrame\\Icons-Classes:13:13:0:0:4:4:3:4:0:1|t|r";
            break;
    }
    return sClass;
}

std::string ArenaSpectatorNPC::GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo, std::string firstTeamName, std::string secondTeamName) {
    std::string teamsMember[BG_TEAMS_COUNT];
    uint32 firstTeamId = 0;
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
        if (Player * player = ObjectAccessor::FindPlayer(itr->first)) {
            if (player->IsSpectator())
                continue;

            if (player->IsGameMaster())
                continue;

            uint32 team = itr->second->GetBgTeamId();
            if (!firstTeamId)
                firstTeamId = team;

            teamsMember[firstTeamId == team] += GetClassIconById(player->getClass());
        }

    std::string data = teamsMember[0] + "" + secondTeamName + " (";
    std::stringstream ss;
    std::stringstream sstwo;
    ss << mmr;
    sstwo << mmrTwo;
    data += sstwo.str();
    data += ") - ";
    data += "(" + ss.str();
    data += ") " + firstTeamName + "" + teamsMember[1];
    return data;
}

ObjectGuid ArenaSpectatorNPC::GetFirstPlayerGuid(Battleground* team) {
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin();
         itr != team->GetPlayers().end(); ++itr)
        if (Player * player = ObjectAccessor::FindPlayer(itr->first))
            return itr->first;
    return ObjectGuid::Empty;
}

std::string ArenaSpectatorNPC::GetMatchCount(uint8 type) {
    const BattlegroundContainer& bgList = sBattlegroundMgr->GetBattlegroundList();
    uint16 i = 0;

    for (auto itr : bgList)
    {
        Battleground* bg = itr.second;
        if (BattlegroundMgr::IsArenaType(bg->GetBgTypeID()) && bg->GetArenaType() == type && bg->isRated())
            i++;
    }

    return std::to_string(i);
}

void ArenaSpectatorNPC::GetMatchInformation(Battleground* arena, Player* target, uint32& firstTeamId, std::string& firstTeamName, std::string& secondTeamName, uint16& mmr, uint16& mmrTwo) {
    uint8 slot;

    switch (arena->GetArenaType())
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

    firstTeamId = target->GetArenaTeamId(slot);
    firstTeamName = (sArenaTeamMgr->GetArenaTeamById(firstTeamId))->GetName();
    Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
    for (; citr != arena->GetPlayers().end(); ++citr)
    {
        if (Player * plrs = ObjectAccessor::FindPlayer(citr->first)) {
            if (plrs->GetArenaTeamId(slot) != firstTeamId) {
                mmrTwo = arena->GetArenaMatchmakerRating(citr->second->GetBgTeamId());
                secondTeamName = (sArenaTeamMgr->GetArenaTeamById(plrs->GetArenaTeamId(0)))->GetName();
            } else if (plrs->GetArenaTeamId(slot) == firstTeamId) {
                mmr = arena->GetArenaMatchmakerRating(citr->second->GetBgTeamId());
            }
        }
    }
}

void ArenaSpectatorNPC::ShowPage(Player* player, uint16 page, uint32 IsTop) {
    uint32 firstTeamId = 0;
    uint16 TypeOne = 0;
    uint16 TypeTwo = 0;
    uint16 TypeThree = 0;
    uint16 mmr = 0;
    uint16 mmrTwo = 0;
    std::string firstTeamName = "";
    std::string secondTeamName = "";
    bool hasNextPage = false;
    const BattlegroundContainer& bgList = sBattlegroundMgr->GetBattlegroundList();
    BattlegroundContainer arenas;

    for (auto itr : bgList)
    {
        Battleground* bg = itr.second;
        if (BattlegroundMgr::IsArenaType(bg->GetBgTypeID()))
            arenas.insert(itr);
    }

    if (arenas.empty())
        return;

    for (BattlegroundContainer::const_iterator itr = arenas.begin(); itr != arenas.end(); ++itr) {
        Battleground* arena = itr->second;
        Player *target = ObjectAccessor::FindPlayer(GetFirstPlayerGuid(arena));

        if (!arena->GetPlayersSize())
            continue;

        if (!arena->isRated()) {
            continue;
        }

        GetMatchInformation(arena, target, firstTeamId, firstTeamName, secondTeamName, mmr, mmrTwo);

        if (IsTop == 2 && arena->GetArenaType() == ARENA_TYPE_2v2) {
            TypeOne++;
            if (TypeOne > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeOne >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        } else if (IsTop == 3 && arena->GetArenaType() == ARENA_TYPE_3v3) {
            TypeTwo++;
            if (TypeTwo > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeTwo >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        } else if (IsTop == 5 && arena->GetArenaType() == ARENA_TYPE_5v5) {
            TypeThree++;
            if (TypeThree > (page + 1) * GamesOnPage) {
                hasNextPage = true;
                break;
            }
            if (TypeThree >= page * GamesOnPage)
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo, firstTeamName, secondTeamName), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena).GetCounter());
        }
    }

    if (page > 0) {
        if (IsTop == 2)
            AddGossipItemFor(player, 7, "<- Back", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page - 1);
        if (IsTop == 3)
            AddGossipItemFor(player, 7, "<- Back", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page - 1);
        if (IsTop == 5)
            AddGossipItemFor(player, 7, "<- Back", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES + page - 1);
    }

    if (hasNextPage) {
        if (IsTop == 2)
            AddGossipItemFor(player, 7, "Next ->", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page + 1);
        if (IsTop == 3)
            AddGossipItemFor(player, 7, "Next ->", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page + 1);
        if (IsTop == 5)
            AddGossipItemFor(player, 7, "Next ->", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES + page + 1);
    }
}