#pragma once
#include <string>
#include <obs-frontend-api.h>
#include "Config.h"

static void s2log(std::string message) {
	Config* cfg = Config::Current();
	if(cfg->logging) {
		blog(LOG_INFO, sprintf("[sc2switcher] %s".c_str(), message.c_str()));
	}
}
