#include "core/entities/Database/entities/DbController.hpp"
#include "core/entities/Indexer/entities/Indexer.hpp"
#include "core/entities/Ini_parser/entities/Ini_parser.hpp"
#include "core/entities/UI/entities/SearchHit.hpp"
#include <future>
#include <iostream>

import Libio;

int main(int argc, char *argv[]) {

    std::unique_ptr<Ini_parser> ini_parser;
    std::unique_ptr<DB_controller> db_controller;
    std::unique_ptr<Indexer> indexer;

    ini_parser = std::make_unique<Ini_parser>(libio::file::get_current_dir_name("settings.ini"));
    if (ini_parser->get_section_count() == 0) {
        libio::output::println("Failed to init ini parser, check your file");
        return 1;
    }

    db_controller = std::make_unique<DB_controller>(ini_parser->get_value<std::string>("Database.host"),
                                                    ini_parser->get_value<std::string>("Database.port"),
                                                    ini_parser->get_value<std::string>("Database.bd_name"),
                                                    ini_parser->get_value<std::string>("Database.username"),
                                                    ini_parser->get_value<std::string>("Database.password"));
    db_controller->drop_tables(); //drop existing tables

    indexer = std::make_unique<Indexer>(ini_parser->get_value<std::string>("Settings.extensions"));
    db_controller->init_tables(); //and then init them

    auto task = std::async(std::launch::async, [&indexer, &ini_parser, &db_controller]() {
        indexer->process_dir(ini_parser->get_value<std::string>("Settings.start_path"), db_controller.get());
    });
    task.get();

    std::string user_input;
    libio::output::println("Enter word(s) to search");
    libio::output::print(">> ");
    std::getline(std::cin, user_input);
    auto split_search_query = libio::string::split(user_input, ' ');
    if (split_search_query.size() <= 4) {
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
    } else {
        libio::output::println("Too many words to search");
    }
}
