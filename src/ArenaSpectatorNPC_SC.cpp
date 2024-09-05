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

#include "AccountMgr.h"
#include "ArenaSpectator.h"
#include "ArenaSpectatorNPC.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "World.h"

class ArenaSpectatorNPC_BG : public BGScript
{
public:
    ArenaSpectatorNPC_BG() : BGScript("ArenaSpectatorNPC_BGScript") { }

    void OnBattlegroundCreate(Battleground* bg) {
        if (bg->isArena())
        {
            sArenaSpectatorNPC->AddBGToMap(bg);
        }
    }

    void OnBattlegroundDestroy(Battleground* bg) {
        sArenaSpectatorNPC->RemoveBGFromMap(bg);
    }
};

class ArenaSpectatorNPC_Creature : public CreatureScript
{
public:
    ArenaSpectatorNPC_Creature() : CreatureScript("ArenaSpectatorNPC_Creature") { }

    bool OnGossipHello(Player *player, Creature* creature) override {
        if (!player || !creature)
            return true;

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\icons\\Achievement_Arena_2v2_4:16|t 2v2 (|cffff0000" + sArenaSpectatorNPC->GetMatchCount(ARENA_TYPE_2v2) + "|r in progress)", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\icons\\Achievement_Arena_3v3_4:16|t 3v3 (|cffff0000" + sArenaSpectatorNPC->GetMatchCount(ARENA_TYPE_3v3) + "|r in progress)", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
        // To do: config Show.5v5.Games
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\icons\\Achievement_Arena_5v5_4:16|t 5v5 (|cffff0000" + sArenaSpectatorNPC->GetMatchCount(ARENA_TYPE_5v5) + "|r in progress)", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES);
        // To do: config Show.3v3Solo.Games
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\icons\\Achievement_Arena_3v3_4:16|t 3v3 Solo (|cffff0000" + sArenaSpectatorNPC->GetMatchCount(4) + "|r in progress)", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3SOLO_GAMES);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_boss_cthun:16|tSpectate Specific Player", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC, "", 0, true);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\PaperDollInfoFrame\\UI-GearManager-Undo:16|tRefresh", GOSSIP_SENDER_MAIN, 100);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\gossipframe\\dailyactivequesticon:16|tHelp", GOSSIP_SENDER_MAIN, 200);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override {
        if (!player || !creature)
            return true;

        ClearGossipMenuFor(player);

        if (action == 100) {
            OnGossipHello(player, creature);
        }

        if (action >= NPC_SPECTATOR_ACTION_2V2_GAMES && action < NPC_SPECTATOR_ACTION_3V3_GAMES) {
            AddGossipItemFor(player, 11, "< Main Menu >", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 4, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
            sArenaSpectatorNPC->ShowPage(player, action - NPC_SPECTATOR_ACTION_2V2_GAMES, ARENA_TYPE_2v2);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        } else if (action >= NPC_SPECTATOR_ACTION_3V3_GAMES && action < NPC_SPECTATOR_ACTION_5V5_GAMES) {
            AddGossipItemFor(player, 11, "< Main Menu >", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 4, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
            sArenaSpectatorNPC->ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3_GAMES, ARENA_TYPE_3v3);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        // 5v5
        //else if (action >= NPC_SPECTATOR_ACTION_5V5_GAMES && action < NPC_SPECTATOR_ACTION_SELECTED_PLAYER) {
        else if (action >= NPC_SPECTATOR_ACTION_5V5_GAMES && action < NPC_SPECTATOR_ACTION_3V3SOLO_GAMES) {
            AddGossipItemFor(player, 11, "< Main Menu >", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 4, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES);
            sArenaSpectatorNPC->ShowPage(player, action - NPC_SPECTATOR_ACTION_5V5_GAMES, ARENA_TYPE_5v5);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        // 3v3 Solo
        else if (action >= NPC_SPECTATOR_ACTION_3V3SOLO_GAMES && action < NPC_SPECTATOR_ACTION_SELECTED_PLAYER) {
            AddGossipItemFor(player, 11, "< Main Menu >", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 4, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3SOLO_GAMES);
            sArenaSpectatorNPC->ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3SOLO_GAMES, 4);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action == 200) {
            AddGossipItemFor(player, 11, "< Main Menu >", GOSSIP_SENDER_MAIN, 100);
            SendGossipMenuFor(player, 190017, creature->GetGUID());
        } else {
            ObjectGuid guid = ObjectGuid(HighGuid::Player, action - NPC_SPECTATOR_ACTION_SELECTED_PLAYER);
            if (Player * target = ObjectAccessor::FindPlayer(guid)) {
                ChatHandler handler(player->GetSession());
                char const *pTarget = target->GetName().c_str();
                ArenaSpectator::HandleSpectatorSpectateCommand(&handler, pTarget);
            }
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override {
        ClearGossipMenuFor(player);

        if (sender != GOSSIP_SENDER_MAIN)
        {
            return false;
        }

        if (!code)
        {
            code = "";
        }

        if (action == NPC_SPECTATOR_ACTION_SPECIFIC) {
            const char *plrName = code;

            char playerName[50];
            strcpy(playerName, plrName);

            for (int i = 0; i < 13; i++) {
                if (!playerName[i])
                    break;
                if (i == 0 && playerName[i] > 96)
                    playerName[0] -= 32;
                else if (playerName[i] < 97)
                    playerName[i] += 32;
            }

            if (Player * target = ObjectAccessor::FindPlayerByName(playerName)) {
                ChatHandler handler(player->GetSession());
                char const *pTarget = target->GetName().c_str();
                CloseGossipMenuFor(player);
                ArenaSpectator::HandleSpectatorSpectateCommand(&handler, pTarget);
                return true;
            }
            ChatHandler(player->GetSession()).PSendSysMessage("Player is not online or does not exist.");
            OnGossipHello(player, creature);
            return false;
        }
        CloseGossipMenuFor(player);
        return true;
    }
};

void AddSC_ArenaSpectatorNPC()
{
    new ArenaSpectatorNPC_Creature();
    new ArenaSpectatorNPC_BG();
}
