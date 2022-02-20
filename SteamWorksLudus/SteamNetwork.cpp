using namespace std;
#include "SteamNetwork.h"

//CONSTRUCTOR
SteamNetwork::SteamNetwork()
{
	this->steam_status = SteamNetwork::init();

	if (this->steam_status != 1) {
		//Set user steamid with SteamUser api
		set_csteamid();

		//Set user name with SteamFriends api
		set_steam_username();
	}
}

//DESTRUCTOR
SteamNetwork::~SteamNetwork()
{
}

//LOGIC
int SteamNetwork::init()
{
	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) { // Check if the application id is valid, otherwise exit the program and return 1
		cout << "Error, AppId is invalid, exiting with error code 1..." << endl;

		return 1;
	}
	if (!SteamAPI_Init()) {
		cout << "Steam failed to Initialize, exiting..." << endl;
		return 1;
	}
	if (!SteamUser()->BLoggedOn()) {
		cout << "User could not log in to Steam, exiting..." << endl;

		return 1;
	}

	//Debug message to print success
	cout << "Steam initialized successfully" << endl;

	//Enable listening for friends message ; ! NEED TO BE DONE AFTER INITIALIZATION !
	if (SteamFriends() != nullptr)
		SteamFriends()->SetListenForFriendsMessages(true);
	else
		cout << "Could not listen for friends message, SteamFriends is nullptr" << endl;
	

	//Steam user steam id
	return 0;
}

// This method will never works because the projet is on a Console Application
void SteamNetwork::open_overlay(const char * pchDialog)
{
	if (this->steam_status == 0) {
		if (SteamFriends() == nullptr) {
			cout << "Error, SteamFriends not initialized in SteamNetwork::open_overlay" << endl;
			return;
		}
		SteamFriends()->ActivateGameOverlay(pchDialog);
	}
}

void SteamNetwork::display_user_information()
{
	if (this->steam_status == 0) {
		uint64 steamid64 = get_steam_id64();
		const char* steamname = get_steam_username();

		if (steamid64 <= 0 or steamname == "") { // steam id 64 should never be 0 or under and steamname should never be nothing
			cout << "Error, could not get steamid or username in SteamNetwork::display_user_information" << endl;
			return;
		}

		//Debug message to print user's steamid64 ; can be check on https://steamid.io/lookup (enter your steam nickname)
		cout << steamid64 << endl;

		//Debug message to print user's nickname ;
		cout << steamname << endl;
	}
}

//STEAMID ACCESSORS
void SteamNetwork::set_csteamid()
{
	if (SteamUser() == nullptr) {
		cout << "Error, SteamUser not initialized in SteamNetwork::set_csteamid" << endl;
		return;
	}
	this->CUSER_STEAMID = SteamUser()->GetSteamID();
}

CSteamID SteamNetwork::get_csteamid()
{
	return this->CUSER_STEAMID;
}

uint64 SteamNetwork::get_steam_id64()
{
	return this->CUSER_STEAMID.ConvertToUint64();
}

//STEAMNAME ACCESSORS
void SteamNetwork::set_steam_username()
{
	if (SteamFriends() == nullptr) {
		cout << "Error, SteamFriends not initialized in SteamNetwork::set_steam_username" << endl;
		return;
	}
	this->USER_STEAMNAME = SteamFriends()->GetPersonaName();
}
const char* SteamNetwork::get_steam_username()
{
	return this->USER_STEAMNAME;
}


//Auto reply message msg to sender
void SteamNetwork::message_received_callback_auto_reply(GameConnectedFriendChatMsg_t* pMessageCallBack, char* received_msg, const char* send_msg) {

	SteamFriends()->ReplyToFriendMessage(pMessageCallBack->m_steamIDUser, send_msg);
	cout << endl;
	cout << "MESSAGE RECEIVED" << endl << endl;
	cout << "FROM (STEAMID) : " << pMessageCallBack->m_steamIDUser.ConvertToUint64() << endl;
	cout << "FROM (NAME) : " << SteamFriends()->GetFriendPersonaName(pMessageCallBack->m_steamIDUser) << endl;
	cout << "MESSAGE ID : " << pMessageCallBack->m_iMessageID << endl;
	cout << "MESSAGE CONTENT : " << received_msg << endl;
	cout << "AUTO REPLIED TO MESSAGE " << pMessageCallBack->m_iMessageID << ": " << send_msg << endl;
	cout << endl;
}

const int SteamNetwork::get_friends_count() {
	if (SteamFriends() != nullptr) {
		const int friendsCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

		if (friendsCount <= 0) {
			return 0;
		}

		return friendsCount;
	}
	return 0;
}

int SteamNetwork::get_steam_status()
{
	return this->steam_status;
}

CSteamID* SteamNetwork::get_friends()
{
	if (SteamFriends() != nullptr) {
		const int friendsCount = get_friends_count();

		if (friendsCount > 0) {
			CSteamID* friendsID = new CSteamID[friendsCount];

			for (int frd_index = 0; frd_index < friendsCount; frd_index++){
				friendsID[frd_index] = get_friend_steamid_by_index(frd_index);
			}

			return friendsID;
		}
	}

	return nullptr;
}

