#include <obs-frontend-api.h>
#include <obs-module.h>
#include <obs.hpp>
#include <util/util.hpp>
#include <QMainWindow>
#include <QMessageBox>
#include <QAction>
#include "auto-scene-switcher.hpp"
#include "tool-helpers.hpp"
#include <curl/curl.h>
#include <condition_variable>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <regex>
#include <mutex>
#include <string.h>
#include <jansson.h>

using namespace std;

struct SwitcherData {
	thread th;
	condition_variable cv;
	mutex m;
	bool stop = false;
	thread t; // curl thread

	void Thread();
	void Start();
	void Stop();
	void api_callback();

	OBSWeakSource inGameScene;
	OBSWeakSource outGameScene;
	OBSWeakSource replayScene;
	string ipAddr;

	bool hasHitMenu = false;

	bool inGame = false;
	bool inReplay = false;
	bool apiInGame = false;
	bool apiInReplay = false;
	bool apiIsLoading = false;

	int state = 0;
	int apiState = 0;

	int STATE_INGAME = 1;
	int STATE_REPLAY = 2;
	int STATE_MENUS  = 3;
	int STATE_LOADING  = 4;
	
	int interval = 1500;

	inline SwitcherData() {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	inline ~SwitcherData()
	{
		Stop();
		curl_global_cleanup();
	}
};
static SwitcherData *switcher = nullptr;

void SwitcherData::Start()
{
	if (!switcher->th.joinable())
		switcher->th = thread([]() {switcher->Thread(); });
}

void SwitcherData::Stop()
{
	if (th.joinable()) {
		{
			lock_guard<mutex> lock(m);
			stop = true;
		}
		cv.notify_one();
		th.join();
	}
}

static inline bool WeakSourceValid(obs_weak_source_t *ws)
{
	obs_source_t *source = obs_weak_source_get_source(ws);
	if (source)
		obs_source_release(source);
	return !!source;
}

SceneSwitcher::SceneSwitcher(QWidget *parent)
	: QDialog(parent),
	ui(new Ui_SceneSwitcher)
{
	ui->setupUi(this);
	lock_guard<mutex> lock(switcher->m);

	ui->inGameScene->addItem("");
	ui->outGameScene->addItem("");
	ui->replayScene->addItem("");
	BPtr<char*> scenes = obs_frontend_get_scene_names();
	char **temp = scenes;
	while (*temp) {
		const char *name = *temp;
		ui->inGameScene->addItem(name);
		ui->outGameScene->addItem(name);
		ui->replayScene->addItem(name);
		temp++;
	}

	ui->inGameScene->setCurrentText(GetWeakSourceName(switcher->inGameScene).c_str());
	ui->outGameScene->setCurrentText(GetWeakSourceName(switcher->outGameScene).c_str());
	ui->replayScene->setCurrentText(GetWeakSourceName(switcher->replayScene).c_str());
	ui->ipAddr->setText(QString::fromStdString(switcher->ipAddr));

	if (switcher->th.joinable()) {
		ui->toggleStartButton->setText(obs_module_text("Stop"));
	}
	else {
		ui->toggleStartButton->setText(obs_module_text("Start"));
	}

	loading = false;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void SwitcherData::api_callback() {
	CURL *curl;
	CURLcode res;
	std::string UIResponse;

	curl = curl_easy_init();
	if (curl) {
		string reqURL = switcher->ipAddr;
		if (reqURL == "") {
			reqURL = "localhost";
		}
		reqURL = "http://" + reqURL + ":6119/ui";

		curl_easy_setopt(curl, CURLOPT_URL, reqURL.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 500);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &UIResponse);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return;
		}
	}
	curl_easy_cleanup(curl);

	std::string gameResponse;

	curl = curl_easy_init();
	if (curl) {
		string reqURL = switcher->ipAddr;
		if (reqURL == "") {
			reqURL = "localhost";
		}
		reqURL = "http://" + reqURL + ":6119/game";

		curl_easy_setopt(curl, CURLOPT_URL, reqURL.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 500);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gameResponse);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return;
		}
	}
	curl_easy_cleanup(curl);

	lock_guard<mutex> lock(switcher->m);

	json_error_t error;
	json_t* root = json_loads(UIResponse.c_str(), 0, &error);
	if (!root) {
		return;
	}
	json_t* screens = json_object_get(root, "activeScreens");
	if (json_array_size(screens) > 1) {
		/*
			this is where you could extend this to other screens
			you have to iterate over them all, they arent in a consistent order

			interesting ones: 
		
			ScreenMultiplayer/ScreenMultiplayer
				ScreenMatchmaking/ScreenMatchmaking
				ScreenCustomGames/ScreenCustomGames
					ScreenBattleLobby/ScreenBattleLobby
				ScreenTournament/ScreenTournament

			ScreenReplay/ScreenReplay
			ScreenCoopCampaign/ScreenCoopCampaign
			ScreenCollection/ScreenCollection

			ScreenSingle/ScreenSingle      // (campaign)
		*/

		// in menus
		switcher->apiState = STATE_MENUS;
	}
	else if (json_array_size(screens) == 0 ) {
		// in game
		switcher->apiState = STATE_INGAME;
	}
	else {
		// we are in the loading screen
		switcher->apiState = STATE_LOADING;
	}
	
	root = json_loads(gameResponse.c_str(), 0, &error);
	if (!root) {
		return;
	}
	json_t* isReplay = json_object_get(root, "isReplay");
	if (isReplay == json_true()) {
		switcher->apiInReplay = true;
	}
	else {
		switcher->apiInReplay = false;
	}
}

