#include "Indexer.hpp"
#include <algorithm>
#include <QMessageBox>

void Indexer::collect_files(const filesys::path &path, std::vector<std::pair<filesys::path, std::string>> &files) {
    try {
        for (const auto &entry: filesys::directory_iterator(path)) {
            const auto &dir_path = entry.path();

            if (filesys::is_directory(dir_path)) {
                collect_files(dir_path, files);
            } else if (filesys::is_regular_file(dir_path)) {
                const auto ext = dir_path.extension().string();
                if (std::ranges::any_of(valid_pattern, [&ext](const std::string &pattern) -> bool {
                    return ext == "." + pattern; //some kind of trick to compare extension and pattern
                })) {
                    files.emplace_back(dir_path, dir_path.filename().string());
                }
            }
        }
    } catch (...) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to Collect files").exec();
        return;
    }
}

void Indexer::process_file(const filesys::path &file_path, const std::string &file_name) {
    try {
        std::string content = libio::file::read_file2(file_path.string());

        //delete punctuation
        std::ranges::transform(content, content.begin(), [](char c) {
            return std::ispunct(c) || c == '\n' ? ' ' : c;
        });

        //to lower case
        content = libio::string::change_string_register(content, true);

        //split
        auto words = libio::string::split(content, ' ');

        //delete empty
        std::erase_if(words, [](const std::string &w) { return w.empty(); });

        auto freq = count_freq(words);

        //database things
        std::lock_guard<std::mutex> lock(db_mutex);
        controller->add_document(freq, file_path.string(), file_name);
    } catch (...) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to Process files").exec();
        return;
    }
}

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

    std::vector<std::pair<filesys::path, std::string>> files;
    collect_files(dir, files);

    std::for_each(files.begin(), files.end(),
                  [this](auto &file_info) {
                      process_file(file_info.first, file_info.second);
                  }
    );
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
