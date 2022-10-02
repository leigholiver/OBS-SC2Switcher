#pragma once

struct player {
	const char* name;
	const char* type;
	const char* race;
	const char* result;
};

struct APIState {
	std::vector<const char *> activeScreens;
	bool isReplay;
	bool isRewind;
	double displayTime;
	std::vector<player*> players;
};
