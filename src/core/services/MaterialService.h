#pragma once

#include <QList>
#include <QString>

#include "core/models/Material.h"
#include "core/models/StockSummary.h"
#include "data/repositories/InventoryEventRepository.h"
#include "data/repositories/MaterialRepository.h"

class MaterialService
{
public:
    MaterialService(MaterialRepository& materialRepository, InventoryEventRepository& eventRepository);

    QList<Material> materials(const QString& keyword = QString());
    QList<StockSummary> stockSummaries(const QString& keyword = QString());
    Material materialById(int id);
    double currentStock(int materialId);
    bool save(Material& material);
    bool remove(int id);
    QString lastError() const;

private:
    MaterialRepository& m_materialRepository;
    InventoryEventRepository& m_eventRepository;
    QString m_lastError;
};
