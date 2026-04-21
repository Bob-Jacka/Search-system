#ifndef SEARCH_SYSTEM_INDEXER_HPP
#define SEARCH_SYSTEM_INDEXER_HPP

#include <filesystem>
#include <string>
#include <unordered_map>
#include "../../Database/entities/DbController.hpp"

import Libio;

namespace filesys = std::filesystem;

class Indexer {
private:
    std::vector<std::string> result = std::vector<std::string>();

    DB_controller *controller;

    std::vector<std::string> valid_patter;

    void index_dir(const filesys::path &path);

public:
    Indexer() = delete;

    Indexer(DB_controller *, const std::string &pattern);

    ~Indexer() = default;

    static std::unordered_map<std::string, int> count_freq(const std::vector<std::string> &words);

    void process_dir(const std::string &start_point); ///main entry point to indexer
};

#endif
