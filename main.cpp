#define ENABLE_UI //comment or uncomment this section to turn on or off ui //TODO choose your ui mode

#include "core/entities/Database/entities/DbController.hpp"
#include "core/entities/Ini_parser/entities/Ini_parser.hpp"
#include "core/entities/Indexer/entities/Indexer.hpp"
#include <future>

#ifdef ENABLE_UI

#include "core/entities/UI/entities/Search_hit_model.hpp"
#include "core/entities/UI/Interface.hpp"
#include <QMessageBox>
#include <QListWidget>

#endif

import Libio;

int main(int argc, char *argv[]) {
#ifdef ENABLE_UI
    QApplication a(argc, argv);
#endif

    std::unique_ptr<Ini_parser> ini_parser;
    std::unique_ptr<DB_controller> db_controller;
    std::unique_ptr<Indexer> indexer;

    DB_controller_builder builder;
    ini_parser = std::make_unique<Ini_parser>(libio::file::get_current_dir_name("settings.ini"));

#ifdef ENABLE_UI
    if (ini_parser->get_section_count() == 0) {
        QMessageBox(QMessageBox::Icon::Critical, "Error",
                    "Ini parser is not initialized correctly, maybe path to ini file is corrupted").exec();
        return EXIT_FAILURE; //return 1 in case of error
    }
#endif

    builder.set_db_name(ini_parser->get_value<std::string>("Database.bd_name"))
            .set_host(ini_parser->get_value<std::string>("Database.host"))
            .set_user(ini_parser->get_value<std::string>("Database.username"))
            .set_password(ini_parser->get_value<std::string>("Database.password"))
            .set_port(ini_parser->get_value<std::string>("Database.port"));

    db_controller = std::make_unique<DB_controller>(builder.build());
    db_controller->drop_tables(); //drop existing tables
    indexer = std::make_unique<Indexer>(db_controller.get(), ini_parser->get_value<std::string>("Settings.extensions"));
    db_controller->init_tables(); //and then init them

#ifdef ENABLE_UI
#pragma message("Using graphical ui")
    auto model = new SearchHitModel();

    //UI block:
    QMainWindow main_win;
    Ui::MainWindow window;

    window.setupUi(&main_win);

    //Window entities:
    QPushButton *search_btn = window.search_btn;
    QTextEdit *search_txt_field = window.to_search;
    QListView *txt_view = window.results;
    txt_view->setModel(model);

    QPushButton::connect(search_btn, &QPushButton::clicked, [search_txt_field, &model, &db_controller] {
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
        if (results_to_view.empty()) {
            QMessageBox(QMessageBox::Icon::Information, "Empty", "No results").exec();
            return;
        }
        model->setHits(results_to_view);
    });

    auto task = std::async(std::launch::async, [&ini_parser, &indexer]() {
        indexer->process_dir(ini_parser->get_value<std::string>("Settings.start_path"));
    });
    task.get();

    main_win.show();

    return QApplication::exec();
}

#else
#pragma message("Using console interface")
    auto task = std::async(std::launch::async, [&indexer, &ini_parser]() {
        indexer->process_dir(ini_parser->get_value<std::string>("Settings.start_path"));
    });
    task.get();

    std::string user_input;
    libio::output::println("Enter word to search");
    libio::output::print(">> ");
    libio::input::user_input(user_input);
    auto split_search_query = libio::string::split(user_input, ' ');
    auto results_to_view = db_controller->find_words(split_search_query);
    if (!results_to_view.empty()) {
        for (const auto &res: results_to_view) {
            libio::output::println("Found:");
            libio::output::println("\tFile path: " + res.file_path);
            libio::output::println("\tFile name: " + res.file_name);
            libio::output::println("\tTotal word score: " + std::to_string(res.total_score));
        }
    } else {
        libio::output::println("No results found");
    }
}

#endif
