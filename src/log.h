#pragma once
#include <string>
#include <obs-frontend-api.h>
#include "Config.h"
#define PLUGIN_NAME "@CMAKE_PROJECT_NAME@"

static void s2log(std::string message) {
	Config* cfg = Config::Current();
	if(cfg->logging) {
		blog(level, "[" PLUGIN_NAME "] " message.c_str());
	}
}
