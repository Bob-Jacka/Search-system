#ifndef INIPARSER_HPP
#define INIPARSER_HPP

#include <string>
#include <vector>
#include <map>

import Libio;

constexpr auto start_section = '[';
constexpr auto end_section = ']';
constexpr auto value_splitter = '=';
constexpr auto comment_start = ';';

template<typename T>
concept concept_ini_guard = requires(T)
{
    std::is_same_v<T, std::string>;
};

/**
 * Main class of the ini parser.
 * Parse ini file.
 */
class Ini_parser {
    using Sections_t = std::map<std::string, std::map<std::string, std::string> >;

    Sections_t m_sections;

    static Sections_t parse_ini_from_vector(const std::vector<std::string> &);

public:
    explicit Ini_parser(const std::string &); //from file

    explicit Ini_parser(const std::vector<std::string> &); //from vector with lines

    Ini_parser() = delete;

    Ini_parser(const Ini_parser &) = delete;

    ~Ini_parser() = default;

    template<typename T>
    requires concept_ini_guard<T>
    T get_value(const std::string &) const;

    template<typename T>
    requires concept_ini_guard<T>
    T get_value_or(const std::string &, const T &) const;

    [[nodiscard]] Sections_t get_sections() const;

    [[nodiscard]] int get_section_count() const;
};

/**
 * Get value from section object
 * @tparam T generic param
 * @param section_param string value, ex - SectionName.sectionValue
 * @return generic parameter value from section
 */
template<typename T>
requires concept_ini_guard<T>
T Ini_parser::get_value(const std::string &section_param) const {
    if (section_param.contains(".")) {
        const auto split_line = libio::string::split(section_param,
                                                     '.'); //0 - name of the section and 1 - name of the value
        for (const auto &section: m_sections) {
            //key is section name, value is section
            if (split_line[0] == section.first) {
                try {
                    if (section.second.contains(split_line[1])) {
                        auto to_return = libio::convert::convert_to_t<T>(section.second.at(split_line[1]));
                        return to_return;
                    }
                } catch (const std::exception &e) {
//                    printf("Exception on line: %s - %s in file %s", __LINE__,
//                           "Error retrieving value: " + std::string(e.what()), __FILE_NAME__);
                }
            }
        }
        //mistype hint
        for (const auto &[fst, snd]: m_sections.at(split_line[0])) {
            libio::output::println("  " + fst + "=" + snd);
        }
        libio::output::println();

        printf("Exception on line: %s - %s in file %s", __LINE__,
               "No value, but maybe you mistyped, upper you will see some values from section", __FILE_NAME__);
    }
}

/**
 * Return value in section or return default value if not found.
 * @tparam T generic parameter
 * @param section_param string value (name of the section), ex - SectionName.sectionValue
 * @param default_value default value that will be given if no value found
 * @return section value or default if not found
 */
template<typename T>
requires concept_ini_guard<T>
T Ini_parser::get_value_or(const std::string &section_param, const T &default_value) const {
    if (section_param.contains(".")) {
        const auto split_line = libio::string::split(section_param,
                                                     "."); //0 - name of the section and 1 - name of the value
        for (const auto &section: m_sections) {
            //key is section name, value is section
            if (split_line[0] == section.first) {
                try {
                    if (section.second.contains(split_line[1])) {
                        auto to_return = libio::convert::convert_to_t<T>(section.second.at(split_line[1]));
                        return to_return;
                    }
                } catch (const std::exception &e) {
                    printf("Exception on line: %s - %s in file %s", __LINE__,
                           "Error retrieving value: " + std::string(e.what()), __FILE_NAME__);
                }
            }
        }
        return default_value;
    }
    return default_value;
}

#endif
