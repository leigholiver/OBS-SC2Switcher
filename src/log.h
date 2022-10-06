#pragma once
#include <string>
#include <obs-frontend-api.h>
#include "Config.h"

static void s2log(std::string message) {
	Config* cfg = Config::Current();
	if(cfg->logging) {
		//blog(LOG_INFO, "[" PLUGIN_NAME "] " message.c_str());

		// do something with the message for compiler errors...
		message.c_str();
	}
}
