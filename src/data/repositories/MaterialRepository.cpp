#include "MaterialRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "utils/DateTimeUtils.h"

MaterialRepository::MaterialRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager)
{
}

QList<Material> MaterialRepository::findAll(const QString& keyword)
{
    QList<Material> materials;
    QString sql = "SELECT id, code, name, unit, location, memo, created_at, updated_at FROM materials";
    const bool hasKeyword = !keyword.trimmed().isEmpty();
    if (hasKeyword) {
        sql += " WHERE code LIKE :keyword OR name LIKE :keyword OR location LIKE :keyword OR memo LIKE :keyword";
    }
    sql += " ORDER BY code ASC";

    QSqlQuery query(m_databaseManager.database());
    query.prepare(sql);
    if (hasKeyword) {
        query.bindValue(":keyword", "%" + keyword.trimmed() + "%");
    }

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return materials;
    }
    while (query.next()) {
        materials.append(fromQuery(query));
    }
    return materials;
}

Material MaterialRepository::findById(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT id, code, name, unit, location, memo, created_at, updated_at FROM materials WHERE id = :id");
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

Material MaterialRepository::findByCode(const QString& code)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT id, code, name, unit, location, memo, created_at, updated_at FROM materials WHERE code = :code");
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

bool MaterialRepository::existsByCode(const QString& code, int excludeId)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("SELECT COUNT(*) FROM materials WHERE code = :code AND id != :exclude_id");
    query.bindValue(":code", code.trimmed());
    query.bindValue(":exclude_id", excludeId);
    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        return true;
    }
    return query.value(0).toInt() > 0;
}

bool MaterialRepository::save(Material& material)
{
    if (material.id < 0) {
        return insert(material);
    }
    return update(material);
}

bool MaterialRepository::upsertByCode(Material& material)
{
    Material existing = findByCode(material.code);
    if (existing.isValid()) {
        material.id = existing.id;
        if (material.name.trimmed().isEmpty()) material.name = existing.name;
        if (material.unit.trimmed().isEmpty()) material.unit = existing.unit;
        if (material.location.trimmed().isEmpty()) material.location = existing.location;
        if (material.memo.trimmed().isEmpty()) material.memo = existing.memo;
        return update(material);
    }
    return insert(material);
}

bool MaterialRepository::insert(Material& material)
{
    const QString now = DateTimeUtils::nowIsoString();
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        INSERT INTO materials (code, name, unit, location, memo, created_at, updated_at)
        VALUES (:code, :name, :unit, :location, :memo, :created_at, :updated_at)
    )");
    query.bindValue(":code", material.code.trimmed());
    query.bindValue(":name", material.name.trimmed());
    query.bindValue(":unit", material.unit.trimmed().isEmpty() ? "ea" : material.unit.trimmed());
    query.bindValue(":location", material.location.trimmed());
    query.bindValue(":memo", material.memo.trimmed());
    query.bindValue(":created_at", now);
    query.bindValue(":updated_at", now);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    material.id = query.lastInsertId().toInt();
    material.createdAt = now;
    material.updatedAt = now;
    return true;
}

bool MaterialRepository::update(const Material& material)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare(R"(
        UPDATE materials
        SET code = :code, name = :name, unit = :unit, location = :location,
            memo = :memo, updated_at = :updated_at
        WHERE id = :id
    )");
    query.bindValue(":code", material.code.trimmed());
    query.bindValue(":name", material.name.trimmed());
    query.bindValue(":unit", material.unit.trimmed().isEmpty() ? "ea" : material.unit.trimmed());
    query.bindValue(":location", material.location.trimmed());
    query.bindValue(":memo", material.memo.trimmed());
    query.bindValue(":updated_at", DateTimeUtils::nowIsoString());
    query.bindValue(":id", material.id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool MaterialRepository::remove(int id)
{
    QSqlQuery query(m_databaseManager.database());
    query.prepare("DELETE FROM materials WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

Material MaterialRepository::fromQuery(const QSqlQuery& query) const
{
    Material material;
    material.id = query.value("id").toInt();
    material.code = query.value("code").toString();
    material.name = query.value("name").toString();
    material.unit = query.value("unit").toString();
    material.location = query.value("location").toString();
    material.memo = query.value("memo").toString();
    material.createdAt = query.value("created_at").toString();
    material.updatedAt = query.value("updated_at").toString();
    return material;
}

QString MaterialRepository::lastError() const
{
    return m_lastError;
}