void SteamNetwork::display_friends()
{
	if (SteamFriends() != nullptr) {
		const int friendsCount = get_friends_count();
		CSteamID* friends = get_friends();

		for (int i = 0; i < friendsCount; i++) {
			cout << "Friend " << i << " with SteamID64 " << get_friend_steamid64_by_index(i) << " - " << get_friend_name(friends[i], false) << endl;
		}
	}
}

const char* SteamNetwork::get_friend_name(CSteamID friendID, bool from_lobby)
{
	if (SteamFriends() != nullptr) {
		if (friendID.ConvertToUint64() != 0) {
			const char* friendName = SteamFriends()->GetFriendPersonaName(friendID);
			if (friendName != "") {
				return friendName;
			}
		}
		else {
			if (from_lobby) {
				const char* name = "LOBBY";
				return name;
			}
		}
	}
	return nullptr;
}

CSteamID SteamNetwork::get_friend_steamid_by_index(int friendIndex) {
	CSteamID tmpId;
	if (SteamFriends() != nullptr)
		return SteamFriends()->GetFriendByIndex(friendIndex, k_EFriendFlagImmediate);
}

uint64 SteamNetwork::get_friend_steamid64_by_index(int friendIndex)
{
	if (friendIndex >= 0) {
		return get_friend_steamid_by_index(friendIndex).ConvertToUint64();
	}
	return 0;
}

//LOBBY
void SteamNetwork::set_steam_lobby(CSteamID lobbyID) {
	this->current_lobby = lobbyID;
}
CSteamID SteamNetwork::get_current_lobby_csteamid() {
	return this->current_lobby;
}

const int SteamNetwork::get_members_count_in_lobby(CSteamID lobbyID) {
	return SteamMatchmaking()->GetNumLobbyMembers(lobbyID);
}

CSteamID* SteamNetwork::get_lobby_members(CSteamID lobbyID) {
	if (lobbyID.ConvertToUint64() != 0) {
		const int membersCount = get_members_count_in_lobby(lobbyID);
		if (membersCount > 0) {
			CSteamID* members = new CSteamID[membersCount];

			for (int i = 0; i < membersCount; i++) {
				CSteamID member_id = SteamMatchmaking()->GetLobbyMemberByIndex(lobbyID, i);
				members[i] = member_id;
			}

			return members;
		}
	}

	return nullptr;
}

CSteamID SteamNetwork::get_member_steamid_from_lobby(CSteamID lobbyID, CSteamID member_id) {
	CSteamID* members = get_lobby_members(lobbyID);

	for (int member_index = 0; member_index < get_members_count_in_lobby(lobbyID); member_index++) {
		if (members[member_index].ConvertToUint64() == member_id.ConvertToUint64()) {
			cout << "FOUND MEMBER ! " << endl;
			return members[member_index];
		}
		else {
			return CSteamID();
		}
	}
}

void SteamNetwork::create_lobby(ELobbyType lobby_type, int max_members) {
	if (SteamMatchmaking() != nullptr) {
		if ( (lobby_type == k_ELobbyTypePrivate || lobby_type == k_ELobbyTypeFriendsOnly || lobby_type == k_ELobbyTypePublic || lobby_type == k_ELobbyTypeInvisible) && max_members < 251) {
			SteamMatchmaking()->CreateLobby(lobby_type, max_members);
		}
		else {
			cout << "Error, could not create lobby" << endl;
		}
	}
}
void SteamNetwork::search_lobbies() {
	if (SteamMatchmaking() != nullptr) {
		SteamMatchmaking()->AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterClose);
		SteamMatchmaking()->RequestLobbyList();
	}

}

void SteamNetwork::join_lobby(uint64 lobby_id64) {
	CSteamID lobbyID = CSteamID(lobby_id64);

	if (lobby_id64 <= 0) {
		cout << endl << "Error, could not find lobby " << lobby_id64 << " in SteamNetwork::join_lobby" << endl;
		return;
	}

	SteamMatchmaking()->JoinLobby(lobbyID);
}
void SteamNetwork::leave_lobby() {
	if (SteamMatchmaking() != nullptr) {
		CSteamID lobbyID = get_current_lobby_csteamid();
		if (get_current_lobby_csteamid().ConvertToUint64() <= 0) {
			//cout << endl << "Error, could not find lobby " << lobbyID.ConvertToUint64() << " in SteamNetwork::leave_lobby" << endl; //DEBUG Message
			return;
		}

		SteamMatchmaking()->LeaveLobby(lobbyID);
	}
}

void SteamNetwork::display_lobby_information(CSteamID lobbyID) {
	cout << "LOBBY ID : " << lobbyID.ConvertToUint64() << endl;
	cout << "DATA : " << SteamMatchmaking()->GetLobbyData(lobbyID, "name") << endl;
}

//CALLBACKS

