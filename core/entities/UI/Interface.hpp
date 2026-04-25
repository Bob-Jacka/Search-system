/********************************************************************************
** Form generated from reading UI file 'search_systemLsjoZm.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SEARCH_SYSTEMLSJOZM_H
#define SEARCH_SYSTEMLSJOZM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE

class Ui_MainWindow : public QMainWindow {
Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                                   "Подтверждение выхода",
                                                                   "Вы действительно хотите закрыть приложение?",
                                                                   QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::No);

        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            clearenv();
            event->accept();
        }
    }

public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *search_bar;
    QHBoxLayout *action_bar;
    QPushButton *search_btn;
    QLabel *select_txt;
    QTextEdit *to_search;
    QVBoxLayout *search_results;
    QLabel *label_3;
    QListView *results;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    Ui_MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
    }

    ~Ui_MainWindow() = default;

    void setupUi() {
        if (this->objectName().isEmpty()) {
            this->setObjectName("Search system");
        }
        this->resize(800, 600);
        this->setFixedSize(800, 600); //do not resize window
        centralwidget = new QWidget(this);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget_3 = new QWidget(centralwidget);
        verticalLayoutWidget_3->setObjectName("verticalLayoutWidget_3");
        verticalLayoutWidget_3->setGeometry(QRect(40, 20, 731, 511));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_3->setSpacing(15);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        search_bar = new QVBoxLayout();
        search_bar->setSpacing(0);
        search_bar->setObjectName("search_bar");
        search_bar->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        search_bar->setContentsMargins(-1, -1, -1, 0);
        action_bar = new QHBoxLayout();
        action_bar->setObjectName("action_bar");
        action_bar->setSpacing(20);
        search_btn = new QPushButton(verticalLayoutWidget_3);
        search_btn->setObjectName("search_btn");

        action_bar->addWidget(search_btn);

        select_txt = new QLabel(verticalLayoutWidget_3);
        select_txt->setObjectName("select_txt");

        action_bar->addWidget(select_txt);

        search_bar->addLayout(action_bar);

        to_search = new QTextEdit(verticalLayoutWidget_3);
        to_search->setObjectName("to_search");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(to_search->sizePolicy().hasHeightForWidth());
        to_search->setSizePolicy(sizePolicy);
        to_search->setMaximumSize(QSize(16777215, 30));

        search_bar->addWidget(to_search);

        verticalLayout_3->addLayout(search_bar);

        search_results = new QVBoxLayout();
        search_results->setObjectName("search_results");
        label_3 = new QLabel(verticalLayoutWidget_3);
        label_3->setObjectName("label_3");

        search_results->addWidget(label_3);

        results = new QListView(verticalLayoutWidget_3);
        results->setObjectName("results");

        search_results->addWidget(results);

        verticalLayout_3->addLayout(search_results);

        this->setCentralWidget(centralwidget);
        menubar = new QMenuBar(this);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 23));
        this->setMenuBar(menubar);
        statusbar = new QStatusBar(this);
        statusbar->setObjectName("statusbar");
        this->setStatusBar(statusbar);

        retranslateUi();

        QMetaObject::connectSlotsByName(this);
    } // setupUi

    void retranslateUi() {
        this->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        search_btn->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        select_txt->setText(QCoreApplication::translate("MainWindow", "Enter text to search above", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Search results:", nullptr));
    } // retranslateUi
};

QT_END_NAMESPACE

#endif // SEARCH_SYSTEMLSJOZM_H
