/********************************************************************************
** Form generated from reading UI file 'auto-scene-switcher.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTO_2D_SCENE_2D_SWITCHER_H
#define UI_AUTO_2D_SCENE_2D_SWITCHER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SceneSwitcher
{
public:
    QPushButton *toggleStartButton;
    QComboBox *inGameScene;
    QComboBox *outGameScene;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_4;
    QLineEdit *ipAddr;
    QLabel *label_5;
    QLineEdit *lineEdit_2;
    QLabel *label_3;

    void setupUi(QDialog *SceneSwitcher)
    {
        if (SceneSwitcher->objectName().isEmpty())
            SceneSwitcher->setObjectName(QStringLiteral("SceneSwitcher"));
        SceneSwitcher->resize(275, 252);
        toggleStartButton = new QPushButton(SceneSwitcher);
        toggleStartButton->setObjectName(QStringLiteral("toggleStartButton"));
        toggleStartButton->setGeometry(QRect(80, 70, 111, 31));
        inGameScene = new QComboBox(SceneSwitcher);
        inGameScene->setObjectName(QStringLiteral("inGameScene"));
        inGameScene->setGeometry(QRect(120, 10, 141, 22));
        outGameScene = new QComboBox(SceneSwitcher);
        outGameScene->setObjectName(QStringLiteral("outGameScene"));
        outGameScene->setGeometry(QRect(120, 40, 141, 22));
        label = new QLabel(SceneSwitcher);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 10, 81, 21));
        label_2 = new QLabel(SceneSwitcher);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 101, 20));
        label_4 = new QLabel(SceneSwitcher);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 140, 101, 16));
        ipAddr = new QLineEdit(SceneSwitcher);
        ipAddr->setObjectName(QStringLiteral("ipAddr"));
        ipAddr->setGeometry(QRect(120, 140, 141, 20));
        label_5 = new QLabel(SceneSwitcher);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 170, 261, 61));
        label_5->setWordWrap(true);
        lineEdit_2 = new QLineEdit(SceneSwitcher);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(160, 220, 81, 20));
        lineEdit_2->setReadOnly(true);
        label_3 = new QLabel(SceneSwitcher);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 120, 221, 16));

        retranslateUi(SceneSwitcher);

        QMetaObject::connectSlotsByName(SceneSwitcher);
    } // setupUi

    void retranslateUi(QDialog *SceneSwitcher)
    {
        SceneSwitcher->setWindowTitle(QApplication::translate("SceneSwitcher", "SC2Switcher", Q_NULLPTR));
        toggleStartButton->setText(QString());
        label->setText(QApplication::translate("SceneSwitcher", "In Game Scene:", Q_NULLPTR));
        label_2->setText(QApplication::translate("SceneSwitcher", "Out of Game Scene:", Q_NULLPTR));
        label_4->setText(QApplication::translate("SceneSwitcher", "SC2 PC IP Address:", Q_NULLPTR));
        label_5->setText(QApplication::translate("SceneSwitcher", "<html><head/><body><p>On your SC2 PC, open the Battle.net launcher, click Options, Game Settings, and under SC2, check 'Additional Command Line Arguments', and paste the following into the textbox:</p></body></html>", Q_NULLPTR));
        lineEdit_2->setText(QApplication::translate("SceneSwitcher", "-clientapi 6119", Q_NULLPTR));
        label_3->setText(QApplication::translate("SceneSwitcher", "If SC2 is running on a different PC:  (optional)", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SceneSwitcher: public Ui_SceneSwitcher {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTO_2D_SCENE_2D_SWITCHER_H
