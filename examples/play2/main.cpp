/**
 * @file play2/main.cpp
 *
 * This is a small playground application for experimenting with SQLite.
 *
 * This example shows how to fetch multiple rows and columns at once.
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

int CountColumns(const PreparedStatement& stmt) {
    return sqlite3_column_count(stmt.get());
}

std::string ColumnName(
    const PreparedStatement& stmt,
    int index
) {
    return sqlite3_column_name(stmt.get(), index);
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

int main(int argc, char* argv[]) {
    // Open our test database.
    //
    // By the way, this is how we created it:
    //
    // sqlite> create table characters(entity int primary key, armor int, con int, dex int, hp int, hpmax int, int int, str int);
    // sqlite> insert into characters values(523, 0, 14, 16, 18, 24, 15, 16);
    // sqlite> insert into characters values(3330, 4, null, 16, 10000, 10000, null, null);
    //
    const auto db = OpenDatabase("play2.db");
    if (!db) {
        fprintf(stderr, "Unable to open database!\r\n");
        return EXIT_FAILURE;
    }
    printf("We're in!  admHack\r\n");

    // Make a prepared statement we can use to look up some attributes
    // of all characters.  Pick the key, one attribute which both
    // characters have, and another attribute which only one character has.
    const auto stmt = BuildStatement(db, "SELECT entity, hp, con FROM characters");

    // Fetch multiple rows and columns.
    //
    // This entails "stepping" the statement.  Each step gives us one more
    // row of results, from which we can fetch individual columns of data.
    //
    // By the way, although we don't need it, we'll also demonstrate how
    // to get the column names as well.
    StepStatementResults stepStatementResults;
    while (
        stepStatementResults = StepStatement(stmt),
        !stepStatementResults.done
    ) {
        // Do the right thing and check for an error first.
        if (stepStatementResults.error) {
            fprintf(stderr, "Something unexpected happened!  Reeeeeeeeee!!!!\n");
            return EXIT_FAILURE;
        }

        // CAUTIONARY NOTE: Relying on the database to tell us how many
        // columns there are, and the column names, is controversial.
        // We're only doing it here to show how it's done and to get
        // some practice.  You should decide for yourself whether this
        // is something that you should do in your own designs, or not.
        const auto numColumns = CountColumns(stmt);
        std::ostringstream columnInfo;
        columnInfo << numColumns << " columns: ";
        for (int i = 0; i < numColumns; ++i) {
            if (i != 0) {
                columnInfo << ", ";
            }
            columnInfo << ColumnName(stmt, i);
        }

        // Fetch the data columns and print them out.
        const auto entity = FetchColumnInt(stmt, 0);
        const auto hp = FetchColumnInt(stmt, 1);
        const auto hasCon = !IsColumnNull(stmt, 2);
        const auto con = FetchColumnInt(stmt, 2);
        printf(
            "Entity %d (%s): hp=%d, con=%d (%s)\n",
            entity,
            columnInfo.str().c_str(),
            hp,
            con,
            hasCon ? "non-null" : "null"
        );
    }

    // That was fun!
    return EXIT_SUCCESS;
}
