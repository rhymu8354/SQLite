/**
 * @file examples/play4/main.cpp
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

std::string FetchColumnString(
    const PreparedStatement& stmt,
    int index
) {
    return (const char*)sqlite3_column_text(stmt.get(), index);
}

int GetCloseTile(
    const DatabaseConnection& db,
    int entity
) {
    const auto stmt = BuildStatement(
        db,
        "SELECT json_extract(on_close, \"$.tile.id\") FROM doors WHERE entity = ?"
    );
    BindStatementParameter(stmt, 1, entity);
    const auto stepStatementResults = StepStatement(stmt);
    if (stepStatementResults.error) {
        fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
        return 0;
    }
    return FetchColumnInt(stmt, 0);
}

void SetCloseTile(
    const DatabaseConnection& db,
    int entity,
    int tile
) {
    auto stmt = BuildStatement(
        db,
        "SELECT json_replace(on_close, \"$.tile.id\", ?) FROM doors WHERE entity = ?"
    );
    BindStatementParameter(stmt, 1, tile);
    BindStatementParameter(stmt, 2, entity);
    auto stepStatementResults = StepStatement(stmt);
    if (stepStatementResults.error) {
        fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
        return;
    }
    const auto onClose = FetchColumnString(stmt, 0);
    stmt = BuildStatement(
        db,
        "UPDATE doors SET on_close = ? WHERE entity = ?"
    );
    BindStatementParameter(stmt, 1, onClose);
    BindStatementParameter(stmt, 2, entity);
    stepStatementResults = StepStatement(stmt);
    if (stepStatementResults.error) {
        fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
        return;
    }
}

bool DumpTable(const DatabaseConnection& db) {
    printf("-----------------------------------------------------\n");
    const auto stmt = BuildStatement(db, "SELECT entity, on_close FROM doors");
    StepStatementResults stepStatementResults;
    while (
        stepStatementResults = StepStatement(stmt),
        !stepStatementResults.done
    ) {
        if (stepStatementResults.error) {
            return false;
        }
        const auto entity = FetchColumnInt(stmt, 0);
        const auto onClose = FetchColumnString(stmt, 1);
        printf(
            "Entity %d: on_close=\"%s\"\n",
            entity,
            onClose.c_str()
        );
    }
    printf("-----------------------------------------------------\n");
    return true;
}

int main(int argc, char* argv[]) {
    // Open our test database.
    //
    // By the way, this is how we created it:
    //
    // sqlite> create table doors(entity int primary key, locked int(1), open int(1), on_close text, on_open text);
    // sqlite> insert into doors values(44466, 0, 1, json('{"tile": {"id": 2}}'), json('{"tile": {"id": 1}}'));
    //
    const auto db = OpenDatabase("test.db");
    if (!db) {
        fprintf(stderr, "Unable to open database!\n");
        return EXIT_FAILURE;
    }

    // These demonstrate reading and modifying a JSON value in the database.
    DumpTable(db);
    auto tile = GetCloseTile(db, 44466);
    printf("The close tile is %d.\n", tile);
    tile = 3;
    printf("Changing the close tile to %d.\n", tile);
    SetCloseTile(db, 44466, tile);
    tile = GetCloseTile(db, 44466);
    printf("The close tile is now %d.\n", tile);
    DumpTable(db);

    // That was fun!
    return EXIT_SUCCESS;
}
