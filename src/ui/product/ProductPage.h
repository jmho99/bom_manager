#pragma once

#include <QWidget>

#include "core/services/ProductService.h"

class QLineEdit;
class QPushButton;
class QTableWidget;

class ProductPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProductPage(ProductService& productService, QWidget* parent = nullptr);

public slots:
    void refresh();

signals:
    void productDataChanged();

private slots:
    void addProduct();
    void editProduct();
    void deleteProduct();

private:
    int selectedProductId() const;

    ProductService& m_productService;
    QLineEdit* m_searchEdit;
    QTableWidget* m_table;
};
