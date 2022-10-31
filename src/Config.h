
#pragma once
#include <obs.hpp>
#include <vector>
#include <string>
#include <algorithm>

#include "Constants.h"
#include "obs-util.h"
#include <obs-frontend-api.h>

class Config {
	public:
		Config();
		~Config();
		static Config* Current();
		static void free();

		// sc2data
		std::string ipAddr;
		std::vector<std::string> usernames;
		std::vector<std::string> recentUsernames;

		// scene switcher
		OBSWeakSource inGameScene;
		OBSWeakSource outGameScene;
		OBSWeakSource replayScene;
		OBSWeakSource obsScene;
		OBSWeakSource menuScenes[10];

		// score tracker
		std::string textSourceName;
		std::string scoreString;

		bool isRunning;

		std::string updateURL;
		std::string updateDescription;
		void checkForUpdates();
		bool logging = false;
		bool switchOnLoad = true;
		bool webhookEnabled = false;
		bool clearSettings = false;

		bool switcherEnabled;
		bool scoresEnabled;
		bool popupsEnabled;

		std::vector<std::string> webhookURLList;


	private:
		static Config* _instance;
};


static void LoadSaveHandler(obs_data_t *save_data, bool saving, void *) {
	if (saving) {
		Config* cfg = Config::Current();
		if(!cfg->clearSettings) {
			obs_data_t *obj = obs_data_create();

			// settings
			obs_data_set_bool(obj, "is_running", cfg->isRunning);
			obs_data_set_bool(obj, "switcher_enabled", cfg->switcherEnabled);
			obs_data_set_bool(obj, "scores_enabled", cfg->scoresEnabled);
			obs_data_set_bool(obj, "popups_enabled", cfg->popupsEnabled);
			obs_data_set_string(obj, "ip_addr", cfg->ipAddr.c_str());
			obs_data_set_bool(obj, "webhook_enabled", cfg->webhookEnabled);
			obs_data_set_bool(obj, "logging", cfg->logging);
			obs_data_set_bool(obj, "switch_on_load", cfg->switchOnLoad);

			obs_data_array_t *array = obs_data_array_create();
			for (std::string &s : cfg->webhookURLList) {
				obs_data_t *array_obj = obs_data_create();
				obs_data_set_string(array_obj, "URL", s.c_str());
				obs_data_array_push_back(array, array_obj);
				obs_data_release(array_obj);
			}
			obs_data_set_array(obj, "webhookURLs", array);
			obs_data_array_release(array);

			obs_data_array_t *array2 = obs_data_array_create();
			for (std::string &s : cfg->usernames) {
				obs_data_t *array_obj = obs_data_create();
				obs_data_set_string(array_obj, "username", s.c_str());
				obs_data_array_push_back(array2, array_obj);
				obs_data_release(array_obj);
			}
			obs_data_set_array(obj, "usernames", array2);
			obs_data_array_release(array2);

			obs_data_set_string(obj, "textSourceName", cfg->textSourceName.c_str());
			obs_data_set_string(obj, "scoreString", cfg->scoreString.c_str());

			// scenes
			obs_data_set_string(obj, "in_game_scene", GetWeakSourceName(cfg->inGameScene).c_str());
			obs_data_set_string(obj, "out_game_scene", GetWeakSourceName(cfg->outGameScene).c_str());
			obs_data_set_string(obj, "replay_scene", GetWeakSourceName(cfg->replayScene).c_str());
			obs_data_set_string(obj, "obs_scene", GetWeakSourceName(cfg->obsScene).c_str());
			obs_data_set_string(obj, "MENU_SCORESCREEN",  GetWeakSourceName(cfg->menuScenes[MENU_SCORESCREEN]).c_str());
			obs_data_set_string(obj, "MENU_PROFILE", GetWeakSourceName(cfg->menuScenes[MENU_PROFILE]).c_str());	
			obs_data_set_string(obj, "MENU_LOBBY", GetWeakSourceName(cfg->menuScenes[MENU_LOBBY]).c_str());
			obs_data_set_string(obj, "MENU_HOME", GetWeakSourceName(cfg->menuScenes[MENU_HOME]).c_str());
			obs_data_set_string(obj, "MENU_CAMPAIGN", GetWeakSourceName(cfg->menuScenes[MENU_CAMPAIGN]).c_str());
			obs_data_set_string(obj, "MENU_COLLECTION", GetWeakSourceName(cfg->menuScenes[MENU_COLLECTION]).c_str());
			obs_data_set_string(obj, "MENU_COOP", GetWeakSourceName(cfg->menuScenes[MENU_COOP]).c_str());
			obs_data_set_string(obj, "MENU_CUSTOM", GetWeakSourceName(cfg->menuScenes[MENU_CUSTOM]).c_str());
			obs_data_set_string(obj, "MENU_REPLAYS", GetWeakSourceName(cfg->menuScenes[MENU_REPLAYS]).c_str());
			obs_data_set_string(obj, "MENU_VERSUS", GetWeakSourceName(cfg->menuScenes[MENU_VERSUS]).c_str());

			obs_data_set_obj(save_data, "sc2switcher2", obj);
			obs_data_release(obj);
		}
	}
	else { 
		Config* cfg = Config::Current();

		obs_data_t *obj = obs_data_get_obj(save_data, "sc2switcher2");
		if (!obj) {
			obj = obs_data_create();
		}

		cfg->ipAddr = obs_data_get_string(obj, "ip_addr");
		
		std::vector<std::string> emptyusernames;
		cfg->usernames = emptyusernames;
		obs_data_array_t *array = obs_data_get_array(obj, "usernames");
		size_t count = obs_data_array_count(array);
		std::vector<std::string> seen;
		for (size_t i = 0; i < count; i++) {
			obs_data_t *array_obj = obs_data_array_item(array, i);
			std::string un = obs_data_get_string(array_obj, "username");
			if (std::find(seen.begin(), seen.end(), un) == seen.end() && un != "") {
				cfg->usernames.push_back(un);
			}
			obs_data_release(array_obj);
		}
		obs_data_array_release(array);

		std::vector<std::string> emptywebhookURLList;
		cfg->webhookURLList = emptywebhookURLList;
		obs_data_array_t *array2 = obs_data_get_array(obj, "webhookURLs");
		size_t count2 = obs_data_array_count(array2);
		std::vector<std::string> seen2;
		for (size_t i = 0; i < count2; i++) {
			obs_data_t *array_obj = obs_data_array_item(array2, i);
			std::string url = obs_data_get_string(array_obj, "URL");
			if (std::find(seen2.begin(), seen2.end(), url) == seen2.end() && url != "") {
				cfg->webhookURLList.push_back(url);
			}
			obs_data_release(array_obj);
		}
		obs_data_array_release(array2);
		
		cfg->textSourceName = obs_data_get_string(obj, "textSourceName");

		
		std::string scoreString = obs_data_get_string(obj, "scoreString");
		if (scoreString != "") {
			cfg->scoreString = scoreString;
		}

		cfg->isRunning = obs_data_get_bool(obj, "is_running");
		cfg->switcherEnabled = obs_data_get_bool(obj, "switcher_enabled");
		cfg->scoresEnabled = obs_data_get_bool(obj, "scores_enabled");
		cfg->popupsEnabled = obs_data_get_bool(obj, "popups_enabled");
		cfg->webhookEnabled = obs_data_get_bool(obj, "webhook_enabled");
		cfg->logging = obs_data_get_bool(obj, "logging");
		cfg->switchOnLoad = obs_data_get_bool(obj, "switch_on_load");


		obs_weak_source_release(cfg->inGameScene);
		obs_weak_source_release(cfg->outGameScene);
		obs_weak_source_release(cfg->replayScene);
		obs_weak_source_release(cfg->obsScene);
		obs_weak_source_release(cfg->menuScenes[MENU_SCORESCREEN]);
		obs_weak_source_release(cfg->menuScenes[MENU_PROFILE]);
		obs_weak_source_release(cfg->menuScenes[MENU_LOBBY]);
		obs_weak_source_release(cfg->menuScenes[MENU_HOME]);
		obs_weak_source_release(cfg->menuScenes[MENU_CAMPAIGN]);
		obs_weak_source_release(cfg->menuScenes[MENU_COLLECTION]);
		obs_weak_source_release(cfg->menuScenes[MENU_COOP]);
		obs_weak_source_release(cfg->menuScenes[MENU_CUSTOM]);
		obs_weak_source_release(cfg->menuScenes[MENU_REPLAYS]);
		obs_weak_source_release(cfg->menuScenes[MENU_VERSUS]);


		cfg->inGameScene = GetWeakSourceByName(obs_data_get_string(obj, "in_game_scene"));
		cfg->outGameScene = GetWeakSourceByName(obs_data_get_string(obj, "out_game_scene"));
		cfg->replayScene = GetWeakSourceByName(obs_data_get_string(obj, "replay_scene"));
		cfg->obsScene = GetWeakSourceByName(obs_data_get_string(obj, "obs_scene"));
		cfg->menuScenes[MENU_SCORESCREEN] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_SCORESCREEN"));
		cfg->menuScenes[MENU_PROFILE] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_PROFILE"));
		cfg->menuScenes[MENU_LOBBY] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_LOBBY"));
		cfg->menuScenes[MENU_HOME] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_HOME"));
		cfg->menuScenes[MENU_CAMPAIGN] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_CAMPAIGN"));
		cfg->menuScenes[MENU_COLLECTION] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_COLLECTION"));
		cfg->menuScenes[MENU_COOP] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_COOP"));
		cfg->menuScenes[MENU_CUSTOM] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_CUSTOM"));
		cfg->menuScenes[MENU_REPLAYS] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_REPLAYS"));
		cfg->menuScenes[MENU_VERSUS] = GetWeakSourceByName(obs_data_get_string(obj, "MENU_VERSUS"));
		

		obs_data_release(obj);
	}
}
