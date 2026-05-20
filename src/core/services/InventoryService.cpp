#include "InventoryService.h"

#include <QSqlDatabase>
#include <QSysInfo>
#include <QUuid>

#include "utils/DateTimeUtils.h"

InventoryService::InventoryService(DatabaseManager& databaseManager,
                                   ProductRepository& productRepository,
                                   MaterialRepository& materialRepository,
                                   BomRepository& bomRepository,
                                   InventoryEventRepository& eventRepository)
    : m_databaseManager(databaseManager),
      m_productRepository(productRepository),
      m_materialRepository(materialRepository),
      m_bomRepository(bomRepository),
      m_eventRepository(eventRepository)
{
}

bool InventoryService::purchaseMaterial(int materialId, double quantity, const QString& eventDate, const QString& memo)
{
    if (quantity <= 0.0) {
        m_lastError = "입고 수량은 0보다 커야 합니다.";
        return false;
    }
    return addMaterialEvent("PURCHASE", materialId, -1, quantity, eventDate, memo);
}

bool InventoryService::consumeMaterial(int materialId, double quantity, const QString& eventDate, const QString& memo)
{
    if (quantity <= 0.0) {
        m_lastError = "출고 수량은 0보다 커야 합니다.";
        return false;
    }
    const double stock = m_eventRepository.currentStock(materialId);
    if (stock < quantity) {
        m_lastError = QString("현재 재고가 부족합니다. 현재 재고: %1").arg(stock);
        return false;
    }
    return addMaterialEvent("CONSUME", materialId, -1, -quantity, eventDate, memo);
}

bool InventoryService::adjustMaterialTo(int materialId, double targetQuantity, const QString& eventDate, const QString& memo)
{
    if (targetQuantity < 0.0) {
        m_lastError = "조정 후 수량은 0보다 작을 수 없습니다.";
        return false;
    }
    const double current = m_eventRepository.currentStock(materialId);
    const double delta = targetQuantity - current;
    return addMaterialEvent("ADJUST", materialId, -1, delta, eventDate, memo);
}

bool InventoryService::buildProduct(int productId, int setCount, const QString& eventDate, const QString& memo)
{
    if (productId < 0) {
        m_lastError = "제작할 제품을 선택하세요.";
        return false;
    }
    if (setCount <= 0) {
        m_lastError = "제작 수량은 1 이상이어야 합니다.";
        return false;
    }

    Product product = m_productRepository.findById(productId);
    if (!product.isValid()) {
        m_lastError = "제품을 찾을 수 없습니다.";
        return false;
    }

    const QList<BomItem> bomItems = m_bomRepository.findByProductId(productId);
    if (bomItems.isEmpty()) {
        m_lastError = "등록된 BOM이 없습니다.";
        return false;
    }

    for (const BomItem& item : bomItems) {
        const double required = item.quantityPerSet * setCount;
        const double stock = m_eventRepository.currentStock(item.materialId);
        if (stock < required) {
            m_lastError = QString("재료 부족: %1 / 필요 %2 %3, 현재 %4 %3")
                .arg(item.materialName)
                .arg(required)
                .arg(item.unit)
                .arg(stock);
            return false;
        }
    }

    QSqlDatabase db = m_databaseManager.database();
    if (!db.transaction()) {
        m_lastError = "DB 트랜잭션을 시작하지 못했습니다.";
        return false;
    }

    for (const BomItem& item : bomItems) {
        const double required = item.quantityPerSet * setCount;
        if (!addMaterialEvent("BUILD_USE", item.materialId, productId, -required, eventDate,
                              QString("%1 %2세트 제작 사용. %3")
                                  .arg(product.name)
                                  .arg(setCount)
                                  .arg(memo))) {
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        m_lastError = "DB 트랜잭션 커밋에 실패했습니다.";
        return false;
    }

    return true;
}

bool InventoryService::addMaterialEvent(const QString& type, int materialId, int productId,
                                        double quantityDelta, const QString& eventDate, const QString& memo)
{
    Material material = m_materialRepository.findById(materialId);
    if (!material.isValid()) {
        m_lastError = "재료를 찾을 수 없습니다.";
        return false;
    }

    Product product;
    if (productId >= 0) {
        product = m_productRepository.findById(productId);
        if (!product.isValid()) {
            m_lastError = "제품을 찾을 수 없습니다.";
            return false;
        }
    }

    InventoryEvent event;
    event.eventUuid = newUuid();
    event.eventType = type;
    event.materialId = material.id;
    event.materialCode = material.code;
    event.materialName = material.name;
    event.materialUnit = material.unit;
    event.productId = product.isValid() ? product.id : -1;
    event.productCode = product.isValid() ? product.code : QString();
    event.productName = product.isValid() ? product.name : QString();
    event.quantityDelta = quantityDelta;
    event.eventDate = eventDate.trimmed().isEmpty() ? DateTimeUtils::todayIsoDate() : eventDate.trimmed();
    event.createdAt = DateTimeUtils::nowIsoString();
    event.sourceDevice = sourceDevice();
    event.memo = memo.trimmed();

    if (!m_eventRepository.insert(event)) {
        m_lastError = m_eventRepository.lastError();
        return false;
    }
    return true;
}

QList<InventoryEvent> InventoryService::events()
{
    return m_eventRepository.findAll();
}

QString InventoryService::newUuid() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString InventoryService::sourceDevice() const
{
    return QSysInfo::machineHostName();
}

QString InventoryService::lastError() const
{
    return m_lastError;
}
