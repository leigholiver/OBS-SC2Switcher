#include <curl/curl.h>
#include <jansson.h>
#include "Config.h"

Config* Config::_instance = new Config();

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

Config::Config() :
	ipAddr(std::string("localhost")),
	scoreString(std::string("vT: ${tw}-${tl}\nvZ: ${zw}-${zl}\nvP: ${pw}-${pl}")),
	isRunning(false),
	switcherEnabled(false),
	scoresEnabled(false),
	popupsEnabled(true) {}

Config* Config::Current() {
   	return _instance;
}

void Config::free(){
	if (_instance) {
		obs_weak_source_release(_instance->inGameScene);
		obs_weak_source_release(_instance->outGameScene);
		obs_weak_source_release(_instance->replayScene);
		obs_weak_source_release(_instance->obsScene);
		obs_weak_source_release(_instance->menuScenes[MENU_SCORESCREEN]);
		obs_weak_source_release(_instance->menuScenes[MENU_PROFILE]);
		obs_weak_source_release(_instance->menuScenes[MENU_LOBBY]);
		obs_weak_source_release(_instance->menuScenes[MENU_HOME]);
		obs_weak_source_release(_instance->menuScenes[MENU_CAMPAIGN]);
		obs_weak_source_release(_instance->menuScenes[MENU_COLLECTION]);
		obs_weak_source_release(_instance->menuScenes[MENU_COOP]);
		obs_weak_source_release(_instance->menuScenes[MENU_CUSTOM]);
		obs_weak_source_release(_instance->menuScenes[MENU_REPLAYS]);
		obs_weak_source_release(_instance->menuScenes[MENU_VERSUS]);
	}
}

Config::~Config() {
    //delete _instance;


}

void Config::checkForUpdates() {
	CURL *curl;
	CURLcode res;
 	std::string response;

 	curl = curl_easy_init();
	if (curl) {
		std::string reqURL = "https://api.github.com/repos/leigholiver/OBS-SC2Switcher/releases/latest";

		struct curl_slist *chunk = NULL;

		curl_easy_setopt(curl, CURLOPT_URL, reqURL.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 500);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		chunk = curl_slist_append(chunk, "User-Agent: OBS-SC2Switcher");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK) {
			return;
		}
	}

	json_error_t error;
	json_t* root = json_loads(response.c_str(), 0, &error);
	if (!root) {
		return;
	}

	json_t* url = json_object_get(root, "tag_name");
	const char *urlText = json_string_value(url);
	float latestVer = std::stof(urlText);
	float currentVer = static_cast<float>(1.0);
	if(latestVer > currentVer) {
		json_t* url2 = json_object_get(root, "html_url");
		const char *urlText2 = json_string_value(url2);
		updateURL = urlText2;

		json_t* patch = json_object_get(root, "body");
		const char *patchText = json_string_value(patch);
		updateDescription = patchText;
	}
}
