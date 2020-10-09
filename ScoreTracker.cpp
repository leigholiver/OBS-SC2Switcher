#include <QMainWindow>
#include <QObject>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QTimer>
#include <obs-frontend-api.h>

#include "SC2Data.h"
#include "Observer.h"
#include "SC2State.h"
#include "ScoreTracker.h"
#include "Constants.h"
#include "Config.h"
#include "APIState.h"
#include "obs-util.h"

ScoreTracker* _instance = nullptr;

ScoreTracker* ScoreTracker::Current() {
    return _instance;
}

ScoreTracker::~ScoreTracker() {
	delete _instance;
}

ScoreTracker::ScoreTracker(SC2Data *sc2): Observer(sc2){ 
	timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, this, [=]{ updateText(); });
    timer->start(1000);
    _instance = this;
}

std::string ScoreTracker::getName() { return "Score Tracker"; }

void ScoreTracker::notify(SC2State*& previous, SC2State*& current) {
 	Config* cfg = Config::Current();
	if(cfg->scoresEnabled && current->fullState.players.size() == 2) {
		bool leftGame = previous->appState != APP_MENU &&
				current->appState == APP_MENU &&
				!current->fullState.isReplay;

		if (leftGame || current->fullState.isRewind) {
			bool iAmPlayerA = false;
			bool iAmPlayerB = false;
			for (size_t i = 0; i < current->fullState.players.size(); i++) {
				for (size_t j= 0; j < cfg->usernames.size(); j++) {
					bool found = current->fullState.players[i]->name == cfg->usernames[j];
					if(found && i==0) {
						iAmPlayerA = true;
					}
					if(found && i==1) {
						iAmPlayerB = true;
					}
				}
			}
			if (iAmPlayerA && iAmPlayerB) {
				// sc2 only tells us the name and barcodes are a thing.common names could also cause stats to
				// be recorded incorrectly.only way to get accurate info is asking the user to confirm
				addConfirmMessage(current->fullState.players[0], current->fullState.players[1]);
			}
			else if (!iAmPlayerA && !iAmPlayerB) {
				// we didnt know which player the user was so we have to ask
				addConfirmMessage(current->fullState.players[0], current->fullState.players[1]);
			}
			else {
				// normal result
				if (iAmPlayerB) {
					recordScore(current->fullState.players[0]->race, current->fullState.players[1]->result);
				}
				else {
					recordScore(current->fullState.players[1]->race, current->fullState.players[0]->result);
				}
			}
		}
	}
}

