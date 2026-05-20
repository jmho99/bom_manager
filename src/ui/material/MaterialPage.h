#pragma once

#include <QWidget>

#include "core/services/InventoryService.h"
#include "core/services/MaterialService.h"

class QLineEdit;
class QTableWidget;

class MaterialPage : public QWidget
{
    Q_OBJECT
public:
    MaterialPage(MaterialService& materialService, InventoryService& inventoryService, QWidget* parent = nullptr);

public slots:
    void refresh();

signals:
    void materialDataChanged();
    void inventoryDataChanged();

private slots:
    void addMaterial();
    void editMaterial();
    void deleteMaterial();
    void purchaseSelected();
    void consumeSelected();
    void adjustSelected();

private:
    int selectedMaterialId() const;

    MaterialService& m_materialService;
    InventoryService& m_inventoryService;
    QLineEdit* m_searchEdit;
    QTableWidget* m_table;
};
