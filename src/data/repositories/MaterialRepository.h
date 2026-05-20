#pragma once

#include <QList>
#include <QString>

#include "core/models/Material.h"
#include "data/DatabaseManager.h"

class QSqlQuery;

class MaterialRepository
{
public:
    explicit MaterialRepository(DatabaseManager& databaseManager);

    QList<Material> findAll(const QString& keyword = QString());
    Material findById(int id);
    Material findByCode(const QString& code);
    bool existsByCode(const QString& code, int excludeId = -1);
    bool save(Material& material);
    bool upsertByCode(Material& material);
    bool remove(int id);
    QString lastError() const;

private:
    Material fromQuery(const QSqlQuery& query) const;
    bool insert(Material& material);
    bool update(const Material& material);

    DatabaseManager& m_databaseManager;
    QString m_lastError;
};
