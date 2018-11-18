#pragma once
#include <QDialog>
#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();
    void closeEvent(QCloseEvent *event) override;

    public slots:
    	void on_toggleStartButton_clicked();
		void on_ipAddr_textChanged(const QString &name);
		void on_addUsernameButton_clicked();
		void on_removeUsernameButton_clicked();

		void on_userNames_itemSelectionChanged();
		void on_recentUsernames_itemSelectionChanged();
		void on_textSourceName_textChanged(const QString& text);
		void on_textTemplate_textChanged();

    	void on_inGameScene_currentTextChanged(const QString &name);
		void on_outGameScene_currentTextChanged(const QString &name);
		void on_replayScene_currentTextChanged(const QString& text);
		void on_obsScene_currentTextChanged(const QString &name);
		void on_replaysScene_currentTextChanged(const QString &name);
		void on_campaignScene_currentTextChanged(const QString &name);
		void on_homeScene_currentTextChanged(const QString &name);
		void on_collectionScene_currentTextChanged(const QString &name);
		void on_coopScene_currentTextChanged(const QString &name);
		void on_customScene_currentTextChanged(const QString &name);
		void on_lobbyScene_currentTextChanged(const QString &name);
		void on_scoreScreenScene_currentTextChanged(const QString &name);
		void on_profileScene_currentTextChanged(const QString &name);
		void on_versusScene_currentTextChanged(const QString &name);
		void on_switcherEnabled_stateChanged(int state);
		void on_scoresEnabled_stateChanged(int state);
		void on_popupsEnabled_stateChanged(int state);
		void on_webhookEnabled_stateChanged(int state);
		void on_clearSettings_stateChanged(int state);
		void on_logging_stateChanged(int state);
		void on_switchOnLoad_stateChanged(int state);

		void on_tWinPlus_clicked();
		void on_tWinMinus_clicked();
		void on_tLossPlus_clicked();
		void on_tLossMinus_clicked();
		void on_zWinPlus_clicked();
		void on_zWinMinus_clicked();
		void on_zLossPlus_clicked();
		void on_zLossMinus_clicked();
		void on_pWinPlus_clicked();
		void on_pWinMinus_clicked();
		void on_pLossPlus_clicked();
		void on_pLossMinus_clicked();

		void on_addURLButton_clicked();
		void on_removeURLButton_clicked();
		void on_webhookURLList_itemSelectionChanged();


private:
    Ui::SettingsDialog* ui;
    bool isLoading = false;
};