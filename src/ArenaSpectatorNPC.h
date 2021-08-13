#ifndef _ArenaSpectatorNPC_H_
#define _ArenaSpectatorNPC_H_

#include "BattlegroundMgr.h"
#include "Common.h"
#include "Player.h"

enum NpcSpectatorActions {
    NPC_SPECTATOR_ACTION_2V2_GAMES = 1000,
    NPC_SPECTATOR_ACTION_3V3_GAMES = 2000,
    NPC_SPECTATOR_ACTION_5V5_GAMES = 3000,
    NPC_SPECTATOR_ACTION_SPECIFIC = 500,
    NPC_SPECTATOR_ACTION_SELECTED_PLAYER = 4000
};

class ArenaSpectatorNPC
{
public:
    static ArenaSpectatorNPC* instance();

    std::string GetClassIconById(uint8 id);
    std::string GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo, std::string firstTeamName, std::string secondTeamName);
    ObjectGuid GetFirstPlayerGuid(Battleground* team);
    std::string GetMatchCount(uint8 type);
    void ShowPage(Player* player, uint16 page, uint32 IsTop);

private:
    const uint8 GamesOnPage = 14;
};

#define sArenaSpectatorNPC ArenaSpectatorNPC::instance()

#endif
