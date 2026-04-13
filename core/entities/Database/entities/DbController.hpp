#ifndef NETOLIBPQ_DBCONTROLLER_HPP
#define NETOLIBPQ_DBCONTROLLER_HPP

#include <memory>
#include <string>
#include <vector>

#include <pqxx/pqxx>

import Libio;

struct Client {
    std::string              name;
    std::string              surname;
    std::string              email;
    std::vector<std::string> phones;
};

/**
 * aka txt file
 */
struct Document {
    std::string name;
    std::string path_to_document;
};

struct Words {
    std::string value;
};

class DB_controller {
    private:
        std::unique_ptr<pqxx::connection> cx; //one connection to rule the world
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

        friend class DB_controller_builder;

        void init_tables();

        void drop_tables() const;

        void select_all() const;

        void add_document(const Document &document) const; ///add document into database

        void find_word(libio::String_con_ref &query) const;

        // void add_client(const Client &client, const std::string &phone) const;
        //
        // void add_phone(const std::string &name, const std::string &phone) const;
        //
        // void update_client(const std::string &email, const std::string &new_name, const std::string &new_surname,
        //                    const std::string &new_email) const;
        //
        // void remove_phone(const std::string &email, const std::string &phone) const;
        //
        // void remove_client(const std::string &email) const;
};

class DB_controller_builder {
    private:
        DB_controller controller;

    public:
        DB_controller_builder &set_host(const std::string &);

        DB_controller_builder &set_port(const std::string &);

        DB_controller_builder &set_db_name(const std::string &);

        DB_controller_builder &set_user(const std::string &);

        DB_controller_builder &set_password(const std::string &);

        DB_controller build();
};

#endif
