#pragma once

#include "Observer.h"
#include "SC2State.h"

class SceneSwitcher : public Observer {
  public:
    explicit SceneSwitcher(SC2Data *sc2);

  	public slots:
  		void notify(SC2State*& previous, SC2State*& current);
  		std::string getName();
};