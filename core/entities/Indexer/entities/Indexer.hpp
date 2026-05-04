#ifndef SEARCH_SYSTEM_INDEXER_HPP
#define SEARCH_SYSTEM_INDEXER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <algorithm>

#include <filesystem>

import Libio;

struct DB_controller;

class Indexer {
private:
    std::vector<std::string> valid_pattern;

    void
    collect_files(const std::filesystem::path &path, std::vector<std::pair<std::filesystem::path, std::string>> &files);

public:

    Indexer() = delete;

    Indexer(const Indexer &) = delete;

    Indexer& operator=(const Indexer& other);

    explicit Indexer(const std::string &pattern);

    ~Indexer() = default;

    void process_dir(const std::string &start_point, DB_controller *db_controller);

    static std::unordered_map<std::string, int> count_freq(const std::vector<std::string> &words);
};

#endif
