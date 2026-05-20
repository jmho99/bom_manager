#include "BomRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "utils/DateTimeUtils.h"

BomRepository::BomRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager)
{
}

QList<BomItem> BomRepository::findAll()
{
    QList<BomItem> items;
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        SELECT b.id, b.product_id, b.material_id, b.quantity_per_set,
               p.code AS product_code, p.name AS product_name,
               m.code AS material_code, m.name AS material_name, m.unit AS unit
        FROM bom_items b
        JOIN products p ON p.id = b.product_id
        JOIN materials m ON m.id = b.material_id
        ORDER BY p.code ASC, m.code ASC
    )");
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return items;
    }
    while (query.next()) {
        items.append(fromQuery(query));
    }
    return items;
}

QList<BomItem> BomRepository::findByProductId(int productId)
{
    QList<BomItem> items;
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        SELECT b.id, b.product_id, b.material_id, b.quantity_per_set,
               p.code AS product_code, p.name AS product_name,
               m.code AS material_code, m.name AS material_name, m.unit AS unit
        FROM bom_items b
        JOIN products p ON p.id = b.product_id
        JOIN materials m ON m.id = b.material_id
        WHERE b.product_id = :product_id
        ORDER BY m.code ASC
    )");
    query.bindValue(":product_id", productId);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return items;
    }
    while (query.next()) {
        items.append(fromQuery(query));
    }
    return items;
}

BomItem BomRepository::findById(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        SELECT b.id, b.product_id, b.material_id, b.quantity_per_set,
               p.code AS product_code, p.name AS product_name,
               m.code AS material_code, m.name AS material_name, m.unit AS unit
        FROM bom_items b
        JOIN products p ON p.id = b.product_id
        JOIN materials m ON m.id = b.material_id
        WHERE b.id = :id
    )");
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return {};
    }
    if (!query.next()) {
        return {};
    }
    return fromQuery(query);
}

bool BomRepository::upsert(int productId, int materialId, double quantityPerSet)
{
    const QString now = DateTimeUtils::nowIsoString();
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        INSERT INTO bom_items (product_id, material_id, quantity_per_set, created_at, updated_at)
        VALUES (:product_id, :material_id, :quantity_per_set, :created_at, :updated_at)
        ON CONFLICT(product_id, material_id)
        DO UPDATE SET quantity_per_set = excluded.quantity_per_set,
                      updated_at = excluded.updated_at
    )");
    query.bindValue(":product_id", productId);
    query.bindValue(":material_id", materialId);
    query.bindValue(":quantity_per_set", quantityPerSet);
    query.bindValue(":created_at", now);
    query.bindValue(":updated_at", now);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool BomRepository::remove(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("DELETE FROM bom_items WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

BomItem BomRepository::fromQuery(const QSqlQuery& query) const
{
    BomItem item;
    item.id = query.value("id").toInt();
    item.productId = query.value("product_id").toInt();
    item.materialId = query.value("material_id").toInt();
    item.quantityPerSet = query.value("quantity_per_set").toDouble();
    item.productCode = query.value("product_code").toString();
    item.productName = query.value("product_name").toString();
    item.materialCode = query.value("material_code").toString();
    item.materialName = query.value("material_name").toString();
    item.unit = query.value("unit").toString();
    return item;
}

QString BomRepository::lastError() const
{
    return m_lastError;
}
