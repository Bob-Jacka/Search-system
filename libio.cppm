module;

/**
 Custom library for actions in Netology C++ course and later for more serious projects.
 Version - 1.25.0
 This library could be a module, but yes, later rewritten to module with LIBIO_EXPERIMENTAL functions.
 Some kind of Boost library for poor people.

 You can connect module file by writing:

 target_sources(<Project name>
        PUBLIC
        FILE_SET all_my_modules TYPE CXX_MODULES FILES
        libio.cppm
)

 In your cmake file
*/

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

/**
 * Library flags
 */
// #define LIBIO_EXPERIMENTAL //uncomment/comment this line to turn on/off LIBIO_EXPERIMENTAL library features
// #define LIBIO_TEST //uncomment/comment this line to turn on/off library test
// #define UNSTABLE //turns on unstable versions of very popular functions
// #define LIBIO_WIDE_STRING //functionality to use wide strings
// #define LIBIO_DEPRECATED //deprecated features in libio
// #define LIBIO_ERROR //turn on errors in functions if defined and just write to std::cerr if not

#ifdef LIBIO_EXPERIMENTAL ///define functions and include other libraries if LIBIO_EXPERIMENTAL tag is defined
#pragma message("Using experimental features")

#include <cmath>
#incldue <vector>
#include <iterator>
#include <sstream>
#endif

export module Libio;

/**
 * Add fully user input experience
 * @param val address of the variable to input in
 */
#define USER_INPUT(val) \
    libio::output::print(">> "); \
    libio::input::user_input(val);\
    libio::output::println(); //just new line symbol

#define REPEAT_FOREVER while(true)

/**
 * Unified namespace for libio library for input/output.
 */
namespace libio {
    using cint [[maybe_unused]]                  = const int;  ///constant custom integer type
    using cbool [[maybe_unused]]                 = const bool; ///constant custom bool type
    export using String_con_ref [[maybe_unused]] = const std::string &;
    export using String [[maybe_unused]]         = std::string;

    /**
     * Namespace for constrains of types using concepts
     */
    export namespace type_constrains {
        template<typename T>
        concept is_string_v = std::is_same_v<T, std::string>;

        /**
         * Check for standard library container
         */
        template<typename T>
        concept is_stl_container = requires(T t)
        {
            // Type must have an iterator type
            typename T::iterator;

            // Must have a begin() method returning an iterator
            { t.begin() } -> std::same_as<typename T::iterator>;

            // Must have an end() method returning an iterator
            { t.end() } -> std::same_as<typename T::iterator>;
        };

        template<typename T>
        concept is_number_v = std::is_integral_v<T> || std::is_floating_point_v<T>;

        template<typename T>
        concept is_bool_v = std::convertible_to<T, bool> && std::same_as<T, bool>;
    }

    /**
     * Contains different output logic
     */
    namespace output {
        /**
         * Namespace for colored text output
         */
        export namespace colored {
            struct Ansi_colors {
                static const std::string _clear_color;
                static const std::string _color_start;
                static const std::string WHITE;
                static const std::string RED;
                static const std::string GREEN;
                static const std::string YELLOW;
                static const std::string MAGENTA;
                static const std::string CYAN;
            };

            const std::string Ansi_colors::_color_start = "\033[";
            const std::string Ansi_colors::_clear_color = "\033[0m";
            const std::string Ansi_colors::WHITE        = "\033[37m";
            const std::string Ansi_colors::RED          = "\033[31m";
            const std::string Ansi_colors::GREEN        = "\033[32m";
            const std::string Ansi_colors::YELLOW       = "\033[33m";
            const std::string Ansi_colors::MAGENTA      = "\033[35m";
            const std::string Ansi_colors::CYAN         = "\033[36m";

            /**
             * Print generic object in color to console.
             * @tparam T generic type
             * @param object generic type object to print.
             * @param separator string separator
             * @param color std::string object with ANSI color sequence
             */
            template<typename T>
            void
            colored_print(const T &object, const std::string &separator = " ", const std::string &color = Ansi_colors::WHITE) {
                if (std::cout.good()) {
                    std::cout << color << object << Ansi_colors::_clear_color << separator;
                }
#ifdef LIBIO_ERROR
                throw std::runtime_error("Colored print is failed");
#else
                std::cerr << "colored print is failed" << std::endl;
#endif
            }

