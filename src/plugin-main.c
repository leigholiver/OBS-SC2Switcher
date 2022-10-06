/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>

#include "plugin-macros.generated.h"

#include <QAction>
#include <QMainWindow>

#include "Config.h"
#include "log.h"
#include "SC2Data.h"
#include "SceneSwitcher.h"
#include "ScoreTracker.h"
#include "Webhook.h"
#include "SettingsDialog.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

SceneSwitcher* sw = nullptr;
ScoreTracker* st = nullptr;
Webhook* wh = nullptr;

bool obs_module_load(void) {
	blog(LOG_INFO, "plugin loaded successfully (version %s)",
	     PLUGIN_VERSION);
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
	blog(LOG_INFO, "plugin unloaded");
	Config::free();
	delete sw;
	delete wh;
}
