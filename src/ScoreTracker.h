#pragma once

#include <QTimer>
#include "Observer.h"
#include "SC2State.h"

class ScoreTracker : public Observer {
  private:
  	QTimer* timer;
  public:
    explicit ScoreTracker(SC2Data *sc2);
	static ScoreTracker* Current();
	~ScoreTracker();
    std::map <
		std::string,
		std::map<std::string, size_t>
	> scores;
    std::string getScoreString();
	void recordScore(std::string race, std::string result);
	void handleButton(std::string race, std::string result, std::string name);
	void addRandomConfirmMessage(std::string result);
	void addConfirmMessage(player* playerA, player* playerB);

  	public slots:
  		void notify(SC2State*& previous, SC2State*& current);
    	void updateText();
    	std::string getName();
};