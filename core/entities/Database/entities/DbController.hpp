#ifndef NETOLIBPQ_DBCONTROLLER_HPP
#define NETOLIBPQ_DBCONTROLLER_HPP

#include <string>
#include <unordered_map>
#include <pqxx/pqxx>
#include "../../UI/entities/SearchHit.hpp"

class DB_controller {
private:
    std::unique_ptr<pqxx::connection> cx; //one connection to rule the world
    bool prepared{};

public:
    DB_controller(const std::string &host, const std::string &port, const std::string &db_name,
                  const std::string &user_name, const std::string &password);

    ~DB_controller();

    DB_controller(DB_controller &) = delete;

    void init_tables();

    void drop_tables() const;

    void add_document(const std::unordered_map<std::string, int> &document_data,
                      const std::string &dir_path,
                      const std::string &file_name) noexcept(false);

    [[nodiscard]] std::vector<SearchHit> find_words(const std::vector<std::string> &query_words) const noexcept;
};

#endif
