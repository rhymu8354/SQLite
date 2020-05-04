/**
 * @file examples/play1/main.cpp
 *
 * This is a small playground application for experimenting with SQLite.
 *
 * This example simply reads one value from a key-value kind of table.
 */

#include <functional>
#include <memory>
#include <sqlite3.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

using DatabaseConnection = std::unique_ptr< sqlite3, std::function< void(sqlite3*) > >;
using PreparedStatement = std::unique_ptr< sqlite3_stmt, std::function< void(sqlite3_stmt*) > >;

DatabaseConnection OpenDatabase(const std::string& path) {
    sqlite3* dbRaw;
    if (sqlite3_open(path.c_str(), &dbRaw) != SQLITE_OK) {
        return nullptr;
    }
    return DatabaseConnection(
        dbRaw,
        [](sqlite3* dbRaw){
            (void)sqlite3_close(dbRaw);
        }
    );
}

PreparedStatement BuildStatement(
    const DatabaseConnection& db,
    const std::string& statement
) {
    sqlite3_stmt* statementRaw;
    if (
        sqlite3_prepare_v2(
            db.get(),
            statement.c_str(),
            (int)(statement.length() + 1), // sqlite wants count to include the null
            &statementRaw,
            NULL
        ) != SQLITE_OK)
    {
        return nullptr;
    }
    return PreparedStatement(
        statementRaw,
        [](sqlite3_stmt* statementRaw){
            (void)sqlite3_finalize(statementRaw);
        }
    );
}

void BindStatementParameter(
    const PreparedStatement& stmt,
    int index,
    const std::string& value
) {
    (void)sqlite3_bind_text(
        stmt.get(),
        index,
        value.data(),
        (int)value.length(),
        SQLITE_TRANSIENT
    );
}

struct StepStatementResults {
    bool done = false;
    bool error = false;
};

StepStatementResults StepStatement(const PreparedStatement& stmt) {
    StepStatementResults results;
    switch (sqlite3_step(stmt.get())) {
        case SQLITE_DONE: {
            results.done = true;
        } break;

        case SQLITE_ROW: {
        } break;

        default: {
            results.error = true;
        } break;
    }
    return results;
}

int FetchColumnInt(
    const PreparedStatement& stmt,
    int index
) {
    return sqlite3_column_int(stmt.get(), index);
}

int main(int argc, char* argv[]) {
    // Open our test database.
    //
    // By the way, this is how we created it:
    //
    // sqlite> create table globals(key text primary key, value text);
    // sqlite> insert into globals values("GameJournalGeneration", 153010);
    // sqlite> insert into globals values("lastTerm", 3720);
    // sqlite> insert into globals values("lastIndex", 38962673);
    //
    const auto db = OpenDatabase("test.db");
    if (!db) {
        fprintf(stderr, "Unable to open database!\r\n");
        return EXIT_FAILURE;
    }
    printf("We're in!  admHack\r\n");

    // Make a prepared statement we can use to look up anything in the globals
    // table.
    auto stmt = BuildStatement(db, "SELECT value FROM globals WHERE key = ?");

    // Fetch something that we know is in the globals table.
    //
    // This entails the following:
    // 1. Binding values for parameters in the statement.
    // 2. "Stepping" the statement at least once.  Each step gives us one more
    //    row of results, from which we can fetch individual columns of data.
    const std::string key = "lastTerm";
    BindStatementParameter(stmt, 1, key);
    auto results = StepStatement(stmt);  // First call makes the value available.
    if (results.error || results.done) {
        fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
        return EXIT_FAILURE;
    }
    const auto lastTerm = FetchColumnInt(stmt, 0);
    results = StepStatement(stmt);  // Second call completes the query.
    if (results.error || !results.done) {
        fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
        return EXIT_FAILURE;
    }
    printf("lastTerm is %d\n", lastTerm);

    // Now we will demonstrate the error handling.  Let's construct a new
    // statement which will cause an error when we step it.
    stmt = BuildStatement(db, "SELECT foo FROM bar");
    if (StepStatement(stmt).error) {
        fprintf(stderr, "Good, we got an error as expected.\n");
    } else {
        fprintf(stderr, "Oops, that should have been an error!\n");
    }

    // That was fun!
    return EXIT_SUCCESS;
}
