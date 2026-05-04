#include "Indexer.hpp"
#include "../../Database/entities/DbController.hpp"

void Indexer::collect_files(const std::filesystem::path &path,
                            std::vector<std::pair<std::filesystem::path, std::string>> &files) {
    try {
        for (const auto &entry: std::filesystem::directory_iterator(path)) {
            const auto &dir_path = entry.path();

            if (std::filesystem::is_directory(dir_path)) {
                collect_files(dir_path, files);
            } else if (std::filesystem::is_regular_file(dir_path)) {
                const auto ext = dir_path.extension().string();
                if (std::ranges::any_of(valid_pattern, [&ext](const std::string &pattern) -> bool {
                    return ext == "." + pattern; //some kind of trick to compare extension and pattern
                })) {
                    files.emplace_back(dir_path, dir_path.filename().string());
                }
            }
        }
    } catch (...) {
        return;
    }
}

/**
 * Entry point to indexer program
 * @param start_point starting point to execute indexer co program
 */
void Indexer::process_dir(const std::string &start_point, DB_controller *db_controller) {
    const std::filesystem::path dir(start_point);

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        libio::output::println("[Error] - path is not exist or not a directory");
        return;
    }

    std::vector<std::pair<std::filesystem::path, std::string>> files;
    collect_files(dir, files);

    std::for_each(files.begin(), files.end(),
                  [&db_controller](auto &file_info) {
                      auto file_path = file_info.first;
                      auto file_name = file_info.second;
                      std::mutex db_mutex;
                      try {
                          std::string content = libio::file::read_file2(file_path.string());

                          //delete punctuation
                          std::ranges::transform(content, content.begin(), [](unsigned char c) -> unsigned char {
                              return std::ispunct(c) || c == '\n' ? ' ' : c;
                          });

                          //to lower case
                          content = libio::string::change_string_register(content, true);

                          //split
                          auto words = libio::string::split(content, ' ');

                          //delete empty
                          std::erase_if(words, [](const std::string &w) { return w.empty(); });

                          auto freq = Indexer::count_freq(words);

                          //database things
                          std::lock_guard<std::mutex> lock(db_mutex);
                          db_controller->add_document(freq, file_path.string(), file_name);
                      } catch (...) {
                          return;
                      }
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

Indexer::Indexer(const std::string &pattern) {
    valid_pattern = libio::string::split(pattern, ',');
}

Indexer &Indexer::operator=(const Indexer &other) {
    if (this == &other) {
        return *this;
    }

    valid_pattern = other.valid_pattern;
    return *this;
}
