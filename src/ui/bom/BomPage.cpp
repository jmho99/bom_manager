#include "BomPage.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "ui/bom/BomItemDialog.h"
#include "ui/common/BuildDialog.h"
#include "utils/MessageBoxHelper.h"

BomPage::BomPage(ProductService& productService,
                 MaterialService& materialService,
                 BomService& bomService,
                 InventoryService& inventoryService,
                 QWidget* parent)
    : QWidget(parent),
      m_productService(productService),
      m_materialService(materialService),
      m_bomService(bomService),
      m_inventoryService(inventoryService)
{
    m_productCombo = new QComboBox(this);
    m_capacityLabel = new QLabel(this);
    auto* addButton = new QPushButton("BOM 항목 추가/수정", this);
    auto* editButton = new QPushButton("선택 항목 수정", this);
    auto* removeButton = new QPushButton("선택 항목 삭제", this);
    auto* buildButton = new QPushButton("선택 제품 제작 처리", this);

    auto* topLayout = new QHBoxLayout;
    topLayout->addWidget(new QLabel("제품", this));
    topLayout->addWidget(m_productCombo, 1);
    topLayout->addWidget(m_capacityLabel, 2);
    topLayout->addWidget(addButton);
    topLayout->addWidget(editButton);
    topLayout->addWidget(removeButton);
    topLayout->addWidget(buildButton);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"BOM ID", "재료 ID", "재료 코드", "재료명", "세트당 필요", "단위", "현재 재고"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_table);

    connect(m_productCombo, &QComboBox::currentIndexChanged, this, &BomPage::refreshBomTable);
    connect(addButton, &QPushButton::clicked, this, &BomPage::addBomItem);
    connect(editButton, &QPushButton::clicked, this, &BomPage::editBomItem);
    connect(removeButton, &QPushButton::clicked, this, &BomPage::removeBomItem);
    connect(buildButton, &QPushButton::clicked, this, &BomPage::buildSelectedProduct);

    refresh();
}

void BomPage::refresh()
{
    const int previousProductId = currentProductId();
    refreshProductCombo();
    const int idx = m_productCombo->findData(previousProductId);
    if (idx >= 0) m_productCombo->setCurrentIndex(idx);
    refreshBomTable();
}

void BomPage::refreshProductCombo()
{
    m_productCombo->blockSignals(true);
    m_productCombo->clear();
    for (const Product& product : m_productService.products()) {
        m_productCombo->addItem(QString("%1 - %2").arg(product.code, product.name), product.id);
    }
    m_productCombo->blockSignals(false);
}

int BomPage::currentProductId() const
{
    if (m_productCombo->currentIndex() < 0) return -1;
    return m_productCombo->currentData().toInt();
}

int BomPage::selectedBomItemId() const
{
    const auto selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;
    return m_table->item(selected.first()->row(), 0)->text().toInt();
}

void BomPage::refreshBomTable()
{
    const int productId = currentProductId();
    const QList<BomItem> items = productId >= 0 ? m_bomService.bomItems(productId) : QList<BomItem>();
    m_table->setRowCount(items.size());
    for (int row = 0; row < items.size(); ++row) {
        const BomItem& item = items[row];
        const double stock = m_materialService.currentStock(item.materialId);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(item.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(item.materialId)));
        m_table->setItem(row, 2, new QTableWidgetItem(item.materialCode));
        m_table->setItem(row, 3, new QTableWidgetItem(item.materialName));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(item.quantityPerSet, 'f', 3)));
        m_table->setItem(row, 5, new QTableWidgetItem(item.unit));
        m_table->setItem(row, 6, new QTableWidgetItem(QString::number(stock, 'f', 3)));
    }
    m_table->resizeColumnsToContents();
    refreshCapacityLabel();
}

void BomPage::refreshCapacityLabel()
{
    QString limiter;
    const int capacity = m_bomService.buildCapacity(currentProductId(), &limiter);
    m_capacityLabel->setText(QString("제작 가능: %1 세트 / 제한 재료: %2").arg(capacity).arg(limiter));
}

void BomPage::addBomItem()
{
    const int productId = currentProductId();
    if (productId < 0) {
        MessageBoxHelper::error(this, "제품을 먼저 등록/선택하세요.");
        return;
    }
    const QList<Material> materials = m_materialService.materials();
    if (materials.isEmpty()) {
        MessageBoxHelper::error(this, "재료를 먼저 등록하세요.");
        return;
    }
    BomItemDialog dialog(materials, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_bomService.saveBomItem(productId, dialog.materialId(), dialog.quantityPerSet())) {
        MessageBoxHelper::error(this, m_bomService.lastError());
        return;
    }
    refreshBomTable();
}

void BomPage::editBomItem()
{
    const int bomId = selectedBomItemId();
    if (bomId < 0) {
        MessageBoxHelper::error(this, "수정할 BOM 항목을 선택하세요.");
        return;
    }
    const int row = m_table->selectedItems().first()->row();
    const int materialId = m_table->item(row, 1)->text().toInt();
    const double quantity = m_table->item(row, 4)->text().toDouble();
    BomItemDialog dialog(m_materialService.materials(), this);
    dialog.setInitial(materialId, quantity);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_bomService.saveBomItem(currentProductId(), dialog.materialId(), dialog.quantityPerSet())) {
        MessageBoxHelper::error(this, m_bomService.lastError());
        return;
    }
    refreshBomTable();
}

void BomPage::removeBomItem()
{
    const int bomId = selectedBomItemId();
    if (bomId < 0) {
        MessageBoxHelper::error(this, "삭제할 BOM 항목을 선택하세요.");
        return;
    }
    if (!MessageBoxHelper::confirm(this, "선택한 BOM 항목을 삭제하시겠습니까?")) return;
    if (!m_bomService.removeBomItem(bomId)) {
        MessageBoxHelper::error(this, m_bomService.lastError());
        return;
    }
    refreshBomTable();
}

void BomPage::buildSelectedProduct()
{
    const int productId = currentProductId();
    if (productId < 0) {
        MessageBoxHelper::error(this, "제작할 제품을 선택하세요.");
        return;
    }
    BuildDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_inventoryService.buildProduct(productId, dialog.setCount(), dialog.eventDate(), dialog.memo())) {
        MessageBoxHelper::error(this, m_inventoryService.lastError());
        return;
    }
    refreshBomTable();
    emit inventoryDataChanged();
}
