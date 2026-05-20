#pragma once

#include <QHash>
#include <QList>
#include <QString>

#include "core/models/InventoryEvent.h"
#include "data/DatabaseManager.h"

class QSqlQuery;

class InventoryEventRepository
{
public:
    explicit InventoryEventRepository(DatabaseManager& databaseManager);

    bool insert(const InventoryEvent& event);
    bool existsByUuid(const QString& eventUuid);
    QList<InventoryEvent> findAll();
    double currentStock(int materialId);
    QHash<int, double> stockMap();
    QString lastError() const;

private:
    InventoryEvent fromQuery(const QSqlQuery& query) const;

    DatabaseManager& m_databaseManager;
    QString m_lastError;
};
