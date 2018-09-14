#pragma once

#include <QObject>

#include "SC2Data.h"
#include "SC2State.h"


#include <obs-frontend-api.h>

class Observer : public QObject {
	Q_OBJECT
    SC2Data *sc2;

	public:
		Observer(SC2Data *scdata, QObject* parent = Q_NULLPTR)
			: QObject(parent) {
			sc2 = scdata;
			sc2->attach(this);
		}
		virtual void notify(SC2State*& previous, SC2State*& current) = 0;
};