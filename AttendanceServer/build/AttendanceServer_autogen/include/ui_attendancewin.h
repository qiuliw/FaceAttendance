/********************************************************************************
** Form generated from reading UI file 'attendancewin.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ATTENDANCEWIN_H
#define UI_ATTENDANCEWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AttendanceWin
{
public:
    QWidget *centralwidget;
    QLabel *pictureLabel;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *AttendanceWin)
    {
        if (AttendanceWin->objectName().isEmpty())
            AttendanceWin->setObjectName("AttendanceWin");
        AttendanceWin->resize(480, 300);
        centralwidget = new QWidget(AttendanceWin);
        centralwidget->setObjectName("centralwidget");
        pictureLabel = new QLabel(centralwidget);
        pictureLabel->setObjectName("pictureLabel");
        pictureLabel->setGeometry(QRect(0, 0, 300, 300));
        pictureLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(108, 255, 133);"));
        AttendanceWin->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(AttendanceWin);
        statusbar->setObjectName("statusbar");
        AttendanceWin->setStatusBar(statusbar);

        retranslateUi(AttendanceWin);

        QMetaObject::connectSlotsByName(AttendanceWin);
    } // setupUi

    void retranslateUi(QMainWindow *AttendanceWin)
    {
        AttendanceWin->setWindowTitle(QCoreApplication::translate("AttendanceWin", "AttendanceWin", nullptr));
        pictureLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AttendanceWin: public Ui_AttendanceWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ATTENDANCEWIN_H
