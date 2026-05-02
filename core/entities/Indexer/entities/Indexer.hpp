#ifndef SEARCH_SYSTEM_INDEXER_HPP
#define SEARCH_SYSTEM_INDEXER_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include "../../Database/entities/DbController.hpp"

import Libio;

class Indexer {
private:
    DB_controller *controller;
    std::vector<std::string> valid_pattern;
    std::mutex db_mutex;

    void collect_files(const std::filesystem::path &path, std::vector<std::pair<std::filesystem::path, std::string>> &files);

    void process_file(const std::filesystem::path &file_path, const std::string &file_name);

    static std::unordered_map<std::string, int> count_freq(const std::vector<std::string> &words);

public:

    Indexer() = delete;

    Indexer(DB_controller *, const std::string &pattern);

    ~Indexer() = default;

    void process_dir(const std::string &start_point);
};

#endif
