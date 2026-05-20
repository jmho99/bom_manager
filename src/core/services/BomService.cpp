#include "BomService.h"

#include <cmath>
#include <limits>

BomService::BomService(BomRepository& bomRepository, InventoryEventRepository& eventRepository)
    : m_bomRepository(bomRepository),
      m_eventRepository(eventRepository)
{
}

QList<BomItem> BomService::bomItems(int productId)
{
    return m_bomRepository.findByProductId(productId);
}

QList<BomItem> BomService::allBomItems()
{
    return m_bomRepository.findAll();
}

int BomService::buildCapacity(int productId, QString* limitingMaterialText)
{
    const QList<BomItem> items = m_bomRepository.findByProductId(productId);
    if (items.isEmpty()) {
        if (limitingMaterialText) *limitingMaterialText = "BOM 없음";
        return 0;
    }

    int capacity = std::numeric_limits<int>::max();
    QString limiter;

    for (const BomItem& item : items) {
        if (item.quantityPerSet <= 0.0) {
            if (limitingMaterialText) *limitingMaterialText = "필요 수량 오류";
            return 0;
        }
        const double stock = m_eventRepository.currentStock(item.materialId);
        const int possible = static_cast<int>(std::floor(stock / item.quantityPerSet));
        if (possible < capacity) {
            capacity = possible;
            limiter = QString("%1 (%2 / 세트당 %3 %4)")
                .arg(item.materialName)
                .arg(stock)
                .arg(item.quantityPerSet)
                .arg(item.unit);
        }
    }

    if (capacity == std::numeric_limits<int>::max()) {
        capacity = 0;
    }
    if (limitingMaterialText) *limitingMaterialText = limiter;
    return capacity;
}

bool BomService::saveBomItem(int productId, int materialId, double quantityPerSet)
{
    if (productId < 0) {
        m_lastError = "제품을 선택하세요.";
        return false;
    }
    if (materialId < 0) {
        m_lastError = "재료를 선택하세요.";
        return false;
    }
    if (quantityPerSet <= 0.0) {
        m_lastError = "1세트당 필요 수량은 0보다 커야 합니다.";
        return false;
    }
    if (!m_bomRepository.upsert(productId, materialId, quantityPerSet)) {
        m_lastError = m_bomRepository.lastError();
        return false;
    }
    return true;
}

bool BomService::removeBomItem(int bomItemId)
{
    if (bomItemId < 0) {
        m_lastError = "삭제할 BOM 항목이 선택되지 않았습니다.";
        return false;
    }
    if (!m_bomRepository.remove(bomItemId)) {
        m_lastError = m_bomRepository.lastError();
        return false;
    }
    return true;
}

QString BomService::lastError() const
{
    return m_lastError;
}
