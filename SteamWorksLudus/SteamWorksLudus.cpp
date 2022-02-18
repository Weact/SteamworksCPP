using namespace std;

#include <iostream>
#include <conio.h>
#include "SteamNetwork.h"

SteamNetwork steam_network;

void _process();

int main()
{
    cout << endl;

    steam_network.display_user_information();
    //steam_network.open_overlay("steamid"); // ActivateGameOverlay will not work and callbacks will never be called due to project console application
    steam_network.display_friends();

    //steam_network.create_lobby(k_ELobbyTypePublic, 5);
    //steam_network.search_lobbies();
    //steam_network.leave_lobby();

    steam_network.join_lobby(109775241108934510);

    _process();
    return 0;
}

void _process() {
    while (1)
    {
        SteamAPI_RunCallbacks(); //Used to get all responses from Steam Callbacks, usually return a pCallBack
    };
}