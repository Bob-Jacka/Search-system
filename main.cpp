#include "core/entities/UI/Interface.hpp"
#include "core/entities/Database/entities/DbController.hpp"
#include "core/entities/Ini_parser/entities/Ini_parser.hpp"
#include "core/entities/Indexer/entities/Indexer.hpp"
#include "core/entities/Model/entities/Search_hit_model.hpp"
#include <QMessageBox>
#include <future>
#include <QListWidget>

import Libio;

std::unique_ptr<Ini_parser> ini_parser;
std::unique_ptr<DB_controller> db_controller;
std::unique_ptr<Indexer> indexer;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DB_controller_builder builder;
    ini_parser = std::make_unique<Ini_parser>(libio::file::get_current_dir_name("settings.ini"));

    if (ini_parser->get_section_count() == 0) {
        QMessageBox(QMessageBox::Icon::Critical, "Error",
                    "Ini parser is not initialized correctly, maybe path to ini file is corrupted").exec();
        return 1;
    }

    builder.set_db_name(ini_parser->get_value<std::string>("Database.bd_name"))
            .set_host(ini_parser->get_value<std::string>("Database.host"))
            .set_user(ini_parser->get_value<std::string>("Database.username"))
            .set_password(ini_parser->get_value<std::string>("Database.password"))
            .set_port(ini_parser->get_value<std::string>("Database.port"));

    db_controller = std::make_unique<DB_controller>(builder.build());
    db_controller->drop_tables(); //drop existing tables
    indexer = std::make_unique<Indexer>(db_controller.get(), ini_parser->get_value<std::string>("Settings.extensions"));
    db_controller->init_tables(); //and then init them

    //UI block:
    auto *main_window = new Ui_MainWindow();

    main_window->setupUi();
    main_window->setAttribute(Qt::WA_DeleteOnClose);

    //Window entities:
    auto search_btn = main_window->findChild<QPushButton *>("search_btn");
    auto search_txt_field = main_window->findChild<QTextEdit *>("to_search");
    auto txt_view = main_window->findChild<QListView *>("results");
    auto *model = new SearchHitModel(main_window);
    txt_view->setModel(model);

    QPushButton::connect(search_btn, &QPushButton::clicked, [search_txt_field, model] {
        auto txt = search_txt_field->toPlainText();
        auto split_search_query = txt.split(' ');

        if (split_search_query.size() > 4) {
            QMessageBox(QMessageBox::Icon::Warning, "Warning", "Cannot search more than 4 words").exec();
            return;
        }

        if (txt.isEmpty()) {
            QMessageBox(QMessageBox::Icon::Warning, "Warning", "There is no text to search").exec();
            return;
        }

        auto results_to_view = db_controller->find_words(split_search_query);
        model->setHits(results_to_view);
    });

    auto task = std::async(std::launch::async, []() {
        indexer->process_dir(ini_parser->get_value<std::string>("Settings.start_path"));
    });
    task.get();
    main_window->show();

    return QApplication::exec();
}

#include "main.moc"
