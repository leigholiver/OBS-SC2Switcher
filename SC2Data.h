#pragma once

#include <QObject>
#include <QTimer>
#include <vector>

#include "SC2State.h"

using std::vector;

class Observer;
class SC2Data : public QObject {
  Q_OBJECT

  private:
  	SC2State* state;
  	QTimer* timer;
	vector<Observer*> watchers;

  public:
    explicit SC2Data(QObject* parent = Q_NULLPTR);
    virtual ~SC2Data();
    static SC2Data* Instance;
    void attach(Observer* obs);

  	public slots:
  		void update();
};