#pragma once

#include <QList>
#include <QString>

#include "core/models/Product.h"
#include "data/repositories/ProductRepository.h"

class ProductService
{
public:
    explicit ProductService(ProductRepository& productRepository);

    QList<Product> products(const QString& keyword = QString());
    Product productById(int id);
    bool save(Product& product);
    bool remove(int id);
    QString lastError() const;

private:
    ProductRepository& m_productRepository;
    QString m_lastError;
};
