#pragma once

#include "Observer.h"
#include "SC2State.h"
#include <curl/curl.h>

class Webhook : public Observer {
  private:
  	void sendRequest(SC2State*& game, std::string event);
  	std::string getQueryStringFromSC2State(SC2State*& game, std::string event, CURL* curl);
	std::string getJSONStringFromSC2State(SC2State*& game, std::string event);

  public:
    explicit Webhook(SC2Data *sc2);

  	public slots:
  		void notify(SC2State*& previous, SC2State*& current);

};