#include <obs-frontend-api.h>
#include <util/util.hpp>

#include "ScoreTracker.h"
#include "SettingsDialog.h"
#include "Config.h"
#include "Constants.h"
#include "obs-util.h"

Config* config = Config::Current();

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent, Qt::Dialog),
    ui(new Ui::SettingsDialog)
{
	isLoading = true;
    ui->setupUi(this);

    // fill them boxes
	ui->inGameScene->addItem("");
	ui->outGameScene->addItem("");
	ui->replayScene->addItem("");

	ui->obsScene->addItem("");
	ui->replaysScene->addItem("");
	ui->campaignScene->addItem("");
	ui->homeScene->addItem("");
	ui->collectionScene->addItem("");
	ui->coopScene->addItem("");
	ui->customScene->addItem("");
	ui->lobbyScene->addItem("");
	ui->scoreScreenScene->addItem("");
	ui->profileScene->addItem("");
	ui->versusScene->addItem("");

	BPtr<char*> scenes = obs_frontend_get_scene_names();
	char **temp = scenes;
	while (*temp) {
		const char *name = *temp;
		ui->inGameScene->addItem(name);
		ui->outGameScene->addItem(name);
		ui->replayScene->addItem(name);
		ui->obsScene->addItem(name);
		ui->replaysScene->addItem(name);
		ui->campaignScene->addItem(name);
		ui->homeScene->addItem(name);
		ui->collectionScene->addItem(name);
		ui->coopScene->addItem(name);
		ui->customScene->addItem(name);
		ui->lobbyScene->addItem(name);
		ui->scoreScreenScene->addItem(name);
		ui->profileScene->addItem(name);
		ui->versusScene->addItem(name);

		temp++;
	}

	ui->inGameScene->setCurrentText(GetWeakSourceName(config->inGameScene).c_str());
	ui->outGameScene->setCurrentText(GetWeakSourceName(config->outGameScene).c_str());
	ui->replayScene->setCurrentText(GetWeakSourceName(config->replayScene).c_str());

	// there has GOT to be a better way to do this
	ui->obsScene->setCurrentText(GetWeakSourceName(config->obsScene).c_str());
	ui->replaysScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_REPLAYS]).c_str());
	ui->campaignScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_CAMPAIGN]).c_str());
	ui->homeScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_HOME]).c_str());
	ui->collectionScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_COLLECTION]).c_str());
	ui->coopScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_COOP]).c_str());
	ui->customScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_CUSTOM]).c_str());
	ui->lobbyScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_LOBBY]).c_str());
	ui->scoreScreenScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_SCORESCREEN]).c_str());
	ui->profileScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_PROFILE]).c_str());
	ui->versusScene->setCurrentText(GetWeakSourceName(config->menuScenes[MENU_VERSUS]).c_str());

	for (size_t i = 0; i < config->usernames.size(); i++) {
		if(config->usernames[i] != "") {
			ui->userNames->addItem(config->usernames[i].c_str());
		}
	}

	for (size_t i = 0; i < config->recentUsernames.size(); i++) {
		ui->recentUsernames->addItem(config->recentUsernames[i].c_str());
	}

	ui->ipAddr->setText(QString::fromStdString(config->ipAddr));

	config->checkForUpdates();
	if(config->updateURL != "") {
		ui->downloadURL->setText(QString::fromStdString("<a href=\"" + config->updateURL + "\">Update Available. Click here to download.</a>"));
		ui->downloadURL->setTextFormat(Qt::RichText);
		ui->downloadURL->setTextInteractionFlags(Qt::TextBrowserInteraction);
		ui->downloadURL->setOpenExternalLinks(true);
		ui->patchNotesLabel->setText(config->updateDescription.c_str());
	}

	if (config->isRunning) {
		ui->toggleStartButton->setText("Stop");
	}
	else {
		ui->toggleStartButton->setText("Start");
	}

	ui->textTemplate->setText(QString::fromStdString(config->scoreString));
	ui->textSourceName->setText(QString::fromStdString(config->textSourceName));

	if(config->switcherEnabled) {
		ui->switcherEnabled->setChecked(true);
	}
	else {
		ui->switcherEnabled->setChecked(false);
	}
	if(config->scoresEnabled) {
		ui->scoresEnabled->setChecked(true);
	}
	else {
		ui->scoresEnabled->setChecked(false);
	}
	if(config->popupsEnabled) {
		ui->popupsEnabled->setChecked(true);
	}
	else {
		ui->popupsEnabled->setChecked(false);
	}
	if(config->clearSettings) {
		ui->clearSettings->setChecked(true);
	}
	else {
		ui->clearSettings->setChecked(false);
	}

	if(config->logging) {
		ui->logging->setChecked(true);
	}
	else {
		ui->logging->setChecked(false);
	}

	if(config->switchOnLoad) {
		ui->switchOnLoad->setChecked(true);
	}
	else {
		ui->switchOnLoad->setChecked(false);
	}

	if(config->webhookEnabled) {
		ui->webhookEnabled->setChecked(true);
	}

	for (size_t i = 0; i < config->webhookURLList.size(); i++) {
		ui->webhookURLList->addItem(config->webhookURLList[i].c_str());
	}

	ScoreTracker* st = ScoreTracker::Current();
	ui->scoresLabel->setText(st->getScoreString().c_str());


	isLoading = false;
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::closeEvent(QCloseEvent*) {
	//obs_frontend_save();
}


