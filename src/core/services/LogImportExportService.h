#pragma once

#include <QString>

#include "data/repositories/BomRepository.h"
#include "data/repositories/InventoryEventRepository.h"
#include "data/repositories/MaterialRepository.h"
#include "data/repositories/ProductRepository.h"

class LogImportExportService
{
public:
    LogImportExportService(ProductRepository& productRepository,
                           MaterialRepository& materialRepository,
                           BomRepository& bomRepository,
                           InventoryEventRepository& eventRepository);

    bool exportJsonl(const QString& filePath);
    bool importJsonl(const QString& filePath, int* importedEventCount = nullptr, int* skippedEventCount = nullptr);
    QString lastError() const;

private:
    ProductRepository& m_productRepository;
    MaterialRepository& m_materialRepository;
    BomRepository& m_bomRepository;
    InventoryEventRepository& m_eventRepository;
    QString m_lastError;
};
