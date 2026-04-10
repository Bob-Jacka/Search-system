#include "Ini_parser.hpp"

Interpreter_ns::Ini_parser::Ini_parser(const std::string &filename) {
    const std::vector<std::string> lines = libio::file::read_file(filename);
    this->sections = parse_ini_from_vector(lines);
}

Interpreter_ns::Ini_parser::Ini_parser(const std::vector<std::string> &lines) {
    this->sections = parse_ini_from_vector(lines);
}

Interpreter_ns::Ini_parser::Sections_t Interpreter_ns::Ini_parser::get_sections() const {
    return std::move(this->sections);
}

int Interpreter_ns::Ini_parser::get_section_count() const {
    return this->sections.size();
}

/**
 * Ugly adapter method for converting sections to console parameters.
 * @return array of chars
 */
char **Interpreter_ns::Ini_parser::convert_to_char_array(const int arg_count) const {
    int counter = 0;
    const auto to_return = libio::array::create_2d_array<char>(arg_count, 1);
    for (const auto &section: this->sections) {
        for (const auto &[fst, snd]: section.second) {
            std::string word = "";
            word += "--" + fst + "=" + snd;
            to_return[counter] = const_cast<char *>(word.c_str());
            ++counter;
        }
    }
    return to_return;
}

Interpreter_ns::Ini_parser::Sections_t Interpreter_ns::Ini_parser::parse_ini_from_vector(const std::vector<std::string> &lines) {
    std::map<std::string, std::map<std::string, std::string> > sections_ini;
    std::string current_section;

    for (const auto &line: lines) {
        std::string trimmed = libio::string::trim(line);

        if (trimmed.empty()) {
            continue;
        }

        if (trimmed[0] == comment_start) {
            continue;
        }

        if (trimmed[0] == start_section && trimmed.back() == end_section) {
            current_section = libio::string::trim(trimmed.substr(1, trimmed.size() - 2));
            continue;
        }

        size_t equal_pos = trimmed.find(value_splitter);
        if (equal_pos != std::string::npos) {
            std::string key = libio::string::trim(trimmed.substr(0, equal_pos));
            std::string value_with_comment = libio::string::trim(trimmed.substr(equal_pos + 1));

            const size_t semicolon_pos = value_with_comment.find(comment_start);
            const std::string value = (semicolon_pos != std::string::npos)
                                          ? libio::string::trim(value_with_comment.substr(0, semicolon_pos))
                                          : value_with_comment;

            if (!current_section.empty()) {
                sections_ini[current_section][key] = value;
            }
        }
    }

    return sections_ini;
}
