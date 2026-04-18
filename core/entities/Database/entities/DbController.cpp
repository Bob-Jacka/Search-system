#include "DbController.hpp"
#include "SQLexception.hpp"

///////////////////////DB_controller

DB_controller::DB_controller(DB_controller &&other) noexcept {
    cx = std::move(other.cx);
    host = std::move(other.host);
    port = std::move(other.port);
    db_name = std::move(other.db_name);
    user_name = std::move(other.user_name);
    password = std::move(other.password);
    other.cx = nullptr;
    try {
        std::string builder_strings;
        builder_strings += "host= " + db_name + "\n";
        builder_strings += "port= " + port + "\n";
        builder_strings += "dbname= " + db_name + "\n";
        builder_strings += "user= " + user_name + "\n";
        builder_strings += "password= " + password;

        cx = std::make_unique<pqxx::connection>(
                builder_strings
        );
    } catch (...) {
        throw SQLexception(__LINE__, "Error in auth postgres user", __FILE_NAME__);
    }
}

void DB_controller::init_tables() {
    using namespace libio::database;
    try {
        pqxx::transaction trn(*cx);

        trn.exec(Sql_methods::CREATE + R"( TABLE IF NOT EXISTS Documents (
            id BIGSERIAL PRIMARY KEY,
            file_path TEXT NOT NULL UNIQUE,
            file_name TEXT NOT NULL,
        );

        CREATE TABLE IF NOT EXISTS Words (
            id BIGSERIAL PRIMARY KEY,
            word VARCHAR(255) NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS DocumentWords (
            document_id BIGINT NOT NULL,,
            word_id BIGINT NOT NULL,
            frequency INTEGER NOT NULL,

            PRIMARY KEY (document_id, word_id),

            CHECK (frequency > 0),

            FOREIGN KEY (document_id) REFERENCES documents(id) ON DELETE CASCADE,
            FOREIGN KEY (word_id) REFERENCES words(id) ON DELETE CASCADE
        );
    )");
        trn.commit();
    } catch (...) {
        throw SQLexception(__LINE__, "Error in initializing tables", __FILE_NAME__);
    }
}

QList<SearchHit> DB_controller::find_words(const QStringList &query_words) const {
    QList<SearchHit> results;

    std::vector<std::string> unique_words = query_words;
    std::sort(unique_words.begin(), unique_words.end());
    unique_words.erase(std::unique(unique_words.begin(), unique_words.end()), unique_words.end());

    if (unique_words.empty()) {
        return results;
    }

    pqxx::nontransaction trn(*cx);

    try {
        pqxx::result res = trn.exec_params(
                "SELECT d.file_name, d.file_path, SUM(dw.frequency) AS score "
                "FROM documents d "
                "JOIN document_words dw ON d.id = dw.document_id "
                "JOIN words w ON dw.word_id = w.id "
                "WHERE w.word = ANY($1) "
                "GROUP BY d.id, d.file_name, d.file_path "
                "ORDER BY score DESC;",
                unique_words
        );

        for (const auto &row: res) {
            results.push_back({
                                      .file_name = row["file_name"].as<std::string>(),
                                      .file_path = row["file_path"].as<std::string>(),
                                      .total_score = row["score"].as<int>()
                              });
        }
    }
    catch (const pqxx::sql_error &e) {
        throw SQLexception(__LINE__, "Failed to search words", __FILE_NAME__);
    }
    catch (const std::exception &e) {
        throw SQLexception(__LINE__, "Failed to search words", __FILE_NAME__);
    }

    return results;
}

void DB_controller::drop_tables() const {
    using namespace libio::database;
    pqxx::transaction trn(*cx);
    trn.exec(Sql_methods::DROP + " TABLE IF EXISTS Documents, Words, DocumentWords;");
    trn.commit();
}

void DB_controller::add_document(const std::unordered_map<std::string, int> &document_data,
                                 const std::string &dir_path,
                                 const std::string &file_name) const {
    pqxx::work trn(*cx);
    cx->set_client_encoding("UTF8");

    try {
        pqxx::row doc_row = trn.exec_params1(
                "INSERT INTO documents (file_path, file_name) "
                "VALUES ($1, $2, $3) "
                "RETURNING id;",
                dir_path, file_name
        );

        auto doc_id = doc_row[0].as<std::int64_t>();

        cx->prepare("upsert_word_and_link",
                    "WITH w AS ( "
                    "  INSERT INTO words (word) VALUES ($1) "
                    "  ON CONFLICT (word) DO UPDATE SET word = EXCLUDED.word "
                    "  RETURNING id "
                    ") "
                    "INSERT INTO document_words (document_id, word_id, frequency) "
                    "VALUES ($2, (SELECT id FROM w), $3) "
                    "ON CONFLICT (document_id, word_id) DO UPDATE SET frequency = EXCLUDED.frequency;"
        );

        for (const auto &[word, frequency]: document_data) {
            if (word.empty()) {
                continue;
            }

            trn.exec_prepared("upsert_word_and_link", word, doc_id, frequency);
        }

        trn.commit();
    }
    catch (const pqxx::sql_error &e) {
        trn.abort();
        throw SQLexception(__LINE__, "Failed to add Document data", __FILE_NAME__);
    }
    catch (const std::exception &e) {
        trn.abort();
        throw SQLexception(__LINE__, "Failed to add Document data", __FILE_NAME__);
    }
}

//Controller builder class:
DB_controller_builder &DB_controller_builder::set_host(const std::string &host_str) {
    controller.host = host_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_port(const std::string &port_str) {
    controller.port = port_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_db_name(const std::string &db_name_str) {
    controller.db_name = db_name_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_user(const std::string &user_str) {
    controller.user_name = user_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_password(const std::string &password_str) {
    controller.password = password_str;
    return *this;
}

DB_controller DB_controller_builder::build() {
    return std::move(this->controller);
}
