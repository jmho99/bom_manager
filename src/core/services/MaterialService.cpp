#include "MaterialService.h"

MaterialService::MaterialService(MaterialRepository& materialRepository, InventoryEventRepository& eventRepository)
    : m_materialRepository(materialRepository),
      m_eventRepository(eventRepository)
{
}

QList<Material> MaterialService::materials(const QString& keyword)
{
    return m_materialRepository.findAll(keyword);
}

QList<StockSummary> MaterialService::stockSummaries(const QString& keyword)
{
    const QList<Material> materials = m_materialRepository.findAll(keyword);
    const QHash<int, double> stockMap = m_eventRepository.stockMap();

    QList<StockSummary> summaries;
    for (const Material& material : materials) {
        StockSummary summary;
        summary.materialId = material.id;
        summary.materialCode = material.code;
        summary.materialName = material.name;
        summary.unit = material.unit;
        summary.location = material.location;
        summary.currentQuantity = stockMap.value(material.id, 0.0);
        summaries.append(summary);
    }
    return summaries;
}

Material MaterialService::materialById(int id)
{
    return m_materialRepository.findById(id);
}

double MaterialService::currentStock(int materialId)
{
    return m_eventRepository.currentStock(materialId);
}

bool MaterialService::save(Material& material)
{
    if (material.code.trimmed().isEmpty()) {
        m_lastError = "재료 코드를 입력하세요.";
        return false;
    }
    if (material.name.trimmed().isEmpty()) {
        m_lastError = "재료명을 입력하세요.";
        return false;
    }
    if (m_materialRepository.existsByCode(material.code, material.id)) {
        m_lastError = "이미 존재하는 재료 코드입니다.";
        return false;
    }
    if (!m_materialRepository.save(material)) {
        m_lastError = m_materialRepository.lastError();
        return false;
    }
    return true;
}

bool MaterialService::remove(int id)
{
    if (id < 0) {
        m_lastError = "삭제할 재료가 선택되지 않았습니다.";
        return false;
    }
    if (!m_materialRepository.remove(id)) {
        m_lastError = m_materialRepository.lastError();
        return false;
    }
    return true;
}

QString MaterialService::lastError() const
{
    return m_lastError;
}
