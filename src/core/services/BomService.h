#pragma once

#include <QList>
#include <QString>

#include "core/models/BomItem.h"
#include "data/repositories/BomRepository.h"
#include "data/repositories/InventoryEventRepository.h"

class BomService
{
public:
    BomService(BomRepository& bomRepository, InventoryEventRepository& eventRepository);

    QList<BomItem> bomItems(int productId);
    QList<BomItem> allBomItems();
    int buildCapacity(int productId, QString* limitingMaterialText = nullptr);
    bool saveBomItem(int productId, int materialId, double quantityPerSet);
    bool removeBomItem(int bomItemId);
    QString lastError() const;

private:
    BomRepository& m_bomRepository;
    InventoryEventRepository& m_eventRepository;
    QString m_lastError;
};
