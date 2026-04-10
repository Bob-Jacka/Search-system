#ifndef BASE_EXCEPTION_HPP
#define BASE_EXCEPTION_HPP

#include <source_location>
#include <string>

/**
 * Namespace for exceptions in ini_parser
 */
namespace Parser_n {
    class ParserException final : public std::exception {
        std::string message;
    public:
        explicit ParserException(const std::string &, const std::source_location & = std::source_location::current());

        [[nodiscard]] const char *what() const noexcept override;

        ~ParserException() override = default;
    };
}

#endif
