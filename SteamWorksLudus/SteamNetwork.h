#pragma once
#include <iostream>
#include <string>
#include "../HeaderFiles/steam_api.h"

class SteamNetwork
{
public:
	SteamNetwork();
	~SteamNetwork();

	int init();
	void open_overlay(const char * pchDialog);
	void display_user_information();
	void message_received_callback_auto_reply(GameConnectedFriendChatMsg_t* pCallBack, char* received_msg, const char* msg);

	//STATUS ACCESSORS
	int get_steam_status();

	//FRIENDS
	const char* get_friend_name(CSteamID friendID, bool from_lobby);
	CSteamID get_friend_steamid_by_index(int friendIndex);
	uint64 get_friend_steamid64_by_index(int friendIndex);

	const int get_friends_count();
	CSteamID* get_friends();
	void display_friends();

	//STEAMID ACCESSORS
	void set_csteamid();
	CSteamID get_csteamid();
	uint64 get_steam_id64(); // used to translate CSteamID to uint64 (SteamID64)

	//STEAMNAME ACCESSORS
	void set_steam_username();
	const char* get_steam_username();

	//LOBBY ACCESSORS
	void set_steam_lobby(CSteamID lobbyID);
	CSteamID get_current_lobby_csteamid();

	//LOBBY
	void create_lobby(ELobbyType lobby_type, int max_members);
	void search_lobbies();
	void join_lobby(uint64 lobby_id64);
	void leave_lobby();

	void display_lobby_information(CSteamID lobbyID);

	CSteamID* get_lobby_members(CSteamID lobbyID);
	const int get_members_count_in_lobby(CSteamID lobbyID);
	CSteamID get_member_steamid_from_lobby(CSteamID lobbyID, CSteamID member_id);

private:
	STEAM_CALLBACK(SteamNetwork, _CB_on_overlay_activated, GameOverlayActivated_t); //Will never work because no graphic cannot open SteamOverlay
	STEAM_CALLBACK(SteamNetwork, _CB_on_message_received, GameConnectedFriendChatMsg_t); //Will trigger when a friend send a message
	STEAM_CALLBACK(SteamNetwork, _CB_on_lobby_created, LobbyCreated_t); //Trigger when a lobby has been created
	STEAM_CALLBACK(SteamNetwork, _CB_on_lobby_entered, LobbyEnter_t); //Trigger when entering a lobby
	STEAM_CALLBACK(SteamNetwork, _CB_on_lobby_data_updated, LobbyDataUpdate_t); //Trigger when lobby datas has been updated (add remove changes)
	STEAM_CALLBACK(SteamNetwork, _CB_on_lobby_chat_updated, LobbyChatUpdate_t); //Trigger when a lobby chat room state has changed (user joined/left lobby)
	STEAM_CALLBACK(SteamNetwork, _CB_on_lobby_match_list, LobbyMatchList_t); //Trigger when requesting lobby list

	//STEAM STATUS
	int steam_status = 0;

	//STEAM USER
	CSteamID CUSER_STEAMID;
	uint64 USER_STEAMID64;
	const char* USER_STEAMNAME = "";

	//LOBBIES
	CSteamID current_lobby;
};