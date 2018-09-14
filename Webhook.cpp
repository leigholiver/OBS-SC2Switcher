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

		if(previous->appState != current->appState &&
			current->appState != APP_INGAME && previous->appState == APP_INGAME) {	
			if(current->fullState.players.size() == 2 && !current->fullState.isReplay) {
				event = "exit";
			}
		}

		if(previous->appState != current->appState &&
			current->appState == APP_INGAME && previous->appState != APP_INGAME) {
			event = "enter";
		}

 		if(event != "") {
 			sendRequest(current, event);
 		}
 	}
 }

 void Webhook::sendRequest(SC2State*& game, std::string event) {
 	if(!game->fullState.isReplay) {
	 	CURL *curl;
		CURLcode res;
		
		Config* cfg = Config::Current();
 		
 		// todo: make this a multi curl 
 		for (string &url : cfg->webhookURLList) {
		 	curl = curl_easy_init();
			if (curl) {
			 	std::string qdelim = "?";
		 	  	std::size_t found = url.find(qdelim);
				if (found!=std::string::npos) {
					qdelim = "&";
				}

				std::string resp = getJSONStringFromSC2State(game, event);
			 	std::string c_url = url + qdelim + "json=" + curl_easy_escape(curl, resp.c_str(), 0);
				curl_easy_setopt(curl, CURLOPT_URL, c_url.c_str());
				curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 150);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				if (res != CURLE_OK) {
					// pass
				}
			}
		}
	}
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