void SettingsDialog::on_inGameScene_currentTextChanged(const QString& text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->inGameScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_outGameScene_currentTextChanged(const QString& text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->outGameScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_replayScene_currentTextChanged(const QString& text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->replayScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_ipAddr_textChanged(const QString& text) {
	if(!isLoading) {
		config->ipAddr = (std::string) text.toUtf8().constData();
	}
}

void SettingsDialog::on_toggleStartButton_clicked()
{
	if (config->isRunning) {
		config->isRunning = false;
		ui->toggleStartButton->setText("Start");
	}
	else {
		config->isRunning = true;
		ui->toggleStartButton->setText("Stop");
	}
}

void SettingsDialog::on_obsScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->obsScene = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_replaysScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_REPLAYS] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_campaignScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_CAMPAIGN] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_homeScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_HOME] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_collectionScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_COLLECTION] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}

}

void SettingsDialog::on_coopScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_COOP] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}

}

void SettingsDialog::on_customScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_CUSTOM] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_lobbyScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_LOBBY] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_scoreScreenScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_SCORESCREEN] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_profileScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_PROFILE] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_versusScene_currentTextChanged(const QString &text) {
	if(!isLoading) {
		obs_source_t* scene = obs_get_source_by_name(text.toUtf8().constData());
		obs_weak_source_t* ws = obs_source_get_weak_source(scene);

		config->menuScenes[MENU_VERSUS] = ws;

		obs_weak_source_release(ws);
		obs_source_release(scene);
	}
}

void SettingsDialog::on_addUsernameButton_clicked() {
	if(!isLoading) {
		// add the username to our usernames
		std::string username = ui->usernameLine->text().toUtf8().constData();

		if (std::find(config->usernames.begin(), config->usernames.end(), username) == config->usernames.end() && username != "") {
			config->usernames.push_back(username);		// add it to the ui list
			ui->userNames->addItem(ui->usernameLine->text());
		}

		// clear the username input
		ui->usernameLine->setText("");
	}
}

void SettingsDialog::on_removeUsernameButton_clicked() {
	if(!isLoading) {
		// remove the list item from our usernames
		config->usernames.erase(std::remove(config->usernames.begin(), config->usernames.end(), ui->usernameLine->text().toUtf8().constData()), config->usernames.end());

		// remove the list item from the ui list
		// have to iterate backwards because the size of the list changes as items are removed
		// also store the name first as the value of usernameLine changes as items are removed and new items selected
		std::string searchName = ui->usernameLine->text().toUtf8().constData();
		for (int i = ui->userNames->count()-1; i >= 0; i--) {
			QListWidgetItem *item = ui->userNames->item(i);
			if (item->text() == QString::fromStdString(searchName)) {
				int row = ui->userNames->row(item);
				delete ui->userNames->takeItem(row);
			}
		}

		// clear the username box if its empty
		// otherwise box will hold selected item
		if (ui->userNames->count() == 0) {
			ui->usernameLine->setText("");
		}
	}
}

void SettingsDialog::on_userNames_itemSelectionChanged() {
	if(!isLoading) {
		if (ui->userNames->currentItem() != NULL && ui->userNames->currentItem()->text().isNull() == false ) {
			ui->usernameLine->setText(ui->userNames->currentItem()->text());
		}
	}
}

void SettingsDialog::on_recentUsernames_itemSelectionChanged() {
	if(!isLoading) {
		if (ui->recentUsernames->currentItem() != NULL && ui->recentUsernames->currentItem()->text().isNull() == false) {
			ui->usernameLine->setText(ui->recentUsernames->currentItem()->text());
		}
	}
}

void SettingsDialog::on_textSourceName_textChanged(const QString& text) {
	if(!isLoading) {
		if (config->textSourceName != (std::string)text.toUtf8().constData()) {
			config->textSourceName = (std::string)text.toUtf8().constData();
		}
	}
}

void SettingsDialog::on_textTemplate_textChanged() {
	if(!isLoading) {
		if (config->scoreString != (std::string)ui->textTemplate->toPlainText().toUtf8().constData()) {
			config->scoreString = (std::string)ui->textTemplate->toPlainText().toUtf8().constData();
		}
	}
}

void SettingsDialog::on_switcherEnabled_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->switcherEnabled = true;
		}
		else {
			config->switcherEnabled = false;
		}
	}
}

