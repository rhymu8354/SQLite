/**
 * @file play3/main.cpp
 *
 * This is a small playground application for experimenting with SQLite.
 *
 * This example shows how to insert new rows, update existing rows, and delete
 * old rows of a table.
 */

#include <functional>
#include <memory>
#include <sqlite3.h>
#include <sstream>
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
    int value
) {
    (void)sqlite3_bind_int(
        stmt.get(),
        index,
        value
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

bool IsColumnNull(
    const PreparedStatement& stmt,
    int index
) {
    return (sqlite3_column_type(stmt.get(), index) == SQLITE_NULL);
}

bool DumpTable(const DatabaseConnection& db) {
    printf("-----------------------------------------------------\n");
    const auto stmt = BuildStatement(db, "SELECT entity, hp, con FROM characters");
    StepStatementResults stepStatementResults;
    while (
        stepStatementResults = StepStatement(stmt),
        !stepStatementResults.done
    ) {
        if (stepStatementResults.error) {
            return false;
        }
        const auto entity = FetchColumnInt(stmt, 0);
        const auto hp = FetchColumnInt(stmt, 1);
        const auto hasCon = !IsColumnNull(stmt, 2);
        const auto con = FetchColumnInt(stmt, 2);
        printf(
            "Entity %d: hp=%d, con=%d (%s)\n",
            entity,
            hp,
            con,
            hasCon ? "non-null" : "null"
        );
    }
    return true;
}

void DemonstrateInsertRow(const DatabaseConnection& db) {
    printf("Inserting a row...\n");

    // Make a prepared statement we can use to add a row.
    const auto stmt = BuildStatement(
        db,
        "INSERT INTO characters (entity, hp) VALUES (?, ?)"
    );

    // Bind the values we want to insert into the row.
    static const int entity = 42626;
    static const int hp = 24;
    BindStatementParameter(stmt, 1, entity);
    BindStatementParameter(stmt, 2, hp);

    // Submit the query; this does the actual insertion.
    (void)StepStatement(stmt);
}

void DemonstrateUpdateRow(const DatabaseConnection& db) {
    static const int entity = 42626;
    static const int con = 22;
    printf("Updating entity %d to have %d con...\n", entity, con);

    // Make a prepared statement we can use to add a row.
    const auto stmt = BuildStatement(
        db,
        "UPDATE characters SET con = ? WHERE entity = ?"
    );

    // Bind the key and the value we want to update.
    BindStatementParameter(stmt, 1, con);
    BindStatementParameter(stmt, 2, entity);

    // Submit the query; this does the actual insertion.
    (void)StepStatement(stmt);
}

void DemonstrateDeleteRow(const DatabaseConnection& db) {
    static const int hp = 30;
    printf("Deleting all rows where hp is more than %d...\n", hp);

    // Make a prepared statement we can use to add a row.
    const auto stmt = BuildStatement(
        db,
        "DELETE FROM characters WHERE hp > ?"
    );

    // Bind the key and the value we want to update.
    BindStatementParameter(stmt, 1, hp);

    // Submit the query; this does the actual insertion.
    (void)StepStatement(stmt);
}

int main(int argc, char* argv[]) {
    // Open our test database.
    //
    // By the way, this is how we created it:
    //
    // sqlite> create table characters(entity int primary key, armor int, con int, dex int, hp int, hpmax int, int int, str int);
    // sqlite> insert into characters values(523, 0, 14, 16, 18, 24, 15, 16);
    // sqlite> insert into characters values(3330, 4, null, 16, 10000, 10000, null, null);
    //
    const auto db = OpenDatabase("play3.db");
    if (!db) {
        fprintf(stderr, "Unable to open database!\n");
        return EXIT_FAILURE;
    }

    // These demonstrate modifying the database in three different ways:
    // 1. Adding a new row to a table.
    // 2. Updating an existing row.
    // 3. Deleting an old row.
    // Between each demonstration, dump out the table.
    if (!DumpTable(db)) {
        goto error;
    }
    DemonstrateInsertRow(db);
    if (!DumpTable(db)) {
        goto error;
    }
    DemonstrateUpdateRow(db);
    if (!DumpTable(db)) {
        goto error;
    }
    DemonstrateDeleteRow(db);
    if (!DumpTable(db)) {
        goto error;
    }

    // That was fun!
    return EXIT_SUCCESS;
error:
    fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
    return EXIT_FAILURE;
}
