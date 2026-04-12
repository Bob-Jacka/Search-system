#include "Indexer.hpp"

void Indexer::process_dir(const std::string &start_point) {
    filesys::path dir(start_point);

    if (!filesys::exists(dir) || !std::filesystem::is_directory(dir)) {
        libio::output::println("[Error] - path is not exist");
        return;
    }
    index_dir(dir);
}

/**
 * Index dir with files and proceed their files
 * @param path path to directory
 * @return vector with processed strings
 */
std::vector<std::string> Indexer::index_dir(const filesys::path &path) {
    for (const auto &entry: filesys::directory_iterator(path)) {
        if (filesys::is_directory(entry.path())) {
            index_dir(entry.path());
        } else if (filesys::is_regular_file(entry.path())) {
            auto file_txt = libio::file::read_file(entry.path().string());
            std::for_each(std::execution::par_unseq, file_txt.begin(), file_txt.end(),
                          libio::string::delete_whitespaces);
            std::transform(std::execution::par, file_txt.begin(), file_txt.end(), result.begin(),
                           [](const auto &to_low_str) {
                               return libio::string::change_string_register(to_low_str, true);
                           });
        }
    }
    return result;
}

std::unordered_map<std::string, int> Indexer::count_freq(const std::vector<std::string> &words) {
    std::unordered_map<std::string, int> freq_res;
    for (auto &word: words) {
        freq_res[word]++;
    }
    return freq_res;
}

Indexer::Indexer(DB_controller *db, const std::string &pattern) {
    controller = db;
    valid_patter = pattern;
}
