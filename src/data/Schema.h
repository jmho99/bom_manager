#pragma once

#include <QString>

#include "data/DatabaseManager.h"

class Schema
{
public:
    explicit Schema(DatabaseManager& databaseManager);

    bool createTables();
    bool resetAllData();
    QString lastError() const;

private:
    bool exec(const QString& sql);

    DatabaseManager& m_databaseManager;
    QString m_lastError;
};
