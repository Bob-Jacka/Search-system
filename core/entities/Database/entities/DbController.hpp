#ifndef NETOLIBPQ_DBCONTROLLER_HPP
#define NETOLIBPQ_DBCONTROLLER_HPP

#include <QList>

#include <pqxx/pqxx>
#include "../../Model/entities/SearchHit.hpp"

class DB_controller {
private:
    std::unique_ptr<pqxx::connection> cx; //one connection to rule the world
    bool prepared = false;

    DB_controller();

    std::string host;
    std::string port;
    std::string db_name;
    std::string user_name;
    std::string password;

public:
    ~DB_controller() = default;

    DB_controller(DB_controller &&) noexcept;

    DB_controller(DB_controller &) = delete;

    DB_controller &operator=(DB_controller &&other) noexcept;

    friend class DB_controller_builder;

    void init_tables();

    void drop_tables() const;

    void add_document(const std::unordered_map<std::string, int> &document_data, const std::string &dir_path,
                      const std::string &file_name) noexcept(false); ///add document into database

    [[nodiscard]] QList<SearchHit> find_words(const QList<QString> &query_words) const noexcept;

    [[nodiscard]] std::vector<SearchHit> find_words(const std::vector<std::string> &query_words) const noexcept;
};

class DB_controller_builder {
private:
    DB_controller to_build;

public:
    DB_controller_builder() = default;

    ~DB_controller_builder() = default;

    DB_controller_builder &set_host(const std::string &);

    DB_controller_builder &set_port(const std::string &);

    DB_controller_builder &set_db_name(const std::string &);

    DB_controller_builder &set_user(const std::string &);

    DB_controller_builder &set_password(const std::string &);

    DB_controller build();
};

#endif
