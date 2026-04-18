#include "Indexer.hpp"
#include "../../Database/entities/DbController.hpp"

/**
 * Entry point to indexer program
 * @param start_point starting point to execute indexer co program
 */
void Indexer::process_dir(const std::string &start_point) {
    const filesys::path dir(start_point);

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
void Indexer::index_dir(const filesys::path &path) {
    for (const auto &entry: filesys::directory_iterator(path)) {
        const auto &dir_path = entry.path();
        const auto file_name = dir_path.filename();
        if (filesys::is_directory(path)) {
            index_dir(dir_path);
        } else if (filesys::is_regular_file(dir_path)) {
            auto file_txt = libio::file::read_file(dir_path.string());

            //clear results:
            std::for_each(std::execution::par_unseq, file_txt.begin(), file_txt.end(),
                          libio::string::delete_whitespaces);

            //to lower case:
            std::transform(std::execution::par, file_txt.begin(), file_txt.end(), result.begin(),
                           [](const auto &to_low_str) {
                               return libio::string::change_string_register(to_low_str, true);
                           });

            auto freq = count_freq(result);
            controller->add_document(freq, dir_path, file_name);
        }
    }
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
