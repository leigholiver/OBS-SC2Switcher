#include <QMainWindow>
#include <QObject>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>

#include <obs-frontend-api.h>

#include "SC2Data.h"
#include "Observer.h"
#include "SC2State.h"
#include "SceneSwitcher.h"
#include "Constants.h"
#include "Config.h"
#include "obs-util.h"
#include "log.h"

SceneSwitcher::SceneSwitcher(SC2Data *sc2): Observer(sc2){ }
std::string SceneSwitcher::getName() { return "Scene Switcher"; }

void SceneSwitcher::notify(SC2State*& previous, SC2State*& current) {
 	Config* cfg = Config::Current();
 	if(cfg->switcherEnabled) {
	 	OBSWeakSource scene;

	 	if(previous->appState != current->appState) {
	 		if(current->appState == APP_LOADING) {
	 			// pre game loading screen
				if (previous->appState == APP_MENU) {
					if(cfg->switchOnLoad) {
						s2log("Loading Screen, entering game. Switching to In Game Scene");
	 					scene = cfg->inGameScene;
	 				}
	 				else {
						s2log("Loading Screen, entering game. Switch On Load Disabled - Not Switching");
	 				}
	 			}
	 		}

	 		// entering game
	 		if (current->appState == APP_INGAME && previous->appState != APP_INGAME) {
				if (current->gameState == GAME_REPLAY) {
					s2log("Entered Replay. Switching to Replay Scene");
					scene = cfg->replayScene;
				}
				else {
					s2log("Entered Game. Switching to In Game Scene");
					scene = cfg->inGameScene;
				}
			}

			// leaving game 
			if (current->appState == APP_MENU && previous->appState != APP_MENU) {
				s2log("Entered Menus. Finding Menu Scene");
	 			OBSWeakSource tmpScene;

	 			s2log("current->menustate is set: " + to_string(current->menuState));
				if (current->menuState == MENU_NONE) {
					s2log("Menu state is MENU_NONE. Switching to Out of Game Scene");
					tmpScene = cfg->outGameScene;
				}
				else {
					s2log("Menustate not MENU_NONE, finding scene");
					tmpScene = cfg->menuScenes[current->menuState];
					if (!tmpScene) {
						s2log("No menu set, using out of game scene");
						tmpScene = cfg->outGameScene;
					}
				}

				if(tmpScene) {
					s2log("Switching to specified menu Scene");
					scene = tmpScene;
				}
				else {
					s2log("No menu scene specified, will not switch");
				}

			}
	 	}

	 	if (current->appState == APP_MENU && current->menuState != previous->menuState) {
			s2log("Menu changed, finding scene");
 			OBSWeakSource tmpScene2;
			
			if (current->menuState == MENU_NONE) {
				s2log("Menu state is MENU_NONE. Switching to Out of Game Scene");
				tmpScene2 = cfg->outGameScene;
			}
			else {
				s2log("Menustate not MENU_NONE, finding scene");
				tmpScene2 = cfg->menuScenes[current->menuState];
			}

			if(tmpScene2) {
				s2log("Switching to specified menu Scene");
				scene = tmpScene2;
			}
			else {
				s2log("No menu scene specified, will not switch");
			}
	 	}

		if (current->gameState == GAME_OBS && current->appState == APP_INGAME) {
			if(current->gameState == GAME_OBS) {
				s2log("Detected observer, switching to observer scene");
				if(cfg->obsScene) {
					scene = cfg->obsScene;
				}
				else {
					s2log("No observer scene found, switching to in game scene");
					scene = cfg->inGameScene;
				}
			}
		}

		// scene change if necesary 
		if (scene) {
			std::string sceneName = GetWeakSourceName(scene);
			s2log("Attempting Scene Switch to: " + sceneName);

			obs_source_t *source = obs_weak_source_get_source(scene);
			obs_source_t *currentSource = obs_frontend_get_current_scene();

			if (source && source != currentSource) {
				obs_frontend_set_current_scene(source);
				s2log("Switched Scenes");
			}
			else {
				s2log("Could not switch scenes, scene does not exist or already in that scene.");
			}

			obs_source_release(currentSource);
			obs_source_release(source);
		}
	}
}