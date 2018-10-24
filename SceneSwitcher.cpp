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

SceneSwitcher::SceneSwitcher(SC2Data *sc2): Observer(sc2){ }

void SceneSwitcher::notify(SC2State*& previous, SC2State*& current) {
 	Config* cfg = Config::Current();
 	if(cfg->switcherEnabled) {
	 	OBSWeakSource scene;

	 	if(previous->appState != current->appState) {
	 		if(current->appState == APP_LOADING) {
	 			// pre game loading screen
	 			if(previous->appState == APP_MENU) {
	 				scene = cfg->inGameScene;
	 			}
	 		}

	 		// entering game
	 		if (current->appState == APP_INGAME && previous->appState != APP_INGAME) {
				if (current->gameState == GAME_REPLAY) {
					scene = cfg->replayScene;
				}
				else {
					scene = cfg->inGameScene;
				}
			}

			// leaving game 
			if (current->appState == APP_MENU && previous->appState != APP_MENU) {
	 			OBSWeakSource tmpScene;
				tmpScene = cfg->menuScenes[current->menuState];
				if (!tmpScene || current->menuState == MENU_NONE) {
					tmpScene = cfg->outGameScene;
				}
				scene = tmpScene;
			}
	 	}

	 	if (current->menuState != previous->menuState) {
	 		if(current->appState == APP_MENU) {
	 			OBSWeakSource tmpScene;
				tmpScene = cfg->menuScenes[current->menuState];
				// if (!tmpScene || current->menuState == MENU_NONE) {
				if (current->menuState == MENU_NONE) {
					tmpScene = cfg->outGameScene;
				}

				if(tmpScene) {
					scene = tmpScene;
				}
	 		}
	 	}

		if (current->gameState == GAME_OBS && current->appState == APP_INGAME) {
			if(current->gameState == GAME_OBS) {
				scene = cfg->obsScene;
			}
		}

		// scene change if necesary 
		if (scene) {
			obs_source_t *source = obs_weak_source_get_source(scene);
			obs_source_t *currentSource = obs_frontend_get_current_scene();

			if (source && currentSource && source != currentSource) {
				obs_frontend_set_current_scene(source);
			}
			obs_source_release(currentSource);
			obs_source_release(source);
		}
	}
}