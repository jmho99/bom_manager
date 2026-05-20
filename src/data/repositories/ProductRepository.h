#pragma once

#include <QList>
#include <QString>

#include "core/models/Product.h"
#include "data/DatabaseManager.h"

class QSqlQuery;

class ProductRepository
{
public:
    explicit ProductRepository(DatabaseManager& databaseManager);

    QList<Product> findAll(const QString& keyword = QString());
    Product findById(int id);
    Product findByCode(const QString& code);
    bool existsByCode(const QString& code, int excludeId = -1);
    bool save(Product& product);
    bool upsertByCode(Product& product);
    bool remove(int id);
    QString lastError() const;

private:
    Product fromQuery(const QSqlQuery& query) const;
    bool insert(Product& product);
    bool update(const Product& product);

    DatabaseManager& m_databaseManager;
    QString m_lastError;
};