// Will never work because no graphic cannot open SteamOverlay
void SteamNetwork::_CB_on_overlay_activated(GameOverlayActivated_t* pCallBack)
{
	if (pCallBack->m_bActive) {
		cout << "Steam overlay is active !" << endl;
	}
	else {
		cout << "Steam overlay is inactive !" << endl;
	}
}

//Will intercept the message and reply if valid
void SteamNetwork::_CB_on_message_received(GameConnectedFriendChatMsg_t* pCallBack)
{
	EChatEntryType entry = k_EChatEntryTypeInvalid;
	char* tmp = new char[4096];
	SteamFriends()->GetFriendMessage(pCallBack->m_steamIDUser, pCallBack->m_iMessageID, tmp, 4096, &entry);

	if (entry != k_EChatEntryTypeChatMsg && entry != k_EChatEntryTypeEmote) {
		return;
	}
	else {
		message_received_callback_auto_reply(pCallBack, tmp, "Hello!");
	}

	delete[] tmp;
}

void SteamNetwork::_CB_on_lobby_created(LobbyCreated_t* pCallBack) {
	EResult eresult = pCallBack->m_eResult;
	uint64 lobbyID = pCallBack->m_ulSteamIDLobby;

	CSteamID cLobbyID = CSteamID(pCallBack->m_ulSteamIDLobby);

	if (eresult == k_EResultOK)
		set_steam_lobby(cLobbyID);
	else {
		cout << "ERROR : eresult != k_EResultOK [in SteamNetwork::_CB_on_lobby_created" << endl;
		return;
	}

	cout << endl << "LOBBY CREATED" << endl;

	cout << "Lobby response : " << eresult << endl;
	cout << "Lobby ID : " << lobbyID << endl;

	SteamMatchmaking()->SetLobbyData(cLobbyID, "lobbyname", "Weact's Lobby");
	cout << SteamMatchmaking()->GetLobbyData(cLobbyID, "lobbyname") << endl;
	get_lobby_members(cLobbyID);
}

void SteamNetwork::_CB_on_lobby_entered(LobbyEnter_t* pCallBack) {
	uint64 lobbyID = pCallBack->m_ulSteamIDLobby;
	bool lobby_locked = pCallBack->m_bLocked;
	uint32 state = pCallBack->m_EChatRoomEnterResponse;

	CSteamID clobbyID = CSteamID(pCallBack->m_ulSteamIDLobby);

	cout << endl << "LOBBY ENTERED" << endl;

	cout << "Lobby ID : " << lobbyID << endl;
	cout << "Lobby locked state : " << lobby_locked << endl;
	cout << "Lobby response : " << state << endl;

	get_lobby_members(clobbyID);
}

void SteamNetwork::_CB_on_lobby_data_updated(LobbyDataUpdate_t* pCallBack) {
	uint64 lobbyID = pCallBack->m_ulSteamIDLobby;
	uint64 changer_steamid = pCallBack->m_ulSteamIDMember;
	uint8 state = pCallBack->m_bSuccess;

	CSteamID changer_csteamid = get_member_steamid_from_lobby(get_current_lobby_csteamid(), CSteamID(changer_steamid));
	const char* changer_username = get_friend_name(changer_csteamid, true);

	

	cout << endl << "LOBBY DATA UPDATED" << endl;

	cout << "Lobby ID : " << lobbyID << endl;
	cout << changer_steamid << " " << changer_username << " made changes to lobby" << endl;
	cout << "Changes / Lobby State : " << bool(state) << endl;
}

void SteamNetwork::_CB_on_lobby_chat_updated(LobbyChatUpdate_t* pCallBack) {
	uint64 lobbyID = pCallBack->m_ulSteamIDLobby;
	uint64 changer_steamid = pCallBack->m_ulSteamIDUserChanged; // Will be the one kicked (or other)
	uint64 makingchange_steamid = pCallBack->m_ulSteamIDMakingChange; // Can be different, will be the one who kick someone for example (or other)
	uint64 state = pCallBack->m_rgfChatMemberStateChange; //k_EChatMemberStateChange : Entered, Left, Disconnected, Kicked, Banned

	const char* changer_username = SteamFriends()->GetFriendPersonaName(CSteamID(changer_steamid));
	const char* makingchange_username = SteamFriends()->GetFriendPersonaName(CSteamID(makingchange_steamid));

	cout << endl << "LOBBY CHAT UPDATE" << endl;

	cout << "Lobby ID : " << lobbyID << endl;
	cout << changer_steamid << " " << changer_username << " has entered/left/disconnected/kicked/banned" << endl;
	cout << makingchange_steamid << " " << makingchange_username << " is the one responsible of the action" << endl;
	cout << "STATE : " << state << endl;
}

void SteamNetwork::_CB_on_lobby_match_list(LobbyMatchList_t* pCallBack) {
	uint32 n_lobbies = pCallBack->m_nLobbiesMatching; //number of lobbies matching

	cout << endl << endl;

	if (n_lobbies > 0) {
		for (int lby_index = 0; lby_index < n_lobbies; lby_index++) {
			CSteamID cLobbyID = SteamMatchmaking()->GetLobbyByIndex(lby_index);
			display_lobby_information(cLobbyID);
		}
	}
}