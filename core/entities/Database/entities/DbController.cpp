#include "DbController.hpp"
#include "SQLexception.hpp"

///////////////////////DB_controller

App::DB_controller::DB_controller() {
    try {
        //Test connection
        cx = std::make_unique<pqxx::connection>(
                "host=localhost "
                "port=5432 "
                "dbname=postgres "
                "user=postgres "
                "password=''");
    } catch (...) {
        throw SQLexception(__LINE__, "Error in auth postgres user", __FILE_NAME__);
    }
}

void App::DB_controller::init_tables() {
    using namespace libio::database;
    try {
        pqxx::transaction trn(*cx);
        trn.exec(Sql_methods::CREATE + R"( TABLE IF NOT EXISTS clients(id SERIAL PRIMARY KEY, name TEXT, surname TEXT, email TEXT UNIQUE);
        CREATE TABLE IF NOT EXISTS phones(id SERIAL PRIMARY KEY, client_id INT REFERENCES clients(id), phone TEXT UNIQUE);)");
        trn.commit();
    } catch (...) {
        throw SQLexception(__LINE__, "Error in initializing tables", __FILE_NAME__);
    }
}

std::vector<App::DB_entities::Client>
App::DB_controller::find_clients(libio::String_con_ref query) const {
    using namespace libio::database;
    using namespace DB_entities;
    std::vector<Client> result;
    pqxx::transaction trn(*cx);
    pqxx::result res = trn.exec_params(Sql_methods::SELECT +
                                       " * "
                                       "FROM clients "
                                       "LEFT JOIN public.phones on public.clients.id = public.phones.client_id "
                                       "WHERE clients.name = $1 OR clients.surname = $1 OR clients.email = $1 OR phones.phone = $1;",
                                       query);
    for (const auto &row: res) {
        Client client{
                .name = row["name"].as<std::string>(),
                .surname=row["surname"].as<std::string>(),
                .email = row["email"].as<std::string>()
        };
        result.push_back(client);
    }
    return result;
}

void App::DB_controller::drop_tables() const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);
    trn.exec(Sql_methods::DROP + " TABLE IF EXISTS clients, phones;");
    trn.commit();
}

void App::DB_controller::add_client(const DB_entities::Client &client, const std::string &phone) const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);
    cx->set_client_encoding("UTF8");
    pqxx::result res = trn.exec_params(
            Sql_methods::INSERT + " INTO clients(name, surname, email) VALUES ($1, $2, $3) RETURNING id;",
            client.name, client.surname, client.email);
    if (!res.empty()) {
        int client_id = res[0][0].as<int>();
        trn.exec_params(Sql_methods::INSERT + " INTO phones(client_id, phone) VALUES ($1, $2);", client_id,
                        phone);
        trn.commit();
    } else {
        trn.abort();
        throw SQLexception(__LINE__, "Failed to add client", __FILE_NAME__);
    }
}

void App::DB_controller::add_phone(const std::string &name, const std::string &phone) const {
    pqxx::transaction trn(*cx);
    pqxx::result res = trn.exec_params("SELECT id, surname FROM clients WHERE name = $1;", name);
    if (!res.empty()) {
        trn.exec_params("UPDATE phones SET phone = $1;", phone);
        trn.commit();
    } else {
        trn.abort();
        throw SQLexception(__LINE__, "Client not found with name: " + name, __FILE_NAME__);
    }
}

void
App::DB_controller::update_client(const std::string &email, const std::string &newName, const std::string &newSurname,
                                  const std::string &newEmail) const {
    pqxx::transaction trn(*cx);
    pqxx::result res = trn.exec_params("SELECT id FROM clients WHERE email = $1;", email);
    if (!res.empty()) {
        int client_id = res[0][0].as<int>();
        trn.exec_params(R"(UPDATE clients SET name = $1, surname = $2, email = $3 WHERE id = $4;)", newName, newSurname,
                        newEmail, client_id);
        trn.commit();
    } else {
        trn.abort();
        throw SQLexception(__LINE__, "Client not found with new name: " + newName, __FILE_NAME__);
    }
}

void App::DB_controller::remove_phone(const std::string &email, const std::string &phone) const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);
    pqxx::result res = trn.exec_params("SELECT id, name, surname FROM clients WHERE email = $1;", email);
    if (!res.empty()) {
        int client_id = res[0][0].as<int>();
        trn.exec_params(Sql_methods::DELETE + " FROM phones WHERE client_id = $1 AND phone = $2;", client_id, phone);
        trn.commit();
        auto client_name = res[0][1].as<std::string>();
        auto client_surname = res[0][2].as<std::string>();
    } else {
        trn.abort();
        throw SQLexception(__LINE__, "Client not found with email: " + email, __FILE_NAME__);
    }
}

void App::DB_controller::remove_client(const std::string &email) const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);
    pqxx::result res = trn.exec_params("SELECT id, name, surname FROM clients WHERE email = $1;", email);
    if (!res.empty()) {
        int client_id = res[0][0].as<int>();
        auto client_name = res[0][1].as<std::string>();
        auto client_surname = res[0][2].as<std::string>();
        trn.exec_params(Sql_methods::DELETE + " FROM phones WHERE client_id = $1;", client_id);
        trn.exec_params(Sql_methods::DELETE + " FROM clients WHERE id = $1;", client_id);
        trn.commit();
    } else {
        trn.abort();
        throw SQLexception(__LINE__, "Client not found with email: " + email, __FILE_NAME__);
    }
}

void App::DB_controller::select_all() const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);

    pqxx::result tables = trn.exec(
            Sql_methods::SELECT + " table_name FROM information_schema.tables WHERE table_schema='public';");

    for (const auto &table_row: tables) {
        auto table_name = table_row[0].as<std::string>();

        std::string query = "SELECT * FROM " + table_name;
        pqxx::result data = trn.exec(query);

        libio::output::println("Table: " + table_name);
        for (const auto &row: data) {
            for (const auto &field: row) {
                libio::output::print(field.c_str(), " ");
            }
            libio::output::println();
        }
    }
}
