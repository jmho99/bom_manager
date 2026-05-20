#include "InventoryEventRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

InventoryEventRepository::InventoryEventRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager)
{
}

bool InventoryEventRepository::insert(const InventoryEvent& event)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        INSERT INTO inventory_events
            (event_uuid, event_type, material_id, material_code, material_name, material_unit,
             product_id, product_code, product_name, quantity_delta, event_date, created_at,
             source_device, memo)
        VALUES
            (:event_uuid, :event_type, :material_id, :material_code, :material_name, :material_unit,
             :product_id, :product_code, :product_name, :quantity_delta, :event_date, :created_at,
             :source_device, :memo)
    )");
    query.bindValue(":event_uuid", event.eventUuid);
    query.bindValue(":event_type", event.eventType);
    query.bindValue(":material_id", event.materialId >= 0 ? QVariant(event.materialId) : QVariant());
    query.bindValue(":material_code", event.materialCode);
    query.bindValue(":material_name", event.materialName);
    query.bindValue(":material_unit", event.materialUnit);
    query.bindValue(":product_id", event.productId >= 0 ? QVariant(event.productId) : QVariant());
    query.bindValue(":product_code", event.productCode);
    query.bindValue(":product_name", event.productName);
    query.bindValue(":quantity_delta", event.quantityDelta);
    query.bindValue(":event_date", event.eventDate);
    query.bindValue(":created_at", event.createdAt);
    query.bindValue(":source_device", event.sourceDevice);
    query.bindValue(":memo", event.memo);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool InventoryEventRepository::existsByUuid(const QString& eventUuid)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT COUNT(*) FROM inventory_events WHERE event_uuid = :event_uuid");
    query.bindValue(":event_uuid", eventUuid);
    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        return true;
    }
    return query.value(0).toInt() > 0;
}

QList<InventoryEvent> InventoryEventRepository::findAll()
{
    QList<InventoryEvent> events;
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        SELECT id, event_uuid, event_type, material_id, material_code, material_name, material_unit,
               product_id, product_code, product_name, quantity_delta, event_date, created_at,
               source_device, memo
        FROM inventory_events
        ORDER BY event_date DESC, id DESC
    )");
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return events;
    }
    while (query.next()) {
        events.append(fromQuery(query));
    }
    return events;
}

double InventoryEventRepository::currentStock(int materialId)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT COALESCE(SUM(quantity_delta), 0) FROM inventory_events WHERE material_id = :material_id");
    query.bindValue(":material_id", materialId);
    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        return 0.0;
    }
    return query.value(0).toDouble();
}

QHash<int, double> InventoryEventRepository::stockMap()
{
    QHash<int, double> map;
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        SELECT material_id, COALESCE(SUM(quantity_delta), 0) AS stock
        FROM inventory_events
        WHERE material_id IS NOT NULL
        GROUP BY material_id
    )");
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return map;
    }
    while (query.next()) {
        map.insert(query.value("material_id").toInt(), query.value("stock").toDouble());
    }
    return map;
}

InventoryEvent InventoryEventRepository::fromQuery(const QSqlQuery& query) const
{
    InventoryEvent event;
    event.id = query.value("id").toInt();
    event.eventUuid = query.value("event_uuid").toString();
    event.eventType = query.value("event_type").toString();
    event.materialId = query.value("material_id").isNull() ? -1 : query.value("material_id").toInt();
    event.materialCode = query.value("material_code").toString();
    event.materialName = query.value("material_name").toString();
    event.materialUnit = query.value("material_unit").toString();
    event.productId = query.value("product_id").isNull() ? -1 : query.value("product_id").toInt();
    event.productCode = query.value("product_code").toString();
    event.productName = query.value("product_name").toString();
    event.quantityDelta = query.value("quantity_delta").toDouble();
    event.eventDate = query.value("event_date").toString();
    event.createdAt = query.value("created_at").toString();
    event.sourceDevice = query.value("source_device").toString();
    event.memo = query.value("memo").toString();
    return event;
}

QString InventoryEventRepository::lastError() const
{
    return m_lastError;
}
