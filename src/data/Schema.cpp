#include "Schema.h"

#include <QSqlError>
#include <QSqlQuery>

Schema::Schema(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager)
{
}

bool Schema::createTables()
{
    const QString productsSql = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            memo TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL
        )
    )";

    const QString materialsSql = R"(
        CREATE TABLE IF NOT EXISTS materials (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            unit TEXT NOT NULL DEFAULT 'ea',
            location TEXT,
            memo TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL
        )
    )";

    const QString bomSql = R"(
        CREATE TABLE IF NOT EXISTS bom_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER NOT NULL,
            material_id INTEGER NOT NULL,
            quantity_per_set REAL NOT NULL,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            UNIQUE(product_id, material_id),
            FOREIGN KEY(product_id) REFERENCES products(id) ON DELETE CASCADE,
            FOREIGN KEY(material_id) REFERENCES materials(id) ON DELETE CASCADE
        )
    )";

    const QString eventsSql = R"(
        CREATE TABLE IF NOT EXISTS inventory_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_uuid TEXT NOT NULL UNIQUE,
            event_type TEXT NOT NULL,
            material_id INTEGER,
            material_code TEXT NOT NULL,
            material_name TEXT NOT NULL,
            material_unit TEXT NOT NULL,
            product_id INTEGER,
            product_code TEXT,
            product_name TEXT,
            quantity_delta REAL NOT NULL,
            event_date TEXT NOT NULL,
            created_at TEXT NOT NULL,
            source_device TEXT,
            memo TEXT
        )
    )";

    const QString idxEventMaterialSql = "CREATE INDEX IF NOT EXISTS idx_inventory_events_material_id ON inventory_events(material_id)";
    const QString idxEventUuidSql = "CREATE INDEX IF NOT EXISTS idx_inventory_events_uuid ON inventory_events(event_uuid)";

    return exec(productsSql)
        && exec(materialsSql)
        && exec(bomSql)
        && exec(eventsSql)
        && exec(idxEventMaterialSql)
        && exec(idxEventUuidSql);
}

bool Schema::exec(const QString& sql)
{
    QSqlQuery query(m_databaseManager.database());
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

QString Schema::lastError() const
{
    return m_lastError;
}
