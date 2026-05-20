#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DatabaseManager::DatabaseManager()
    : m_connectionName("inventory_bom_" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

DatabaseManager::~DatabaseManager()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            db.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool DatabaseManager::open(const QString& databasePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(databasePath);

    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = ON");
    return true;
}

QSqlDatabase DatabaseManager::database() const
{
    return QSqlDatabase::database(m_connectionName);
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}