std::string ScoreTracker::getScoreString() {
	// this probably needs refactoring
	Config* cfg = Config::Current();
	std::string output = cfg->scoreString;
	std::map <size_t, std::map <std::string, std::string>> scoreMap;

	scoreMap[0]["search"] = "${tw}";
	scoreMap[0]["replace"] = std::to_string(scores["Terr"]["Victory"]);
	scoreMap[1]["search"] = "${tl}";
	scoreMap[1]["replace"] = std::to_string(scores["Terr"]["Defeat"]);

	scoreMap[2]["search"] = "${zw}";
	scoreMap[2]["replace"] = std::to_string(scores["Zerg"]["Victory"]);
	scoreMap[3]["search"] = "${zl}";
	scoreMap[3]["replace"] = std::to_string(scores["Zerg"]["Defeat"]);

	scoreMap[4]["search"] = "${pw}";
	scoreMap[4]["replace"] = std::to_string(scores["Prot"]["Victory"]);
	scoreMap[5]["search"] = "${pl}";
	scoreMap[5]["replace"] = std::to_string(scores["Prot"]["Defeat"]);

	for (size_t i = 0; i < scoreMap.size(); i++) {
		std::string search = scoreMap[i]["search"];
		std::string replace = scoreMap[i]["replace"];
		size_t pos = 0;
		while ((pos = output.find(search, pos)) != std::string::npos) {
			output.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}
	
	return output;
}

void ScoreTracker::updateText() {
	Config* cfg = Config::Current();
	obs_source_t *source;
	try {
		source = obs_get_source_by_name(cfg->textSourceName.c_str());
	}
	catch (...) {
		return;
	}
	if (source) {
		obs_data_t *obj = obs_data_create();
		obs_data_set_string(obj, "text", getScoreString().c_str());
		obs_source_update(source, obj);
		obs_data_release(obj);
	}
	obs_source_release(source);
}

void ScoreTracker::recordScore(std::string race, std::string result) {
	if (race == "random") {
		addRandomConfirmMessage(result);
	}
	else {
		scores[race][result] = scores[race][result] + 1;
		updateText();
	}
}

void ScoreTracker::handleButton(std::string race, std::string result, std::string name) {
	Config* cfg = Config::Current();
	if (std::find(cfg->usernames.begin(), cfg->usernames.end(), name) == cfg->usernames.end()) {
		cfg->usernames.push_back(name);
	}
	recordScore(race, result);
}

void ScoreTracker::addRandomConfirmMessage(std::string result) {
	Config* cfg = Config::Current();
	if(cfg->popupsEnabled) {
		QMainWindow* mainWindow = (QMainWindow*)obs_frontend_get_main_window();
		QMessageBox* msgBox = new QMessageBox(mainWindow);
		msgBox->setText("Which race was your opponent playing?");

		QAbstractButton* pButtonT = msgBox->addButton("Terran", QMessageBox::ActionRole);
		QObject::connect(pButtonT, &QPushButton::released, this, [=]{
			handleButton("Terr", result, "");
		});

		QAbstractButton* pButtonZ = msgBox->addButton("Zerg", QMessageBox::ActionRole);
		QObject::connect(pButtonZ, &QPushButton::released, this, [=]{
			handleButton("Zerg", result, "");
		});

		QAbstractButton* pButtonP = msgBox->addButton("Protoss", QMessageBox::ActionRole);
		QObject::connect(pButtonP, &QPushButton::released, this, [=]{
			handleButton("Prot", result, "");
		});
		msgBox->setWindowModality(Qt::NonModal);
		msgBox->setAttribute(Qt::WA_ShowWithoutActivating);
		msgBox->setWindowFlags(Qt::Tool);
		msgBox->open();
	}
	else {
		// find another way to handle them that doesnt steal focus 
	}
}


void ScoreTracker::addConfirmMessage(player* playerA, player* playerB) {
	Config* cfg = Config::Current();
	if(cfg->popupsEnabled) {
		QMainWindow* mainWindow = (QMainWindow*)obs_frontend_get_main_window();
		QMessageBox* msgBox = new QMessageBox(mainWindow);
		msgBox->setText("Which player were you?");

		char playerAButton[50];
		sprintf(playerAButton, "%s: %s (%s)", playerA->race, playerA->name, playerA->result);

		QAbstractButton* pButtonT = msgBox->addButton(playerAButton, QMessageBox::ActionRole);
		QObject::connect(pButtonT, &QPushButton::released, this, [=]{
			handleButton(playerB->race, playerA->result, playerA->name);
		});

		char playerBButton[50];
		sprintf(playerBButton, "%s: %s (%s)", playerB->race, playerB->name, playerB->result);
		QAbstractButton* pButtonZ = msgBox->addButton(playerBButton, QMessageBox::ActionRole);
		QObject::connect(pButtonZ, &QPushButton::released, this, [=]{
			handleButton(playerA->race, playerB->result, playerB->name);
		});

		msgBox->setWindowModality(Qt::NonModal);
		msgBox->setAttribute(Qt::WA_ShowWithoutActivating);
		msgBox->setWindowFlags(Qt::Tool);
		msgBox->open();
	}
	else {
		// find another way to handle them that doesnt steal focus 
	}
	
}

