#include "AppContext.h"

#include "data/Schema.h"

#include <QDir>
#include <QStandardPaths>

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
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (dataDir.isEmpty()) {
        m_lastError = "앱 데이터 저장 경로를 찾을 수 없습니다.";
        return false;
    }

    QDir dir(dataDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        m_lastError = "앱 데이터 폴더를 생성하지 못했습니다: " + dataDir;
        return false;
    }

    m_databasePath = dir.filePath("inventory_bom.db");

    if (!m_databaseManager.open(m_databasePath)) {
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

bool AppContext::resetAllData()
{
    Schema schema(m_databaseManager);
    if (!schema.resetAllData()) {
        m_lastError = schema.lastError();
        return false;
    }

    return true;
}

QString AppContext::lastError() const
{
    return m_lastError;
}

QString AppContext::databasePath() const
{
    return m_databasePath;
}

ProductService& AppContext::productService() { return m_productService; }
MaterialService& AppContext::materialService() { return m_materialService; }
BomService& AppContext::bomService() { return m_bomService; }
InventoryService& AppContext::inventoryService() { return m_inventoryService; }
LogImportExportService& AppContext::logImportExportService() { return m_logImportExportService; }
