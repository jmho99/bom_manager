#include "ProductRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "utils/DateTimeUtils.h"

ProductRepository::ProductRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager)
{
}

QList<Product> ProductRepository::findAll(const QString& keyword)
{
    QList<Product> products;
    QString sql = "SELECT id, code, name, memo, created_at, updated_at FROM products";
    const bool hasKeyword = !keyword.trimmed().isEmpty();
    if (hasKeyword) {
        sql += " WHERE code LIKE :keyword OR name LIKE :keyword OR memo LIKE :keyword";
    }
    sql += " ORDER BY code ASC";

    QSqlQuery query(m_databaseManager.database());
    query.prepare(sql);
    if (hasKeyword) {
        query.bindValue(":keyword", "%" + keyword.trimmed() + "%");
    }

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return products;
    }

    while (query.next()) {
        products.append(fromQuery(query));
    }
    return products;
}

Product ProductRepository::findById(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT id, code, name, memo, created_at, updated_at FROM products WHERE id = :id");
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

Product ProductRepository::findByCode(const QString& code)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT id, code, name, memo, created_at, updated_at FROM products WHERE code = :code");
    query.bindValue(":code", code.trimmed());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return {};
    }
    if (!query.next()) {
        return {};
    }
    return fromQuery(query);
}

bool ProductRepository::existsByCode(const QString& code, int excludeId)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT COUNT(*) FROM products WHERE code = :code AND id != :exclude_id");
    query.bindValue(":code", code.trimmed());
    query.bindValue(":exclude_id", excludeId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        return true;
    }
    return query.value(0).toInt() > 0;
}

bool ProductRepository::save(Product& product)
{
    if (product.id < 0) {
        return insert(product);
    }
    return update(product);
}

bool ProductRepository::upsertByCode(Product& product)
{
    Product existing = findByCode(product.code);
    if (existing.isValid()) {
        product.id = existing.id;
        if (product.name.trimmed().isEmpty()) product.name = existing.name;
        if (product.memo.trimmed().isEmpty()) product.memo = existing.memo;
        return update(product);
    }
    return insert(product);
}

bool ProductRepository::insert(Product& product)
{
    const QString now = DateTimeUtils::nowIsoString();
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        INSERT INTO products (code, name, memo, created_at, updated_at)
        VALUES (:code, :name, :memo, :created_at, :updated_at)
    )");
    query.bindValue(":code", product.code.trimmed());
    query.bindValue(":name", product.name.trimmed());
    query.bindValue(":memo", product.memo.trimmed());
    query.bindValue(":created_at", now);
    query.bindValue(":updated_at", now);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    product.id = query.lastInsertId().toInt();
    product.createdAt = now;
    product.updatedAt = now;
    return true;
}

bool ProductRepository::update(const Product& product)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        UPDATE products
        SET code = :code, name = :name, memo = :memo, updated_at = :updated_at
        WHERE id = :id
    )");
    query.bindValue(":code", product.code.trimmed());
    query.bindValue(":name", product.name.trimmed());
    query.bindValue(":memo", product.memo.trimmed());
    query.bindValue(":updated_at", DateTimeUtils::nowIsoString());
    query.bindValue(":id", product.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool ProductRepository::remove(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("DELETE FROM products WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

Product ProductRepository::fromQuery(const QSqlQuery& query) const
{
    Product product;
    product.id = query.value("id").toInt();
    product.code = query.value("code").toString();
    product.name = query.value("name").toString();
    product.memo = query.value("memo").toString();
    product.createdAt = query.value("created_at").toString();
    product.updatedAt = query.value("updated_at").toString();
    return product;
}

QString ProductRepository::lastError() const
{
    return m_lastError;
}
