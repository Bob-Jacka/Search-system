#ifndef SEARCH_SYSTEM_SEARCHHIT_HPP
#define SEARCH_SYSTEM_SEARCHHIT_HPP

#include <string>

struct SearchHit {
    std::string file_name;
    std::string file_path;
    int total_score;
};

#endif
