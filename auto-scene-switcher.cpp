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
	string ipAddr;
	bool inGame = false;
	bool apiInGame = false;
	int interval = 3000;

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
	BPtr<char*> scenes = obs_frontend_get_scene_names();
	char **temp = scenes;
	while (*temp) {
		const char *name = *temp;
		ui->inGameScene->addItem(name);
		ui->outGameScene->addItem(name);
		temp++;
	}

	ui->inGameScene->setCurrentText(GetWeakSourceName(switcher->inGameScene).c_str());
	ui->outGameScene->setCurrentText(GetWeakSourceName(switcher->outGameScene).c_str());
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
	std::string responseFromServer;

	curl = curl_easy_init();
	if (curl) {
		string reqURL = switcher->ipAddr;
		if (reqURL == "") {
			reqURL = "localhost";
		}
		reqURL = "http://" + reqURL + ":6119/ui";

		curl_easy_setopt(curl, CURLOPT_URL, reqURL.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseFromServer);
		res = curl_easy_perform(curl);

		lock_guard<mutex> lock(switcher->m);
		// this is disgusting but im just prototyping and i kinda dont really need to go
		// through the hassle of working out json parsing libraries #SorryNotSorry
		// ill fix this when i add the replay scene option
		if (responseFromServer == "{\"activeScreens\":[\"ScreenLoading/ScreenLoading\"]}" || responseFromServer == "{\"activeScreens\":[]}")
		{
			switcher->apiInGame = true;
		}
		else {
			switcher->apiInGame = false;
		}
	}
	curl_easy_cleanup(curl);
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

		if ((!apiInGame && inGame) || (apiInGame && !inGame)) {
			inGame = !inGame;

			OBSWeakSource scene = switcher->outGameScene;
			if (inGame) {
				scene = switcher->inGameScene;
			}

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