#include <QMessageBox>
#include "DbController.hpp"

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
        builder_strings += "host=" + host + " ";
        builder_strings += "port=" + port + " ";
        builder_strings += "dbname=" + db_name + " ";
        builder_strings += "user=" + user_name + " ";
        builder_strings += "password=" + password;

        cx = std::make_unique<pqxx::connection>(builder_strings.c_str());
    }
    catch (pqxx::broken_connection &e) {
        QMessageBox(QMessageBox::Icon::Critical, "Error", e.what()).exec();
    }

    catch (...) {
        QMessageBox(QMessageBox::Icon::Critical, "Error", "Error in auth postgres user").exec();
    }
}

DB_controller &DB_controller::operator=(DB_controller &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    cx = std::move(other.cx);
    host = std::move(other.host);
    port = std::move(other.port);
    db_name = std::move(other.db_name);
    user_name = std::move(other.user_name);
    password = std::move(other.password);
    other.cx = nullptr;

    try {
        std::string builder_strings;
        builder_strings += "host=" + host + " ";
        builder_strings += "port=" + port + " ";
        builder_strings += "dbname=" + db_name + " ";
        builder_strings += "user=" + user_name + " ";
        builder_strings += "password=" + password;

        cx = std::make_unique<pqxx::connection>(builder_strings.c_str());
    }
    catch (pqxx::broken_connection &e) {
        QMessageBox(QMessageBox::Icon::Critical, "Error", e.what()).exec();
    }
    catch (...) {
        QMessageBox(QMessageBox::Icon::Critical, "Error", "Error in auth postgres user").exec();
    }
    return *this;
}

void DB_controller::init_tables() {
    try {
        pqxx::transaction trn(*cx);

        trn.exec(R"(CREATE TABLE IF NOT EXISTS Documents (
            id BIGSERIAL PRIMARY KEY,
            file_path TEXT NOT NULL UNIQUE,
            file_name TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS Words (
            id BIGSERIAL PRIMARY KEY,
            word VARCHAR(255) NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS DocumentWords (
            document_id BIGINT NOT NULL,
            word_id BIGINT NOT NULL,
            frequency INTEGER NOT NULL,

            PRIMARY KEY (document_id, word_id),

            CHECK (frequency > 0),

            FOREIGN KEY (document_id) REFERENCES Documents(id) ON DELETE CASCADE,
            FOREIGN KEY (word_id) REFERENCES Words(id) ON DELETE CASCADE
        );
    )");
        trn.commit();
    }
    catch (pqxx::broken_connection &e) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", e.what()).exec();
    }
    catch (...) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Error in initializing tables").exec();
    }
}

QList<SearchHit> DB_controller::find_words(const QStringList &query_words) const noexcept {
    QList<SearchHit> results;
    std::vector<std::string> unique_words;
    unique_words.reserve(query_words.size());

    for (const QString &qstr: query_words) {
        unique_words.push_back(qstr.toUtf8().toStdString());
    }

    std::sort(unique_words.begin(), unique_words.end());
    unique_words.erase(std::unique(unique_words.begin(), unique_words.end()), unique_words.end());

    if (unique_words.empty()) {
        return results;
    }

    pqxx::nontransaction trn(*cx);

    try {
        pqxx::result res = trn.exec_params(
                "SELECT d.file_name, d.file_path, SUM(dw.frequency) AS score "
                "FROM Documents d "
                "JOIN DocumentWords dw ON d.id = dw.document_id "
                "JOIN Words w ON dw.word_id = w.id "
                "WHERE w.word = ANY($1) "
                "GROUP BY d.id, d.file_name, d.file_path "
                "ORDER BY score DESC;",
                unique_words
        );

        for (const auto &row: res) {
            SearchHit hit;
            hit.file_name = row["file_name"].as<std::string>();
            hit.file_path = row["file_path"].as<std::string>();
            hit.total_score = row["score"].as<int>();
            results.push_back(hit);
        }
    }
    catch (const pqxx::sql_error &e) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to search words").exec();
    }
    catch (...) {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Exception in find words").exec();
    }

    return results;
}

