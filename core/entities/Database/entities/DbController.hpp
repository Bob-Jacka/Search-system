#ifndef NETOLIBPQ_DBCONTROLLER_HPP
#define NETOLIBPQ_DBCONTROLLER_HPP

#include <string>
#include <vector>

#include <pqxx/pqxx>

import Libio;

namespace App {
    namespace DB_entities {
        struct Client {
            std::string name;
            std::string surname;
            std::string email;
            std::vector<std::string> phones;
        };
    }

    class DB_controller {
    private:
        std::unique_ptr<pqxx::connection> cx;

    public:
        DB_controller();

        ~DB_controller() = default;

        void init_tables();

        void drop_tables() const;

        void select_all() const;

        void add_client(const DB_entities::Client &client, const std::string &phone) const;

        void add_phone(const std::string &name, const std::string &phone) const;

        void update_client(const std::string &email, const std::string &new_name, const std::string &new_surname,
                           const std::string &new_email) const;

        void remove_phone(const std::string &email, const std::string &phone) const;

        void remove_client(const std::string &email) const;

        [[nodiscard]] std::vector<DB_entities::Client> find_clients(libio::String_con_ref &query) const;
    };
}

#endif
