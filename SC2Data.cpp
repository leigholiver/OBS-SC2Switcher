#include <QObject>
#include <QCoreApplication>
#include <QTimer>
#include <obs-frontend-api.h>
#include <curl/curl.h>

#include "SC2Data.h"
#include "SC2State.h"
#include "Observer.h"
#include "Config.h"
#include "log.h"

QT_USE_NAMESPACE

SC2Data* SC2Data::Instance = nullptr;

SC2Data::SC2Data(QObject* parent)
    : QObject(parent)
{
	stopping = false;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1500);
	state = nullptr;
}

SC2Data::~SC2Data() {
	timer->stop();
	stopping = true;
	//delete Instance;
}

void SC2Data::stop() {
	stopping = true;
	timer->stop();
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
		std::string reqURL;
		std::string UIResponse;
		std::string gameResponse;

		reqURL = config->ipAddr;
		if(reqURL == "") {
			reqURL = "localhost";
		}
		reqURL = "http://" + reqURL + ":6119/";


		int still_running = 0;
		CURLM *multi_handle;
		multi_handle = curl_multi_init();

		CURL *uihandle;
		uihandle = curl_easy_init();
		if (!uihandle) {
			return;
		}
		curl_multi_add_handle(multi_handle, uihandle);
		curl_easy_setopt(uihandle, CURLOPT_URL, (reqURL + "ui").c_str());
		curl_easy_setopt(uihandle, CURLOPT_TIMEOUT_MS, 500);
		curl_easy_setopt(uihandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(uihandle, CURLOPT_WRITEDATA, &UIResponse);

		CURL *gamehandle;
		gamehandle = curl_easy_init();
		if (!gamehandle) {
			return;
		}
		curl_multi_add_handle(multi_handle, gamehandle);
		curl_easy_setopt(gamehandle, CURLOPT_URL, (reqURL + "game").c_str());
		curl_easy_setopt(gamehandle, CURLOPT_TIMEOUT_MS, 500);
		curl_easy_setopt(gamehandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(gamehandle, CURLOPT_WRITEDATA, &gameResponse);


		curl_multi_perform(multi_handle, &still_running);
		while (still_running) {
			CURLMcode mc; /* curl_multi_wait() return code */
			int numfds;
			mc = curl_multi_wait(multi_handle, NULL, 0, 100, &numfds);
			if (mc != CURLM_OK) {
				s2log("/curl Failed");
				s2log(curl_multi_strerror(mc));

				curl_multi_remove_handle(multi_handle, uihandle);
				curl_easy_cleanup(uihandle);
				curl_multi_remove_handle(multi_handle, gamehandle);
				curl_easy_cleanup(gamehandle);
				curl_multi_cleanup(multi_handle);

				return;
			}
			if (stopping) {
				curl_multi_remove_handle(multi_handle, uihandle);
				curl_easy_cleanup(uihandle);
				curl_multi_remove_handle(multi_handle, gamehandle);
				curl_easy_cleanup(gamehandle);
				curl_multi_cleanup(multi_handle);
				return;
			}
			QCoreApplication::processEvents();
			curl_multi_perform(multi_handle, &still_running);
		}		

		if (stopping) {
			return;
		}

		if (UIResponse == "") {
			s2log("no ui response");
			return;
		}
		if (gameResponse == "") {
			s2log("no game response");
			return;
		}

		s2log("requests finished");
		s2log(UIResponse);
		s2log(gameResponse);
		
		// create an sc2state object
		SC2State* newState = new SC2State();
		newState->fromJSONString(UIResponse, gameResponse);
		if(state == nullptr) {
			state = newState;
		}

		if(state->appState != newState->appState || 
			state->gameState != newState->gameState || 
			state->menuState != newState->menuState ) {
			s2log("state has changed, notifying");
			for (size_t i = 0; i < watchers.size(); i++) {
				s2log("notifying " + watchers[i]->getName());
	    		watchers[i]->notify(state, newState);
			}
			state = newState;
		}
	}
}