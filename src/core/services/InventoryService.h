#pragma once

#include <QList>
#include <QString>

#include "core/models/InventoryEvent.h"
#include "data/DatabaseManager.h"
#include "data/repositories/BomRepository.h"
#include "data/repositories/InventoryEventRepository.h"
#include "data/repositories/MaterialRepository.h"
#include "data/repositories/ProductRepository.h"

class InventoryService
{
public:
    InventoryService(DatabaseManager& databaseManager,
                     ProductRepository& productRepository,
                     MaterialRepository& materialRepository,
                     BomRepository& bomRepository,
                     InventoryEventRepository& eventRepository);

    bool purchaseMaterial(int materialId, double quantity, const QString& eventDate, const QString& memo);
    bool consumeMaterial(int materialId, double quantity, const QString& eventDate, const QString& memo);
    bool adjustMaterialTo(int materialId, double targetQuantity, const QString& eventDate, const QString& memo);
    bool buildProduct(int productId, int setCount, const QString& eventDate, const QString& memo);

    QList<InventoryEvent> events();
    QString lastError() const;

private:
    bool addMaterialEvent(const QString& type, int materialId, int productId,
                          double quantityDelta, const QString& eventDate, const QString& memo);
    QString newUuid() const;
    QString sourceDevice() const;

    DatabaseManager& m_databaseManager;
    ProductRepository& m_productRepository;
    MaterialRepository& m_materialRepository;
    BomRepository& m_bomRepository;
    InventoryEventRepository& m_eventRepository;
    QString m_lastError;
};
