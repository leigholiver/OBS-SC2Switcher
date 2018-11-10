#include <obs-module.h>
#include <QAction>
#include <QMainWindow>

#include "Config.h"
#include "SC2Data.h"
#include "SceneSwitcher.h"
#include "ScoreTracker.h"
#include "Webhook.h"
#include "forms/SettingsDialog.h"

OBS_DECLARE_MODULE()

SceneSwitcher* sw = nullptr;
ScoreTracker* st = nullptr;
Webhook* wh = nullptr;

bool obs_module_load(void) {
	// set up load/save
	obs_frontend_add_save_callback(LoadSaveHandler, nullptr);

	//set up the settings dialog
	QAction *action = (QAction*)obs_frontend_add_tools_menu_qaction(
		"SC2Switcher");
	auto cb = []() {
		QMainWindow *window =
			(QMainWindow*)obs_frontend_get_main_window();
		SettingsDialog sd(window);
		sd.exec();
	};

	action->connect(action, &QAction::triggered, cb);

	// sc2 
	SC2Data::Instance = new SC2Data();

	// listeners
	sw = new SceneSwitcher(SC2Data::Instance);
	st = new ScoreTracker(SC2Data::Instance);
	wh = new Webhook(SC2Data::Instance);

	return true;
}

void obs_module_unload(void) {
	delete sw;
	delete wh;
	Config* cfg = Config::Current();
	cfg->~Config();
}
