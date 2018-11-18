#pragma once
#include "Constants.h"
#include "APIState.h"

class SC2State : public QObject {
	Q_OBJECT

	public:
		explicit SC2State(QObject* parent = Q_NULLPTR);
		virtual ~SC2State();
		int appState = 0;
		int gameState = 0;
		int menuState = 0;
		APIState fullState;
		void fromJSONString(std::string uiResponse, std::string gameResponse);
		void fillState();
};