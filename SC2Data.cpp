#include <QObject>
#include <QTimer>
#include <obs-frontend-api.h>
#include <curl/curl.h>

#include "SC2Data.h"
#include "SC2State.h"
#include "Observer.h"
#include "Config.h"

QT_USE_NAMESPACE

SC2Data* SC2Data::Instance = nullptr;

SC2Data::SC2Data(QObject* parent)
    : QObject(parent)
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
	state = nullptr;
}

SC2Data::~SC2Data() {
	delete Instance;
}

void SC2Data::attach(Observer *obs) {
    watchers.push_back(obs);
}
	
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void SC2Data::update() {
	Config* config = Config::Current();
	if(config->isRunning) {
		CURL *curl;
		CURLcode res;
		std::string reqURL;
		std::string UIResponse;
		std::string gameResponse;

		reqURL = config->ipAddr;
		if(reqURL == "") {
			reqURL = "localhost";
		}
		reqURL = "http://" + reqURL + ":6119/";

		curl = curl_easy_init();
		if (curl) {
			std::string url = reqURL + "ui";
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 50);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &UIResponse);
			res = curl_easy_perform(curl);
			//curl_easy_cleanup(curl);	
			if (res != CURLE_OK) {
				curl_easy_cleanup(curl);
				return;
			}
		}

		curl_easy_reset(curl);
		if (curl) {
			std::string url = reqURL + "game";
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 50);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gameResponse);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			if (res != CURLE_OK) {
				return;
			}
		}

		// create an sc2state object
		SC2State* newState = new SC2State();
		newState->fromJSONString(UIResponse, gameResponse);
		if(state == nullptr) {
			state = newState;
		}

		if(state->appState != newState->appState || 
			state->gameState != newState->gameState || 
			state->menuState != newState->menuState ) {
			for (size_t i = 0; i < watchers.size(); i++) {
	    		watchers[i]->notify(state, newState);
			}
			state = newState;
		}
	}
}