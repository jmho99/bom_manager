#pragma once

#include <QString>

#include "core/services/BomService.h"
#include "core/services/InventoryService.h"
#include "core/services/LogImportExportService.h"
#include "core/services/MaterialService.h"
#include "core/services/ProductService.h"
#include "data/DatabaseManager.h"
#include "data/repositories/BomRepository.h"
#include "data/repositories/InventoryEventRepository.h"
#include "data/repositories/MaterialRepository.h"
#include "data/repositories/ProductRepository.h"

class AppContext
{
public:
    AppContext();

    bool initialize();
    QString lastError() const;

    ProductService& productService();
    MaterialService& materialService();
    BomService& bomService();
    InventoryService& inventoryService();
    LogImportExportService& logImportExportService();

private:
    QString m_lastError;

    DatabaseManager m_databaseManager;
    ProductRepository m_productRepository;
    MaterialRepository m_materialRepository;
    BomRepository m_bomRepository;
    InventoryEventRepository m_eventRepository;

    ProductService m_productService;
    MaterialService m_materialService;
    BomService m_bomService;
    InventoryService m_inventoryService;
    LogImportExportService m_logImportExportService;
};
