#pragma once

#include <QList>
#include <QString>

#include "core/models/BomItem.h"
#include "data/DatabaseManager.h"

class QSqlQuery;

class BomRepository
{
public:
    explicit BomRepository(DatabaseManager& databaseManager);

    QList<BomItem> findAll();
    QList<BomItem> findByProductId(int productId);
    BomItem findById(int id);
    bool upsert(int productId, int materialId, double quantityPerSet);
    bool remove(int id);
    QString lastError() const;

private:
    BomItem fromQuery(const QSqlQuery& query) const;

    DatabaseManager& m_databaseManager;
    QString m_lastError;
};