            template<typename T>
            void
            colored_println(const T &object, const std::string &color = Ansi_colors::WHITE) {
                if (std::cout.good()) {
                    std::cout << color << object << Ansi_colors::_clear_color << "\n";
                }
#ifdef LIBIO_ERROR
                throw std::runtime_error("Colored println is failed");
#else
                std::cerr << "Colored println is failed" << std::endl;
#endif
            }
        }

#ifdef UNSTABLE
#warning "Using unstable functions in libio, be careful"
        /**
         * Print given generic message in console with new line. By default, equal to "".
         * @warning If using C++23 - use std::println.
         * @param str string to output
         * @tparam T generic parameter of type to console print
         */
        export template<typename T = std::string>
        void println(const T &str = "\n") {
            std::cout << str << std::endl;
        }

        /**
         * Print given generic message in console without new line.
         * @warning If using C++23 - use std::print.
         * @tparam T generic type
         * @param str string to output
         * @param separator text separator
         */
        export template<typename T>
        void print(const T &str, std::string separator = "") {
            std::cout << str << separator;
        }

#elifndef UNSTABLE

        /**
         * Print given generic message in console with new line. By default, equal to "".
         * @warning If using C++23 - use std::println.
         * @param str string to output
         * @tparam T generic parameter of type to console print
         */
        export template<typename T = std::string>
        void println(const T &str = "\n") {
            if (std::cout.good()) {
                std::cout << str << std::endl;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("Println is failed due to cout");
#else
            std::cerr << "Println is failed due to cout" << std::endl;
#endif
        }

        /**
         * Print given generic message in console without new line.
         * @warning If using C++23 - use std::print.
         * @tparam T generic type
         * @param str string to output
         * @param separator text separator
         */
        export template<typename T>
        void print(const T &str, std::string separator = "") {
            if (std::cout.good()) {
                std::cout << str << separator;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("Print is failed due to cout");
#else
            std::cerr << "Print is failed due to cout" << std::endl;
#endif
        }

#endif

#ifdef LIBIO_WIDE_STRING
#pragma message("Using libio wide string functionality")

        /**
         * Print given wide string message in console with new line.
         * @warning If using C++23 - use std::println.
         * @param str string to output
         */
        export void println_w(const std::wstring &str) {
            if (std::wcout.good()) {
                std::wcout << str << std::endl;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("Println_w is failed due to wcout");
#else
            std::cerr << "Println_w is failed due to wcout" << std::endl;
#endif
        }

        /**
         * Print given wide string message in console without new line.
         * @warning If using C++23 - use std::print.
         * @param str string to output
         * @param separator text separator
         */
        export void print_w(const std::wstring &str, const std::wstring &separator) {
            if (std::wcout.good()) {
                std::wcout << str << separator;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("Print_w is failed due to wcout");
#else
            std::cerr << "Print_w is failed due to wcout" << std::endl;
#endif
        }
#endif

        /**
         * Function for array output with separator.
         * Older brother of dynamicArrayOutput
         * @param array generic array pointer
         * @param array_size size of array.
         * @param separator separator value between elements
         * @param is_inline I do not know why I do this.
         */
        export template<typename T>
        void line_array_output(const T *  array, const int array_size, const std::string &separator = " ",
                               const bool is_inline                                                 = false) {
            for (int i = 0; i < array_size - 1; ++i) {
                std::cout << array[i] << separator;
            }
            std::cout << array[array_size - 1];

            if (!is_inline) {
                std::cout << std::endl;
            }
        }

        /**
         * Output container from STL into console
         * @tparam T generic type
         * @param array standard container
         * @param separator separator string
         * @param endsymbol symbol at the end of output sequence
         */
        export template<typename T>
            requires std::copyable<T>
        void line_array_output(T array, const std::string &separator = " ", const std::string &endsymbol = "") {
            const size_t array_size = array.size();
            int          i          = 0;
            for (; i < array_size - 1; ++i) {
                std::cout << array[i] << separator;
            }
            std::cout << array[i] << endsymbol;
        }

        export template<typename T>
            requires libio::type_constrains::is_stl_container<T>
        std::string line_array_output_return(T array, const std::string &separator = " ") {
            const size_t array_size = array.size();
            int          i          = 0;
            std::string  result;
            for (; i < array_size - 1; ++i) {
                result += array[i];
                result += separator;
            }
            result += array[i];
            result += "";
            return result;
        }

        /**
         * New technology parametrized function for array output with old innovations
         * @tparam T generic type
         * @param array generic array pointer.
         * @param size size of the array.
         * @param reverse order of array output, seq or reverse.
         * @param separator separator value between elements
         */
        export template<typename T>
        void
        dynamic_array_output(const T *array, const int size, const bool reverse = false, const std::string &separator = " ") {
            if (reverse) {
                for (int i = size - 1; i >= 0; --i) {
                    std::cout << array[i] << separator;
                }
            } else {
                for (int i = 0; i < size; ++i) {
                    std::cout << array[i] << separator;
                }
            }
            std::cout << std::endl;
        }

        /**
         * Weird construction for correct output separator.
         * @tparam T generic type
         * @param container container object to print out
         * @param separator separator value between values
         */
        export template<typename T>
            requires std::copyable<T>
        void print_container(const T &container, const std::string &separator = " ") {
            const size_t container_size = container.size();
            int          i              = 0;
            for (const auto &elem: container) {
                if (i < container_size - 1) {
                    std::cout << elem << separator;
                } else {
                    std::cout << elem;
                }
                ++i;
            }
            std::cout << std::endl;
        }

#ifdef LIBIO_EXPERIMENTAL
        /**
        * Print pyramid object one line by line
        * @param array
        * @param n
        */
        export void print_pyramid(const int *array, const int n) {
            for (int i = 0; i < n; ++i) {
                const int level = (i == 0) ? 0 : static_cast<int>(std::floor(std::log2(static_cast<double>(i + 1))));

                if (i == 0) {
                    std::cout << level << " root " << array[i] << '\n';
                } else {
                    const int   p    = (i - 1) / 2;
                    const char *side = (i == 2 * p + 1) ? "left" : "right";
                    std::cout << level << " " << side << " (" << array[p] << ") " << array[i] << '\n';
                }
            }
        }

        /**
         *
         * @param array pyramid object in array
         * @param i
         */
        export void print_one_element(const int *array, const int i) {
            const int level = (i == 0) ? 0 : static_cast<int>(std::floor(std::log2(static_cast<double>(i + 1))));

            if (i == 0) {
                std::cout << level << " root " << array[i] << '\n';
            } else {
                const int   p    = (i - 1) / 2;
                const char *side = (i == 2 * p + 1) ? "left" : "right";
                std::cout << level << " " << side << " (" << array[p] << ") " << array[i] << '\n';
            }
        }

#endif
    }

    /**
     * Namespace for string actions in libio
     */
    export namespace string {
        /**
         * Split string without separator
         * @param input input string to split
         * @return vector object with strings
         */
        std::vector<std::string> split(std::string const &input) {
            std::stringstream        ss(input);
            std::vector<std::string> result;
            std::string              word;
            while (ss >> word) {
                result.push_back(word);
            }
            return result;
        }

        /**
        * Split string into vector object and return changed string.
        * @param s source string to split.
        * @param delim delimiter to split on.
        * @return vector if you want to assign to variable.
        */
        std::vector<std::string> split(const std::string &s, const std::string &delim = " ") {
            std::vector<std::string>   result;
            const std::regex           del(delim);
            std::sregex_token_iterator it(s.begin(),
                                          s.end(), del, -1);
            const std::sregex_token_iterator end;
            while (it != end) {
                result.push_back(*it);
                ++it;
            }
            return result;
        }

        /**
         * Split method for char delimeter
         * @param str source string
         * @param delimiter
         * @return
         */
        std::vector<std::string> split(const std::string &str, const char delimiter) {
            std::vector<std::string> result;
            std::istringstream       stream(str);
            std::string              token;

            while (std::getline(stream, token, delimiter)) {
                result.push_back(token);
            }

            return result;
        }

        /**
         * Delete whitespaces at begin and end of the given string.
         * @param s source string.
         * @return string object without whitespaces.
         */
        std::string delete_whitespaces(const std::string &s) {
            const size_t first_char_pos = s.find_first_not_of(" \t\n\r\f\v");
            std::string  output_string  = s; //copy source string
            if (first_char_pos != std::string::npos) {
                output_string.erase(0, first_char_pos);
            }
            return output_string;
        }

        /**
         * Change string register by invoking std functions
         * @param str source string value.
         * @param regis output string register, can be either false (upper) or true (lower).
         * @param loc localization object
         * @return string in selected register.
         */
        std::string
        change_string_register(const std::string &str, const bool regis, const std::locale &loc = std::locale("en")) {
            decltype(auto) func = (!regis) ? std::toupper<char> : std::tolower<char>;
            std::string    result;
            for (const auto ch: str) {
                result += func(ch, loc);
            }
            return result;
        }

        /**
         Function for replacing all strings occurrences in string
         @param str source string
         @param replace replace this in source
         @param with replace with this string in source
         @return string with replacements
         */
        [[maybe_unused]] inline std::string &
        replace_string_all(std::string &str, const std::string &replace, const std::string &with) {
            if (!replace.empty()) {
                std::size_t pos = 0;
                while ((pos = str.find(replace, pos)) != std::string::npos) {
                    str.replace(pos, replace.length(), with);
                    pos += with.length();
                }
            }
            return str;
        }

        /**
         * Split string into tuple by delimiter.
        * @param s source string to split
        * @param delim delimiter to split on
        * @return tuple if you want to assign to variable.
        */
        std::tuple<std::string, std::string> split_by_first_delim(const std::string &s, const char delim = ' ') {
            if (const size_t pos = s.find(delim); pos != std::string::npos) {
                const std::string first_part  = s.substr(0, pos);
                const std::string second_part = s.substr(pos + 1);
                return {first_part, second_part};
            }
            throw std::runtime_error("split_by_first_delim: delimiter not found");
        }
#ifdef LIBIO_ERROR
        throw std::runtime_error ("split_by_first_delim: delimiter not found");
#endif
    }

    /**
     * Another unuseful function for string actions
     * @param str source string to trim
     * @return trimmed string
     */
    std::string trim(const std::string &str) {
        const size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) {
            return "";
        }
        const size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, last - first + 1);
    }

#ifdef LIBIO_EXPERIMENTAL
    /**
     * Replace string with another string
     */
    inline std::string replace(const std::string &str, const std::string &replace, const std::string &with) {
        //
    }
#endif

    /**
     * Contains different input logic.
     */
    export namespace input {
#ifdef LIBIO_DEPRECATED
#warning "Using deprecated libio features"

        /**
         * Writes down int value into variable by address
         * @param variableAddress address of variable to output data to it.
         */
        inline void int_user_input(int &variableAddress) {
            if (std::cin.good()) {
                std::cin >> variableAddress;
            }
#ifdef LIBIO_ERROR
        throw std::runtime_error ("split_by_first_delim: delimiter not found");
#endif
        }

        /**
         * Writes down long value into variable by address
         * @param variableAddress address of variable to output data to it.
         */
        inline void long_user_input(long &variableAddress) {
            if (std::cin.good()) {
                std::cin >> variableAddress;
            }
#ifdef LIBIO_ERROR
        throw std::runtime_error ("split_by_first_delim: delimiter not found");
#endif
        }

        /**
         * Writes down string into variable by address
         * @param variableAddress address of variable to output data to it.
         */
        inline void string_user_input(std::string &variableAddress) {
            if (std::cin.good()) {
                std::cin >> variableAddress;
            }
#ifdef LIBIO_ERROR
        throw std::runtime_error ("split_by_first_delim: delimiter not found");
#endif
        }
#endif
        /**
         * Writes down value into variable by address.
         * @tparam T generic type.
         * @param variableAddress address of variable to output data to it.
         */
        template<typename T>
        void user_input(T &variableAddress) {
            if (std::cin.good()) {
                std::cin >> variableAddress;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("split_by_first_delim: delimiter not found");
#else
            std::cerr << "split_by_first_delim: delimiter not found" << std::endl;
#endif
        }

        /**
        * Different variant for userInput.
        * @tparam T generic type for variable.
        * @return variable of generic type.
        */
        template<typename T = std::string>
        T user_input() {
            T variable;
            if (std::cin.good()) {
                std::cin >> variable;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("split_by_first_delim: delimiter not found");
#else
            std::cerr << "User input is failed" << std::endl;
#endif
            return variable;
        }

        /**
         * Input symbols (strings) in line
         * @param input_symbol symbol that appear in start of inputting
         * @return user string
         */
        std::string line_input(const std::string &input_symbol) {
            std::string line;
            std::cout << input_symbol;
            std::getline(std::cin, line);
            return line;
        }
    }

    /**
     * Contains arrays actions
     */
    export namespace array {
        template<typename T>
        void fast_copy(T *dest, const T *src, size_t count) {
            if constexpr (std::is_trivially_copyable<T>::value) {
                std::cout << "Using fast memcpy for trivial type." << std::endl;
                memcpy(dest, src, count * sizeof(T));
            } else {
                std::cout << "Using slow element-by-element copy." << std::endl;
                for (size_t i = 0; i < count; ++i) {
                    dest[i] = src[i];
                }
            }
        }

        /**
         * Delete dynamically allocated array
         * @tparam T generic type.
         * @param array array of generic type.
         * @param rows rows count in this array.
         */
        template<typename T>
        void delete_dynamic_array(T *array, const int rows) {
            for (int i = 0; i < rows; ++i) {
                delete[] array[i];
            }
            delete[] array;
        }

        template<typename T>
        T *create_1d_array(int);

        template<>
        int *create_1d_array(const int rows) {
            const auto dyn_array = new int[rows];
            for (int i = 0; i < rows; ++i) {
                dyn_array[i] = 0;
            }
            return dyn_array;
        }

        /**
         * Create one dimensional array of generic type
         * @param rows rows count
         * @return constructed array of generic type
         */
        template<>
        std::string *create_1d_array(const int rows) {
            const auto dyn_array = new std::string[rows];
            for (int i = 0; i < rows; ++i) {
                dyn_array[i] = "";
            }
            return dyn_array;
        }

        /**
         * Inline function for creating 2d generic array
         * @param rows rows of the array
         * @param cols columns of the array
         * @tparam T generic param for type of the objects in array
         * @return: initialized 2d generic array
         */
        template<typename T>
        T **create_2d_array(const int rows, const int cols) {
            const auto dyn_array = new T *[rows];
            for (int i = 0; i < rows; ++i) {
                dyn_array[i] = new T *[cols];
            }
            return dyn_array;
        }

        /**
         * @tparam T generic type
         * @param depth
         * @param sizes
         * @return vector with elements
         */
        template<typename T>
        std::vector<T> create_ndim_array(const size_t depth, const std::vector<size_t> &sizes) {
            if (depth == sizes.size()) {
                return {};
            }
            std::vector<std::vector<void *> > result(sizes[depth]);
            for (auto &sub: result) {
                sub = create_ndim_array<T>(depth + 1, sizes);
            }
            return static_cast<std::vector<T>>(result);
        }

        /**
         * Resolve old string (c-style) string and return its size.
         * @param old_string c-style string
         * @return int value of size
         */
        template<typename T>
        int get_dynamic_array_size([[maybe_unused]] T *old_string) {
            constexpr int count = sizeof(old_string) / sizeof(old_string[0]);
            return count;
        }

#ifdef LIBIO_EXPERIMENTAL
        /**
         *
         * @tparam T generic type
         * @param sizes
         * @return
         */
        template<typename T>
        std::vector<T> create_ndim_array(const std::vector<size_t> &sizes) {
            if (sizes.empty())
                return std::vector<T>();

            std::vector<T> flat;
            if (sizes.size() == 1) {
                return std::vector<T>(sizes[0]);
            }
            if (sizes.size() == 2) {
                std::vector<std::vector<T> > result(sizes[0], std::vector<T>(sizes[1]));
                return result;
            }
            return;
        }

        std::tuple<int *, int, int> increase_dynamic_array(int *arr, int logical_size, int actual_size) {
            if (arr != nullptr) {
                actual_size  *= 2;
                auto new_arr = new int[actual_size];
                for (int i = 0; i < logical_size; ++i) {
                    new_arr[i] = arr[i];
                }
                delete[] arr;
                return {new_arr, logical_size, actual_size};
            }
#ifdef LIBIO_ERROR
        throw std::runtime_error ("Cannot allocate memory for array");
#else
        std::cerr<< "Cannot allocate memory for array" << std::endl;
#endif
        throw;
        }
#endif
    }

    /**
     * Contains file actions.
     * Ex. write or create.
     */
    export namespace file {
        /**
         * Creates file for read and write.
         * @param fileName name of the file, create if not exists.
         * @return file handler or nullptr if error occurred.
         */
        inline std::ofstream create_write_file(const std::string &fileName) {
            try {
                std::ofstream file(fileName);
                return file;
            } catch (const std::exception &e) {
                std::cerr << e.what() << "\n";
            }
            return nullptr;
        }

#ifdef LIBIO_EXPERIMENTAL
        /**
         * Creates file for read and write.
         * @param fileName name of the file, create if not exists.
         * @param is_create create if not exist
         * @return file handler.
         */
        inline std::ofstream create_write_file(const std::string &fileName, bool is_create) {
            if (is_create and check_existance()) {
                std::ofstream file(fileName);
                return file;
            }
        }
#endif

        /**
         * Read file line by line.
         * @param fileName name of the file.
         * @return vector with lines.
         */
        inline std::vector<std::string> read_file(const std::string &fileName) {
            auto lines = std::vector<std::string>();
            if (std::ifstream file(fileName); file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    lines.emplace_back(line);
                }
                file.close();
                return lines;
            }
#ifdef LIBIO_ERROR
            throw std::runtime_error("Error reading file: " + fileName);
#else
            std::cerr << "Error reading file: " << fileName << std::endl;
#endif
            return {};
        }

        /**
         * Read file into string object
         * @param fileName name of the file to read
         * @return string object with file data
         */
        inline std::string read_file2(const std::string &fileName) {
            const std::ifstream in(fileName, std::ios::binary);
            std::ostringstream  out;
            out << in.rdbuf();
            return out.str();
        }

        /**
         * Write file line by line.
         * @param fileName name of the file to write into.
         * @param lines vector value of lines of text.
         * @return output file handler.
         */
        inline std::ofstream write_file(const std::string &fileName, const std::vector<std::string> &lines) {
            auto out = std::ofstream(fileName);
            std::for_each(lines.begin(), lines.end(), [&out](const std::string &line) {
                out << line << std::endl;
            });
            return out;
        }

#ifdef LIBIO_EXPERIMENTAL
        /**
        * Open file and return condition variable of open
        * @param file_name name of the file to open
        * @return tuple with file handler and bool (true if file open)
        */
        std::tuple<std::ifstream, bool> open_file(const std::string &file_name) {
            if (auto file = std::ifstream(file_name); file.is_open()) {
                return std::make_tuple<std::ifstream, bool>(std::move(file), true);
            } else {
                return std::make_tuple<std::ifstream, bool>(std::move(nullptr), false);
            }
        }

        /**
        * Function for checking if file exists in filesystem.
        * @param file_name name of the file to check.
        */
        bool check_file_existence(const std::string &file_name) {
            auto [file, cond] = open_file(file_name);
            if (cond) {
                file.close();
                return true;
            }
            file.close();
            return false;
        }

#endif

#ifdef LIBIO_EXPERIMENTAL
        /**
         * Function for receiving few lines from file.
         * @tparam T generic type.
         * @param fileName name of the file.
         * @param count how many lines to get.
         * @return vector with string values.
         */
        template<typename T>
        std::vector<T> get_few_lines_from(const std::string &fileName, const int count) {
            std::ifstream file(fileName);
            auto          lines         = std::vector<T>();
            int           inner_counter = 0;
            std::string   line;
            while (std::getline(file, line)) {
                ++inner_counter;
                if (inner_counter == count) {
                    break;
                }
                lines.emplace_back(line);
            }
            return lines;
        }

#endif

        /**
         * Platform independent filepath getter.
         * @param optional_file_name
         * @return string value of current path
         */
        std::string get_current_dir_name(const std::string &optional_file_name = "") {
            auto res = std::filesystem::current_path().string();
            if (optional_file_name != "") {
                res += "/" + optional_file_name;
            }
            return res;
        }
    }

    /**
     * Namespace for concurrency things
     */
    namespace concurrency {
        //
    }

    /**
     * Namespace for database tricks
     */
    export namespace database {
        /**
         * Methods of sql execution
         */
        struct Sql_methods {
            static libio::String SELECT;
            static libio::String DELETE;
            static libio::String UPDATE;
            static libio::String INSERT;
            static libio::String CREATE;
            static libio::String DROP;
        };

        libio::String Sql_methods::SELECT = "SELECT";
        libio::String Sql_methods::DELETE = "DELETE";
        libio::String Sql_methods::UPDATE = "UPDATE";
        libio::String Sql_methods::INSERT = "INSERT";
        libio::String Sql_methods::CREATE = "CREATE";
        libio::String Sql_methods::DROP   = "DROP";

        enum DATABASE_TYPE {
            //
        };

#ifdef LIBIO_EXPERIMENTAL
        void create_connection(const std::string &database_name) {
            //
        }

        void close_connection(const std::string &database_name) {
            //
        }
#endif
    }

#ifdef LIBIO_EXPERIMENTAL
#pragma message("Using libio assembler functions")
    /**
     * Namespace for inline assembler code and other
     */
    namespace other {
        extern "C" int func(int x);
        asm(R"(
        .globl func
        .type func, @function
        func:
        .cfi_startproc
        movl %edi, %eax /* x is in RDI, see x86-64 calling convention */
        addl $1, %eax
        ret
        .cfi_endproc
    )");
    }
#endif

    /**
     * Namespace for type convertion without using reinterpret or other cast
     */
    export namespace convert {
        /**
         * Convert string representation into bool value;
         * @param str source string object
         * @throw error if source string is unknown
         * @return bool value
         */
        bool str2bool(const std::string &str) {
            if (str == "true" || str == "1" || str == "True") [[likely]] {
                return true;
            }
            if (str == "false" || str == "0" || str == "False") {
                return false;
            }
            throw;
        }

        std::string str2str(const bool source) {
            if (source == true) [[likely]] {
                return "true";
            }
            if (source == false) {
                return "false";
            }
            throw;
        }

        template<typename T>
        T convert_to_t(const std::string &source);

        template<>
        int convert_to_t(const std::string &source) {
            try {
                return std::stoi(source);
            } catch (const std::exception &e) {
#ifdef LIBIO_ERROR
                throw std::runtime_error("Cannot convert string to '" + source + "' in int " + e.what());
#else
                std::cerr << "split_by_first_delim: delimiter not found" << std::endl;
                return -1;
#endif
            }
        }

        template<>
        std::string convert_to_t(const std::string &source) {
            return source.empty() ? "0" : source;
        }

        template<>
        bool convert_to_t(const std::string &source) {
            if (source == "false" || source == "False") {
                return false;
            }
            if (source == "true" || source == "True") {
                return true;
            }
            return false;
        }

#ifdef LIBIO_EXPERIMENTAL
        template<>
        int conver_to_t(const std::string &str) {
            if (!str) {
                return 0;
            }

            int sign = 1;
            int i    = 0;

            while (str[i] == ' ') {
                i++;
            }

            if (str[i] == '-' || str[i] == '+') {
                sign = (str[i++] == '-') ? -1 : 1;
            }

            while (str[i] >= '0' && str[i] <= '9') {
                result = result * 10 + (str[i++] - '0');

                if (result *sign



                >
                std::numeric_limits<int>::max()
                ) {
                    return std::numeric_limits<int>::max();
                }
                if (result *sign<std::numeric_limits<int>::min()) {
                    return std::numeric_limits<int>::min();
                }
            }

            return static_cast<int>(result * sign);
        }

#endif

        /**
        * Convert usual string object to wide string.
        * @param str source std::string object
        * @return wide string object
        */
        std::wstring to_wstring(const std::string &str) {
            std::vector<wchar_t> buf(str.size());
            std::use_facet<std::ctype<wchar_t> >(std::locale()).widen(str.data(),
                                                                      str.data() + str.size(),
                                                                      buf.data());
            return std::wstring{buf.data(), buf.size()};
        }
    }
}