void SwitcherData::Thread() {
	chrono::duration<long long, milli> duration =
		chrono::milliseconds(interval);

	for (;;) {

		thread t(&SwitcherData::api_callback, this);

		unique_lock<mutex> lock(m);
		cv.wait_for(lock, duration);

		t.join();
		if (switcher->stop) {
			switcher->stop = false;
			break;
		}
		duration = chrono::milliseconds(interval);
		
		OBSWeakSource scene;
		if (switcher->apiState != switcher->state) {
			if (switcher->apiState == STATE_LOADING) {
				if (switcher->state == STATE_MENUS) {
					// enter game event, show in game scene for loading screen w/ player info etc
					scene = switcher->inGameScene;
				}
				else if (switcher->state == STATE_INGAME ) {
					// exit game event 
					scene = switcher->outGameScene;
				}
			}
			if (switcher->apiState == STATE_INGAME && switcher->state != STATE_INGAME) {
				if (switcher->apiInReplay) {
					scene = switcher->replayScene;
				}
				else {
					scene = switcher->inGameScene;
				}
			}
			if (switcher->apiState == STATE_MENUS && switcher->state != STATE_MENUS) {
				scene = switcher->outGameScene;
			}
			// update the state
			switcher->state = switcher->apiState;
		}

		if (scene) {
			obs_source_t *source = obs_weak_source_get_source(scene);
			obs_source_t *currentSource = obs_frontend_get_current_scene();

			if (source && source != currentSource)
				obs_frontend_set_current_scene(source);

			obs_source_release(currentSource);
			obs_source_release(source);

		}
	}
}

void SceneSwitcher::closeEvent(QCloseEvent*)
{
	obs_frontend_save();
}

void SceneSwitcher::on_inGameScene_currentTextChanged(const QString& text)
{
	if (!loading) {
		lock_guard<mutex> lock(switcher->m);
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		switcher->inGameScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SceneSwitcher::on_outGameScene_currentTextChanged(const QString& text)
{
	if (!loading) {
		lock_guard<mutex> lock(switcher->m);
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		switcher->outGameScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SceneSwitcher::on_replayScene_currentTextChanged(const QString& text)
{
	if (!loading) {
		lock_guard<mutex> lock(switcher->m);
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		switcher->replayScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SceneSwitcher::on_ipAddr_textChanged(const QString& text)
{
	if (!loading) {
		lock_guard<mutex> lock(switcher->m);
		switcher->ipAddr = (string)text.toUtf8().constData();
	}
}

void SceneSwitcher::on_toggleStartButton_clicked()
{
	if (switcher->th.joinable()) {
		switcher->Stop();
		ui->toggleStartButton->setText(obs_module_text("Start"));
	}
	else {
		switcher->Start();
		ui->toggleStartButton->setText(obs_module_text("Stop"));
	}
}

static void SaveSceneSwitcher(obs_data_t *save_data, bool saving, void *)
{
	if (saving) {
		lock_guard<mutex> lock(switcher->m);
		obs_data_t *obj = obs_data_create();

		string inGameSceneName = GetWeakSourceName(switcher->inGameScene);
		obs_data_set_string(obj, "in_game_scene", inGameSceneName.c_str());

		string outGameSceneName = GetWeakSourceName(switcher->outGameScene);
		obs_data_set_string(obj, "out_game_scene", outGameSceneName.c_str());

		string replaySceneName = GetWeakSourceName(switcher->replayScene);
		obs_data_set_string(obj, "replay_scene", replaySceneName.c_str());

		obs_data_set_bool(obj, "is_running", switcher->th.joinable());
		obs_data_set_string(obj, "ip_addr", switcher->ipAddr.c_str());

		obs_data_set_obj(save_data, "sc2switcher", obj);
		obs_data_release(obj);
	}
	else {
		switcher->m.lock();
		obs_data_t *obj = obs_data_get_obj(save_data, "sc2switcher");

		if (!obj)
			obj = obs_data_create();

		string inGameScene =
			obs_data_get_string(obj, "in_game_scene");
		switcher->inGameScene = GetWeakSourceByName(inGameScene.c_str());

		string outGameScene =
			obs_data_get_string(obj, "out_game_scene");
		switcher->outGameScene = GetWeakSourceByName(outGameScene.c_str());

		string replayScene =
			obs_data_get_string(obj, "replay_scene");
		switcher->replayScene = GetWeakSourceByName(replayScene.c_str());

		string ipAddr = obs_data_get_string(obj, "ip_addr");
		switcher->ipAddr = ipAddr;

		switcher->m.unlock();

		if (obs_data_get_bool(obj, "is_running")) {
			switcher->Start();
		}
		else {
			switcher->Stop();
		}

		obs_data_release(obj);
	}
}

extern "C" void FreeSceneSwitcher()
{
	delete switcher;
	switcher = nullptr;
}

static void OBSEvent(enum obs_frontend_event event, void *)
{
	if (event == OBS_FRONTEND_EVENT_EXIT)
		FreeSceneSwitcher();
}

extern "C" void InitSceneSwitcher()
{
	QAction *action = (QAction*)obs_frontend_add_tools_menu_qaction(
		"SC2 Scene Switcher");

	switcher = new SwitcherData;
	auto cb = []()
	{
		obs_frontend_push_ui_translation(obs_module_get_string);

		QMainWindow *window =
			(QMainWindow*)obs_frontend_get_main_window();

		SceneSwitcher ss(window);
		ss.exec();

		obs_frontend_pop_ui_translation();
	};

	obs_frontend_add_save_callback(SaveSceneSwitcher, nullptr);
	obs_frontend_add_event_callback(OBSEvent, nullptr);
	action->connect(action, &QAction::triggered, cb);
}