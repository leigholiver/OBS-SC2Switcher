#include <curl/curl.h>

#include "SC2Data.h"
#include "Observer.h"
#include "SC2State.h"
#include "Webhook.h"
#include "Constants.h"
#include "Config.h"
#include "obs-util.h"
#include "APIState.h"
#include "ScoreTracker.h"

Webhook::Webhook(SC2Data *sc2): Observer(sc2){ }

void Webhook::notify(SC2State*& previous, SC2State*& current) {
 	Config* cfg = Config::Current();
 	if(cfg->webhookEnabled) {
 		std::string event = "";
 		// quit and rewind
 		if(previous->gameState == GAME_INGAME && 
 			previous->appState == APP_INGAME && 
 			current->gameState == GAME_REPLAY) {
 			if(current->fullState.players.size() == 2) {
 				event = "exit";
 			}
 		}

 		// just quit 
		if(current->appState != APP_INGAME && 
			previous->appState == APP_INGAME) {
			if(current->fullState.players.size() == 2 && !current->fullState.isReplay) {
				event = "exit";
			}
		}

		if(current->appState == APP_INGAME && 
			previous->appState != APP_INGAME &&
			current->gameState != GAME_REPLAY) {
			event = "enter";
		}

		if (event != "") {
			sendRequest(current, event);
 		}
 	}
 }

void Webhook::sendRequest(SC2State*& game, std::string event) {
	Config* cfg = Config::Current();

	int still_running = 0;
	CURLM *multi_handle;
	multi_handle = curl_multi_init();
		
	// vector of handles 
	vector<CURL*> handles;

		for (string &url : cfg->webhookURLList) {
		CURL *handle;
		handle = curl_easy_init();
		if (handle) {
			curl_multi_add_handle(multi_handle, handle);
			handles.push_back(handle);
		 	std::string qdelim = "?";
	 	  	std::size_t found = url.find(qdelim);
			if (found!=std::string::npos) {
				qdelim = "&";
			}

			std::string resp = getJSONStringFromSC2State(game, event);
		 	std::string c_url = url + qdelim + "json=" + curl_easy_escape(handle, resp.c_str(), 0);
			curl_easy_setopt(handle, CURLOPT_URL, c_url.c_str());
			curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, 500);
		}
	}

	curl_multi_perform(multi_handle, &still_running);
	while(still_running) {
		CURLMcode mc; /* curl_multi_wait() return code */ 
		int numfds;
		mc = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);

		if(mc != CURLM_OK) {
			break;
		}
		curl_multi_perform(multi_handle, &still_running);
	}
	// clean up each handle
	for (CURL* &handle : handles) {
		curl_multi_remove_handle(multi_handle, handle);
			curl_easy_cleanup(handle);
	}
	curl_multi_cleanup(multi_handle);
 }

 std::string Webhook::getQueryStringFromSC2State(SC2State*& game, std::string event, CURL* curl) {
	Config* cfg = Config::Current();
	std::string resp = "";
 	for(size_t i=0; i<game->fullState.players.size(); i++) {
		std::string index = std::to_string(i);
		resp = resp + "&players[" + index + "][name]=" + curl_easy_escape(curl, game->fullState.players[i]->name, 0);
		resp = resp + "&players[" + index + "][type]=" + curl_easy_escape(curl, game->fullState.players[i]->type, 0);
		resp = resp + "&players[" + index + "][race]=" + curl_easy_escape(curl, game->fullState.players[i]->race, 0);
		resp = resp + "&players[" + index + "][result]=" + curl_easy_escape(curl, game->fullState.players[i]->result, 0);
 		if (std::find(cfg->usernames.begin(), cfg->usernames.end(), game->fullState.players[i]->name) != cfg->usernames.end()) {
			resp = resp + "&players[" + index + "][isme]=true";
		}
		else {
			resp = resp + "&players[" + index + "][isme]=false";
		}
	}

	std::string dp = std::to_string(game->fullState.displayTime);
	resp = resp + "&displayTime=" + dp;
	resp = resp + "&event=" + event;
 	return resp;
 }

std::string Webhook::getJSONStringFromSC2State(SC2State*& game, std::string event) {
	// i know this is dirty but im just testing it as the query string option 
	// doesnt work with node. ill update it... 
	Config* cfg = Config::Current();
	std::string resp = "";
	resp = resp + "{\"players\": [";
 	for(size_t i=0; i<game->fullState.players.size(); i++) {
		resp = resp + "{";
		resp = resp + "\"name\": \"" + game->fullState.players[i]->name + "\",";
		resp = resp + "\"type\": \"" + game->fullState.players[i]->type + "\",";
		resp = resp + "\"race\": \"" + game->fullState.players[i]->race + "\",";
		resp = resp + "\"result\": \"" + game->fullState.players[i]->result + "\",";

 		if (std::find(cfg->usernames.begin(), cfg->usernames.end(), game->fullState.players[i]->name) != cfg->usernames.end()) {
			resp = resp + "\"isme\": true";
		}
		else {
			resp = resp + "\"isme\": false";
		}
		resp = resp + "}";
		if(i + 1 != game->fullState.players.size()) {
			resp = resp + ",";
		}
	}
	resp = resp + "],";
	std::string dp = std::to_string(game->fullState.displayTime);
	resp = resp + "\"displayTime\": \"" + dp + "\",";
	resp = resp + "\"event\": \"" + event + "\",";

	ScoreTracker* st = ScoreTracker::Current();
	resp = resp + "\"scores\": { ";
	resp = resp + "\"Terr\": {\"Victory\": " + to_string(st->scores["Terr"]["Victory"]) + ", \"Defeat\": " + to_string(st->scores["Terr"]["Defeat"]) + " },";
	resp = resp + "\"Prot\": {\"Victory\": " + to_string(st->scores["Prot"]["Victory"]) + ", \"Defeat\": " + to_string(st->scores["Prot"]["Defeat"]) + " },";
	resp = resp + "\"Zerg\": {\"Victory\": " + to_string(st->scores["Zerg"]["Victory"]) + ", \"Defeat\": " + to_string(st->scores["Zerg"]["Defeat"]) + " }";
	resp = resp + "}";

	resp = resp + "}";
 	return resp;
 }