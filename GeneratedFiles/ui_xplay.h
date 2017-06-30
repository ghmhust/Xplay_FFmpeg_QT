/********************************************************************************
** Form generated from reading UI file 'xplay.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "videowidget.h"
#include "xslider.h"

QT_BEGIN_NAMESPACE

class Ui_XplayClass
{
public:
    videoWidget *openGLWidget;
    QPushButton *playButton;
    QPushButton *openButton;
    QLabel *totaltime;
    QLabel *playtime;
    QLabel *fenge;
    Xslider *playslider;

    void setupUi(QWidget *XplayClass)
    {
        if (XplayClass->objectName().isEmpty())
            XplayClass->setObjectName(QStringLiteral("XplayClass"));
        XplayClass->resize(800, 600);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Xplay/video.ico"), QSize(), QIcon::Normal, QIcon::Off);
        XplayClass->setWindowIcon(icon);
        openGLWidget = new videoWidget(XplayClass);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 800, 600));
        playButton = new QPushButton(XplayClass);
        playButton->setObjectName(QStringLiteral("playButton"));
        playButton->setGeometry(QRect(110, 500, 101, 91));
        playButton->setStyleSheet(QLatin1String("QPushButton:!hover{border-image:url(:/Xplay/playbefore.png)}\n"
"QPushButton:hover{border-image:url(:/Xplay/playafter.png)}"));
        openButton = new QPushButton(XplayClass);
        openButton->setObjectName(QStringLiteral("openButton"));
        openButton->setGeometry(QRect(40, 520, 51, 61));
        openButton->setStyleSheet(QLatin1String("QPushButton:!hover{border-image:url(:/Xplay/openafter.png)}\n"
"QPushButton:hover{border-image:url(:/Xplay/openbefore.png)}"));
        totaltime = new QLabel(XplayClass);
        totaltime->setObjectName(QStringLiteral("totaltime"));
        totaltime->setGeometry(QRect(690, 530, 51, 21));
        totaltime->setStyleSheet(QLatin1String("color:rgb(255, 255, 255);\n"
"font: 75 11pt \"Arial\";"));
        playtime = new QLabel(XplayClass);
        playtime->setObjectName(QStringLiteral("playtime"));
        playtime->setGeometry(QRect(600, 530, 51, 21));
        playtime->setStyleSheet(QLatin1String("color:rgb(255, 255, 255);\n"
"font: 75 11pt \"Arial\";"));
        fenge = new QLabel(XplayClass);
        fenge->setObjectName(QStringLiteral("fenge"));
        fenge->setGeometry(QRect(670, 530, 16, 21));
        fenge->setStyleSheet(QLatin1String("color:rgb(255, 255, 255);\n"
"font: 75 11pt \"Arial\";"));
        playslider = new Xslider(XplayClass);
        playslider->setObjectName(QStringLiteral("playslider"));
        playslider->setGeometry(QRect(80, 480, 651, 22));
        playslider->setMaximum(999);
        playslider->setSingleStep(0);
        playslider->setPageStep(100);
        playslider->setOrientation(Qt::Horizontal);

        retranslateUi(XplayClass);
        QObject::connect(openButton, SIGNAL(clicked()), XplayClass, SLOT(open()));
        QObject::connect(playslider, SIGNAL(sliderPressed()), XplayClass, SLOT(sliderPress()));
        QObject::connect(playslider, SIGNAL(sliderReleased()), XplayClass, SLOT(sliderRelease()));
        QObject::connect(playButton, SIGNAL(clicked()), XplayClass, SLOT(play()));

        QMetaObject::connectSlotsByName(XplayClass);
    } // setupUi

    void retranslateUi(QWidget *XplayClass)
    {
        XplayClass->setWindowTitle(QApplication::translate("XplayClass", "Xplay", 0));
        playButton->setText(QString());
        openButton->setText(QString());
        totaltime->setText(QApplication::translate("XplayClass", "000:00", 0));
        playtime->setText(QApplication::translate("XplayClass", "000:00", 0));
        fenge->setText(QApplication::translate("XplayClass", "/", 0));
    } // retranslateUi

};

namespace Ui {
    class XplayClass: public Ui_XplayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
