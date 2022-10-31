#pragma once

// the order is important with these as the labels are not 
// in a consistent order in sc2. the scene switcher will  
// use whichever it comes accross first. menu consts must
// go first so that they match up to the menuLabels array 
// properly. this is kind of bad? 
const int MENU_SCORESCREEN = 0;
const int MENU_PROFILE = 1;
const int MENU_LOBBY = 2;
const int MENU_HOME =  3;
const int MENU_CAMPAIGN =  4;
const int MENU_COLLECTION =  5;
const int MENU_COOP =  6;
const int MENU_CUSTOM =  7;
const int MENU_REPLAYS =  8;
const int MENU_VERSUS =  9;
const int MENU_NONE =  10;

const char* const menuLabels[]  = {
	"ScreenScore/ScreenScore",
	"ScreenUserProfile/ScreenUserProfile",
	"ScreenBattleLobby/ScreenBattleLobby",
	"ScreenHome/ScreenHome",
	"ScreenSingle/ScreenSingle",
	"ScreenCollection/ScreenCollection",
	"ScreenCoopCampaign/ScreenCoopCampaign",
	"ScreenCustom/ScreenCustom",
	"ScreenReplay/ScreenReplay",
	"ScreenMultiplayer/ScreenMultiplayer",
};

const int APP_INGAME = 11;
const int APP_MENU = 12;
const int APP_LOADING = 13;

const int GAME_INGAME = 14;
const int GAME_OBS = 15;
const int GAME_REPLAY = 16;