#pragma once

#include <QDialog>
#include <memory>
#include <vector>
#include <string>

#include "ui_auto-scene-switcher.h"
struct obs_weak_source;
typedef struct obs_weak_source obs_weak_source_t;

class QCloseEvent;

class SceneSwitcher : public QDialog {
	Q_OBJECT

public:
	bool loading = true;

public:
	std::unique_ptr<Ui_SceneSwitcher> ui;
	SceneSwitcher(QWidget *parent);
	void closeEvent(QCloseEvent *event) override;

	public slots:
	void on_inGameScene_currentTextChanged(const QString &name);
	void on_outGameScene_currentTextChanged(const QString &name);
	void on_replayScene_currentTextChanged(const QString& text);
	void on_ipAddr_textChanged(const QString &name);
	void on_toggleStartButton_clicked();
};