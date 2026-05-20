#pragma once

#include <QWidget>

#include "core/services/BomService.h"
#include "core/services/InventoryService.h"
#include "core/services/MaterialService.h"
#include "core/services/ProductService.h"

class QComboBox;
class QLabel;
class QTableWidget;

class BomPage : public QWidget
{
    Q_OBJECT
public:
    BomPage(ProductService& productService,
            MaterialService& materialService,
            BomService& bomService,
            InventoryService& inventoryService,
            QWidget* parent = nullptr);

public slots:
    void refresh();

signals:
    void inventoryDataChanged();

private slots:
    void refreshBomTable();
    void addBomItem();
    void editBomItem();
    void removeBomItem();
    void buildSelectedProduct();

private:
    int currentProductId() const;
    int selectedBomItemId() const;
    void refreshProductCombo();
    void refreshCapacityLabel();

    ProductService& m_productService;
    MaterialService& m_materialService;
    BomService& m_bomService;
    InventoryService& m_inventoryService;

    QComboBox* m_productCombo;
    QLabel* m_capacityLabel;
    QTableWidget* m_table;
};
