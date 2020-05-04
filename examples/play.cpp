/**
 * @file play.cpp
 *
 * This is a small playground application for experimenting with SQLite.
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

bool StepStatement(const PreparedStatement& stmt) {
    return (sqlite3_step(stmt.get()) == SQLITE_DONE);
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
    // sqlite>  insert into globals values("lastTerm", 3720);
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
    const auto stmt = BuildStatement(db, "select value from globals where key = ?");

    // Fetch something that we know is in the globals table.
    //
    // This entails the following:
    // 1. Binding values for parameters in the statement.
    // 2. "Stepping" the statement at least once.  Each step gives us one
    //    more row of
    const std::string key = "lastTerm";
    BindStatementParameter(stmt, 1, key);
    (void)StepStatement(stmt); // First call makes the value available.
    const auto lastTerm = FetchColumnInt(stmt, 0);
    (void)StepStatement(stmt); // Second call completes the query.
    printf("lastTerm is %d\n", lastTerm);

    // That was fun!
    return EXIT_SUCCESS;
}
