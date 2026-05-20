#include "AppContext.h"

#include "data/Schema.h"

AppContext::AppContext()
    : m_productRepository(m_databaseManager),
      m_materialRepository(m_databaseManager),
      m_bomRepository(m_databaseManager),
      m_eventRepository(m_databaseManager),
      m_productService(m_productRepository),
      m_materialService(m_materialRepository, m_eventRepository),
      m_bomService(m_bomRepository, m_eventRepository),
      m_inventoryService(m_databaseManager, m_productRepository, m_materialRepository, m_bomRepository, m_eventRepository),
      m_logImportExportService(m_productRepository, m_materialRepository, m_bomRepository, m_eventRepository)
{
}

bool AppContext::initialize()
{
    if (!m_databaseManager.open("inventory_bom.db")) {
        m_lastError = m_databaseManager.lastError();
        return false;
    }

    Schema schema(m_databaseManager);
    if (!schema.createTables()) {
        m_lastError = schema.lastError();
        return false;
    }
    return true;
}

QString AppContext::lastError() const
{
    return m_lastError;
}

ProductService& AppContext::productService() { return m_productService; }
MaterialService& AppContext::materialService() { return m_materialService; }
BomService& AppContext::bomService() { return m_bomService; }
InventoryService& AppContext::inventoryService() { return m_inventoryService; }
LogImportExportService& AppContext::logImportExportService() { return m_logImportExportService; }
