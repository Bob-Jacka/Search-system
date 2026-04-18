#include "core/entities/UI/Interface.hpp"
#include "core/entities/Database/entities/DbController.hpp"
#include "core/entities/Ini_parser/entities/Ini_parser.hpp"
#include "core/entities/Indexer/entities/Indexer.hpp"
#include <QMessageBox>
#include <future>
#include <QListWidget>
#include <QStandardItemModel>
#include <QStringListModel>

import Libio;

std::unique_ptr<Ini_parser> ini_parser;
std::unique_ptr<DB_controller> db_controller;
std::unique_ptr<Indexer> indexer;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DB_controller_builder builder;
    builder.set_db_name(ini_parser->get_value<std::string>("bd_name"))
            .set_host(ini_parser->get_value<std::string>("host"))
            .set_user(ini_parser->get_value<std::string>("username"))
            .set_password(ini_parser->get_value<std::string>("password"))
            .set_port(ini_parser->get_value<std::string>("port"));

    ini_parser = std::make_unique<Ini_parser>(libio::file::get_current_dir_name("settings.ini"));
    db_controller = std::make_unique<DB_controller>(builder.build());
    db_controller->init_tables();

    //UI block:
    const auto main_window = std::make_unique<Ui::MainWindow>();
    auto main_container_win = std::make_unique<QMainWindow>();

    main_window->setupUi(main_container_win.get());

    //Window entities:
    auto search_btn = main_container_win->findChild<QPushButton *>("search_btn");
    auto search_txt_field = main_container_win->findChild<QTextEdit *>("to_search");
    auto txt_view = main_container_win->findChild<QListView *>("results");
    auto model = std::make_unique<QStringListModel>();

    QPushButton::connect(search_btn, &QPushButton::clicked, [&search_txt_field, txt_view, &model] {
        auto txt = search_txt_field->toPlainText();
        auto split_search_query = txt.split(' '); //split search query by space symbol

        if (split_search_query.size() > 4) {
            QMessageBox(QMessageBox::Icon::Warning, "Warning", "Cannot search more than 4 words").exec();
            return;
        }
        if (txt.isEmpty()) {
            QMessageBox(QMessageBox::Icon::Warning, "Warning", "There is not text to search").exec();
            return;
        } else {

            //Get results from database
//            auto results_to_view = db_controller->find_words(split_search_query);

            auto output_model = std::make_unique<QStandardItemModel>(3, 2);
            output_model->setItem(0, 0, new QStandardItem("Tom"));
            output_model->setItem(1, 0, new QStandardItem("Bob"));
            output_model->setItem(2, 0, new QStandardItem("Sam"));

            txt_view->setModel(output_model.get());
            indexer->process_dir(search_txt_field->toPlainText().toStdString());
            txt_view->update();
        }
    });

    if (main_container_win) {
        db_controller->drop_tables(); //drop existing tables
        main_container_win->show();
    }

    return QApplication::exec();
}
