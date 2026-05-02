#include "DbController.hpp"

DB_controller::~DB_controller() = default;

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
        return;
    }
    catch (...) {
        return;
    }
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
        return;
    }
    catch (const std::exception &e) {
        trn.abort();
        return;
    }
    catch (...) {
        trn.abort();
        return;
    }
}

DB_controller::DB_controller(const std::string &host, const std::string &port, const std::string &db_name,
                             const std::string &user_name, const std::string &password) {

    std::string builder_strings;
    builder_strings += "host=" + host + " ";
    builder_strings += "port=" + port + " ";
    builder_strings += "dbname=" + db_name + " ";
    builder_strings += "user=" + user_name + " ";
    builder_strings += "password=" + password;

    try {
        cx = std::make_unique<pqxx::connection>(builder_strings.c_str());
    }
    catch (const pqxx::broken_connection &e) {
        return;
    }
}
