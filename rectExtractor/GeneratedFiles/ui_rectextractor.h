/********************************************************************************
** Form generated from reading UI file 'rectextractor.ui'
**
** Created: Tue Dec 30 19:23:10 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECTEXTRACTOR_H
#define UI_RECTEXTRACTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rectExtractorClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *rectExtractorClass)
    {
        if (rectExtractorClass->objectName().isEmpty())
            rectExtractorClass->setObjectName(QString::fromUtf8("rectExtractorClass"));
        rectExtractorClass->resize(600, 400);
        centralWidget = new QWidget(rectExtractorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        rectExtractorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(rectExtractorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        rectExtractorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(rectExtractorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        rectExtractorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(rectExtractorClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        rectExtractorClass->setStatusBar(statusBar);

        retranslateUi(rectExtractorClass);

        QMetaObject::connectSlotsByName(rectExtractorClass);
    } // setupUi

    void retranslateUi(QMainWindow *rectExtractorClass)
    {
        rectExtractorClass->setWindowTitle(QApplication::translate("rectExtractorClass", "rectExtractor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class rectExtractorClass: public Ui_rectExtractorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECTEXTRACTOR_H