void SettingsDialog::on_logging_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->logging = true;
		}
		else {
			config->logging = false;
		}
	}
}

void SettingsDialog::on_switchOnLoad_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->switchOnLoad = true;
		}
		else {
			config->switchOnLoad = false;
		}
	}
}


void SettingsDialog::on_scoresEnabled_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->scoresEnabled = true;
		}
		else {
			config->scoresEnabled = false;
		}
	}
}

void SettingsDialog::on_clearSettings_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->clearSettings = true;
		}
		else {
			config->clearSettings = false;
		}
	}
}

void SettingsDialog::on_popupsEnabled_stateChanged(int state) {
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->popupsEnabled = true;
		}
		else {
			config->popupsEnabled = false;
		}
	}
}


void SettingsDialog::on_webhookEnabled_stateChanged(int state){
	if(!isLoading) {
		if(state == Qt::Checked) {
			config->webhookEnabled = true;
		}
		else {
			config->webhookEnabled = false;
		}
	}
}

void SettingsDialog::on_tWinPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading && st) {
		st->scores["Terr"]["Victory"] = st->scores["Terr"]["Victory"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_tWinMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if (!isLoading) {
		if (st->scores["Terr"]["Victory"] > 0) {
			st->scores["Terr"]["Victory"] = st->scores["Terr"]["Victory"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}

void SettingsDialog::on_tLossPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		st->scores["Terr"]["Defeat"] = st->scores["Terr"]["Defeat"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_tLossMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if (!isLoading) {
		if (st->scores["Terr"]["Defeat"] > 0) {
			st->scores["Terr"]["Defeat"] = st->scores["Terr"]["Defeat"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}

void SettingsDialog::on_zWinPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		st->scores["Zerg"]["Victory"] = st->scores["Zerg"]["Victory"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_zWinMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if (!isLoading) {
		if (st->scores["Zerg"]["Victory"] > 0) {
			st->scores["Zerg"]["Victory"] = st->scores["Zerg"]["Victory"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}

void SettingsDialog::on_zLossPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		st->scores["Zerg"]["Defeat"] = st->scores["Zerg"]["Defeat"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_zLossMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if (!isLoading) {
		if (st->scores["Zerg"]["Defeat"] > 0) {
			st->scores["Zerg"]["Defeat"] = st->scores["Zerg"]["Defeat"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}

void SettingsDialog::on_pWinPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		st->scores["Prot"]["Victory"] = st->scores["Prot"]["Victory"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_pWinMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if (!isLoading) {
		if (st->scores["Prot"]["Victory"] > 0) {
			st->scores["Prot"]["Victory"] = st->scores["Prot"]["Victory"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}

void SettingsDialog::on_pLossPlus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		st->scores["Prot"]["Defeat"] = st->scores["Prot"]["Defeat"] + 1;
		st->updateText();
		ui->scoresLabel->setText(st->getScoreString().c_str());
	}
}

void SettingsDialog::on_pLossMinus_clicked() {
	ScoreTracker* st = ScoreTracker::Current();
	if(!isLoading) {
		if (st->scores["Prot"]["Defeat"] > 0) {
			st->scores["Prot"]["Defeat"] = st->scores["Prot"]["Defeat"] - 1;
			st->updateText();
			ui->scoresLabel->setText(st->getScoreString().c_str());
		}
	}
}


void SettingsDialog::on_addURLButton_clicked() {
	if(!isLoading) {
		std::string username = ui->webhookEnterGame->text().toUtf8().constData();
		config->webhookURLList.push_back(username);
		ui->webhookURLList->addItem(ui->webhookEnterGame->text());
		ui->webhookEnterGame->setText("");
	}
}

void SettingsDialog::on_removeURLButton_clicked() {
	if(!isLoading) {
		config->webhookURLList.erase(std::remove(config->webhookURLList.begin(), config->webhookURLList.end(), ui->webhookEnterGame->text().toUtf8().constData()), config->webhookURLList.end());

		std::string searchName = ui->webhookEnterGame->text().toUtf8().constData();
		for (int i = ui->webhookURLList->count()-1; i >= 0; i--) {
			QListWidgetItem *item = ui->webhookURLList->item(i);
			if (item->text() == QString::fromStdString(searchName)) {
				int row = ui->webhookURLList->row(item);
				delete ui->webhookURLList->takeItem(row);
			}
		}

		if (ui->webhookURLList->count() == 0) {
			ui->webhookEnterGame->setText("");
		}
	}
}

void SettingsDialog::on_webhookURLList_itemSelectionChanged() {
	if(!isLoading) {
		if (ui->webhookURLList->currentItem() != NULL && ui->webhookURLList->currentItem()->text().isNull() == false) {
			ui->webhookEnterGame->setText(ui->webhookURLList->currentItem()->text());
		}
	}
}
