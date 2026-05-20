#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool open(const QString& databasePath);
    QSqlDatabase database() const;
    QString lastError() const;

private:
    QString m_connectionName;
    QString m_lastError;
};
