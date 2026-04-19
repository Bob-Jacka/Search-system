#ifndef SEARCH_SYSTEM_SEARCHHIT_HPP
#define SEARCH_SYSTEM_SEARCHHIT_HPP

#include <string>

struct SearchHit {
    std::string file_name;
    std::string file_path;
    int total_score;

    SearchHit() = default;
    SearchHit(std::string name, std::string path, int score)
            : file_name(std::move(name)), file_path(std::move(path)), total_score(score) {}
    ~SearchHit() = default;
};

#endif