/**
 * No ui version of find words
 * @param query_words vector with strings
 * @return vector with results
 */
std::vector<SearchHit> DB_controller::find_words(const std::vector<std::string> &query_words) const noexcept {
    std::vector<SearchHit> results;
    std::vector<std::string> unique_words;
    unique_words.reserve(query_words.size());

    for (const auto &str: query_words) {
        unique_words.push_back(str);
    }

    std::sort(unique_words.begin(), unique_words.end());
    unique_words.erase(std::unique(unique_words.begin(), unique_words.end()), unique_words.end());

    if (unique_words.empty()) {
        return results;
    }

    pqxx::nontransaction trn(*cx);

    try {
        pqxx::result res = trn.exec_params(
                "SELECT d.file_name, d.file_path, SUM(dw.frequency) AS score "
                "FROM Documents d "
                "JOIN DocumentWords dw ON d.id = dw.document_id "
                "JOIN Words w ON dw.word_id = w.id "
                "WHERE w.word = ANY($1) "
                "GROUP BY d.id, d.file_name, d.file_path "
                "ORDER BY score DESC;",
                unique_words
        );

        for (const auto &row: res) {
            SearchHit hit;
            hit.file_name = row["file_name"].as<std::string>();
            hit.file_path = row["file_path"].as<std::string>();
            hit.total_score = row["score"].as<int>();
            results.push_back(hit);
        }
    }
    catch (const pqxx::sql_error &e) {
        printf("Failed to search words");
    }
    catch (...) {
        printf("Exception in find words");
    }

    return results;
}

void DB_controller::drop_tables() const {
    pqxx::transaction trn(*cx);
    trn.exec("DROP TABLE IF EXISTS Documents, Words, DocumentWords;");
    trn.commit();
}

/**
 * Add document into postgresql database
 * @param document_data data from document
 * @param dir_path path to file directory
 * @param file_name name of the file
 */
void DB_controller::add_document(const std::unordered_map<std::string, int> &document_data,
                                 const std::string &dir_path,
                                 const std::string &file_name) noexcept(false) {
    pqxx::work trn(*cx);
    cx->set_client_encoding("UTF8");

    try {
        if (!prepared) {
            cx->prepare("upsert_word_and_link",
                        "WITH w AS ( "
                        "  INSERT INTO Words (word) VALUES ($1) "
                        "  ON CONFLICT (word) DO UPDATE SET word = EXCLUDED.word "
                        "  RETURNING id "
                        ") "
                        "INSERT INTO DocumentWords (document_id, word_id, frequency) "
                        "VALUES ($2, (SELECT id FROM w), $3) "
                        "ON CONFLICT (document_id, word_id) DO UPDATE SET frequency = EXCLUDED.frequency;"
            );
            prepared = true;
        }

        pqxx::row doc_row = trn.exec_params1(
                "INSERT INTO Documents (file_path, file_name) "
                "VALUES ($1, $2) "
                "RETURNING id;",
                dir_path, file_name
        );

        auto doc_id = doc_row[0].as<std::int64_t>();

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
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to add Document data").exec();
        return;
    }
    catch (const std::exception &e) {
        trn.abort();
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Failed to add Document data").exec();
        return;
    }
    catch (...) {
        trn.abort();
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Massive error in add document").exec();
        return;
    }
}

DB_controller::DB_controller() {
    this->cx = nullptr;
}

//Controller builder class:
DB_controller_builder &DB_controller_builder::set_host(const std::string &host_str) {
    to_build.host = host_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_port(const std::string &port_str) {
    to_build.port = port_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_db_name(const std::string &db_name_str) {
    to_build.db_name = db_name_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_user(const std::string &user_str) {
    to_build.user_name = user_str;
    return *this;
}

DB_controller_builder &DB_controller_builder::set_password(const std::string &password_str) {
    to_build.password = password_str;
    return *this;
}

DB_controller DB_controller_builder::build() {
    return std::move(this->to_build);
}
