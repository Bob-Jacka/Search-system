#ifndef SEARCH_SYSTEM_INDEXER_HPP
#define SEARCH_SYSTEM_INDEXER_HPP

#include <algorithm>
#include <execution>
#include <filesystem>
#include <string>

import Libio;

namespace filesys = std::filesystem;

class DB_controller;

class Ini_parser;

class Indexer {
    private:
        std::vector<std::string> result = std::vector<std::string>();

        DB_controller *controller;

        std::string valid_patter;

        std::vector<std::string> index_dir(const filesys::path &path);

        void save_results_2_db();

        void init_db_structure();

    public:
        Indexer() = delete;

        Indexer(DB_controller *, const std::string &pattern);

        ~Indexer() = default;

        static std::unordered_map<std::string, int> count_freq(const std::vector<std::string> &words);

        void process_dir(const std::string &start_point); ///main entry point to indexer
};

#endif
