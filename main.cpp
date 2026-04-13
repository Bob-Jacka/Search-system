#include "core/entities/UI/Interface.hpp"
#include "core/entities/Database/entities/DbController.hpp"
#include "core/entities/Ini_parser/entities/Ini_parser.hpp"
#include "core/entities/Indexer/entities/Indexer.hpp"
#include <QMessageBox>
#include <future>

import Libio;

using String = std::string;

std::unique_ptr<Ini_parser>    ini_parser;
std::unique_ptr<DB_controller> db_controller;
std::unique_ptr<Indexer>       indexer;

int main(int argc, char *argv[]) {
    QApplication          a(argc, argv);
    DB_controller_builder builder;
    ini_parser    = std::make_unique<Ini_parser>(libio::file::get_current_dir_name("settings.ini"));
    db_controller = std::make_unique<DB_controller>(builder
                                                    .set_db_name(ini_parser->get_value<String>("bd_name"))
                                                    .set_host(ini_parser->get_value<String>("host"))
                                                    .set_user(ini_parser->get_value<String>("username"))
                                                    .set_password(ini_parser->get_value<String>("password"))
                                                    .set_port(ini_parser->get_value<String>("port"))
                                                    .build());

    //UI block:
    {
        const auto main_window        = std::make_unique<Ui::MainWindow>();
        auto       main_container_win = std::make_unique<QMainWindow>();

        main_window->setupUi(main_container_win.get());

        //Window entities:
        auto search_btn       = main_container_win->findChild<QPushButton *>("search_btn");
        auto search_txt_field = main_container_win->findChild<QTextEdit *>("to_search");
        auto txt_view         = main_container_win->findChild<QListView *>("results");

        QPushButton::connect(search_btn, &QPushButton::clicked, [&search_txt_field, txt_view] {
            if (search_txt_field->toPlainText().isEmpty()) {
                QMessageBox(QMessageBox::Icon::Warning, "Warning", "There is not text to search").exec();
            } else {
                db_controller; //TODO proceed search request
            }
        });

        if (main_container_win) {
            main_container_win->show();
        }
    }

    return QApplication::exec();
}
