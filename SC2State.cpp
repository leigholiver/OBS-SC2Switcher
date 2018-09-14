#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <jansson.h>
#include <typeinfo>
#include "Constants.h"
#include "Config.h"
#include "SC2State.h"
#include "APIState.h"

#include "obs-util.h"

const char* jsonToString(json_t * result, const char* name) {
	json_t * jsonName = json_object_get(result, name);
	if (NULL != jsonName && json_is_string(jsonName)) {
		return json_string_value(jsonName);
	}
	return nullptr;
}


/**
AppState (GAME/MENU/LOADING)
GameState (INGAME/OBS/REPLAY)
MenuState (SCORESCREEN/PROFILE/HOME/CAMPAIGN/COOP/REPLAYS/VERSUS/CUSTOM/LOBBY/COLLECTION)
**/
SC2State::SC2State(QObject* parent)
    : QObject(parent)
{

}

SC2State::~SC2State() {
}

void SC2State::fromJSONString(std::string uiResponse, std::string gameResponse) {
	
	APIState api;

	json_error_t error;
	json_t* root = json_loads(uiResponse.c_str(), 0, &error);
	if (!root) {
		return;
	}
	json_t* screens = json_object_get(root, "activeScreens");
	for (size_t i = 0; i < json_array_size(screens); i++) {
		json_t* obj_txt = json_array_get(screens, i);;
		const char* strText;
		if (NULL != obj_txt && json_is_string(obj_txt)) {
			strText = json_string_value(obj_txt);
			api.activeScreens.push_back(strText);
		}
	}

	root = json_loads(gameResponse.c_str(), 0, &error);
	if (!root) {
		return;
	}

	json_t* isReplay = json_object_get(root, "isReplay");
	api.isReplay = (isReplay == json_true());

	json_t* displayTime = json_object_get(root, "displayTime");
	if(json_is_number(displayTime)) {
		double dp = json_real_value(displayTime);
		api.displayTime = dp;
	}
	
	
	json_t* players = json_object_get(root, "players");
	for (size_t i = 0; i < json_array_size(players); i++) {
		player* p = new player;
		json_t* playerJSON = json_array_get(players, i);
		p->name = (jsonToString(playerJSON, "name") == nullptr? "" : jsonToString(playerJSON, "name"));
		p->type = (jsonToString(playerJSON, "type") == nullptr? "" : jsonToString(playerJSON, "type"));
		p->race = (jsonToString(playerJSON, "race") == nullptr? "" : jsonToString(playerJSON, "race"));
		p->result = (jsonToString(playerJSON, "result") == nullptr? "" : jsonToString(playerJSON, "result"));
		api.players.push_back(p);
	}

	fullState = api;
	fillState();
}

void SC2State::fillState() {
	Config* cfg = Config::Current();
	if (fullState.activeScreens.size() > 1) {
		bool found = false;
		int sceneIdx = 0;
		int max = sizeof(menuLabels) / sizeof(menuLabels[0]);

		while (!found && sceneIdx < max) {
			for (size_t i = 0; i < fullState.activeScreens.size(); i++) {
				std::string active = fullState.activeScreens[i];
				std::string label = menuLabels[sceneIdx];
				if (active == label) {
					menuState = sceneIdx;
					found = true;
					break;
				}
			}
			sceneIdx++;
		}
		if(!found) {
			menuState = MENU_NONE;
		}
		appState = APP_MENU;
	}
	else if (fullState.activeScreens.size() == 0) {
		// in game
		appState = APP_INGAME;
		gameState = GAME_INGAME;
	}
	else {
		// we are in the loading screen
		appState = APP_LOADING;
	}

	if(fullState.isReplay) {
		gameState = GAME_REPLAY;
	}

	bool usernameFound = false;
	for (size_t i = 0; i < fullState.players.size(); i++) {
		if (cfg->usernames.size() > 0) {
			// if this is one of our usernames
			if (std::find(cfg->usernames.begin(), cfg->usernames.end(), fullState.players[i]->name) != cfg->usernames.end()) {
				usernameFound = true;
			}
		}
		// if this username isnt already in our recents, add it
		if (std::find(cfg->recentUsernames.begin(), cfg->recentUsernames.end(), fullState.players[i]->name) == cfg->recentUsernames.end()) {
			cfg->recentUsernames.push_back(fullState.players[i]->name);
			
			// limit the size of the recent usernames list, it could get quite big
			if (cfg->recentUsernames.size() > 8) {
				cfg->recentUsernames.erase(cfg->recentUsernames.begin());
			}
		}
	}

	if (!usernameFound && cfg->usernames.size() > 0) {
		gameState = GAME_OBS;
	}
}


