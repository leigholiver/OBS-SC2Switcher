#pragma once
#include <string>
#include <obs-frontend-api.h>
#include "Config.h"

static void s2log(std::string message) {
	Config* cfg = Config::Current();
	if(cfg->logging) {
		blog(LOG_INFO, std::string("[sc2switcher] " + message).c_str());
	}
}
