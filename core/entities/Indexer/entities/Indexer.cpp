#include "Indexer.hpp"
#include <algorithm>
#include <QMessageBox>

/**
 * Entry point to indexer program
 * @param start_point starting point to execute indexer co program
 */
void Indexer::process_dir(const std::string &start_point) {
    const filesys::path dir(start_point);

    if (!filesys::exists(dir) || !std::filesystem::is_directory(dir)) {
        libio::output::println("[Error] - path is not exist or not a directory");
        return;
    }
    index_dir(dir);
}

/**
 * Index dir with files and proceed their files
 * @param path path to directory
 * @return vector with processed strings
 */
void Indexer::index_dir(const filesys::path &path) noexcept {
    try {
        for (const auto &entry: filesys::directory_iterator(path)) {
            const auto &dir_path = entry.path();
            const auto file_name = dir_path.filename();
            const auto ext = dir_path.extension().string();

            if (filesys::is_directory(dir_path)) {
                index_dir(dir_path);
            } else if (filesys::is_regular_file(dir_path)) {
                if (std::ranges::any_of(valid_pattern, [&ext](const std::string &pattern) -> bool {
                    return ext == "." + pattern; //some kind of trick, add dot to compare with extension
                })) {
                    std::string content = libio::file::read_file2(dir_path.string());

                    //delete punctuation
                    std::ranges::transform(content, content.begin(), [](char c) {
                        return std::ispunct(c) || c == '\n' ? ' ' : c;
                    });

                    //to lower case
                    content = libio::string::change_string_register(content, true);

                    //split
                    auto words = libio::string::split(content, ' ');

                    //Delete empty
                    std::erase_if(words, [](const std::string &w) { return w.empty(); });

                    auto freq = count_freq(words);
                    controller->add_document(freq, dir_path.string(), file_name.string());
                }
            }
        }
    } catch (...) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to Index files").exec();
        return;
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
    valid_pattern = libio::string::split(pattern, ',');
}
