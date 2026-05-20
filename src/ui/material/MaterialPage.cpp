#include "MaterialPage.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "ui/common/StockEventDialog.h"
#include "ui/material/MaterialDialog.h"
#include "utils/MessageBoxHelper.h"

MaterialPage::MaterialPage(MaterialService& materialService, InventoryService& inventoryService, QWidget* parent)
    : QWidget(parent),
      m_materialService(materialService),
      m_inventoryService(inventoryService)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("재료 코드, 재료명, 위치 검색");
    auto* searchButton = new QPushButton("검색", this);
    auto* addButton = new QPushButton("재료 등록", this);
    auto* editButton = new QPushButton("수정", this);
    auto* deleteButton = new QPushButton("삭제", this);
    auto* purchaseButton = new QPushButton("구매 입고", this);
    auto* consumeButton = new QPushButton("사용 출고", this);
    auto* adjustButton = new QPushButton("재고 조정", this);

    auto* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_searchEdit, 1);
    topLayout->addWidget(searchButton);
    topLayout->addWidget(addButton);
    topLayout->addWidget(editButton);
    topLayout->addWidget(deleteButton);
    topLayout->addWidget(purchaseButton);
    topLayout->addWidget(consumeButton);
    topLayout->addWidget(adjustButton);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"ID", "재료 코드", "재료명", "현재 재고", "단위", "위치", "메모"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_table);

    connect(searchButton, &QPushButton::clicked, this, &MaterialPage::refresh);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &MaterialPage::refresh);
    connect(addButton, &QPushButton::clicked, this, &MaterialPage::addMaterial);
    connect(editButton, &QPushButton::clicked, this, &MaterialPage::editMaterial);
    connect(deleteButton, &QPushButton::clicked, this, &MaterialPage::deleteMaterial);
    connect(purchaseButton, &QPushButton::clicked, this, &MaterialPage::purchaseSelected);
    connect(consumeButton, &QPushButton::clicked, this, &MaterialPage::consumeSelected);
    connect(adjustButton, &QPushButton::clicked, this, &MaterialPage::adjustSelected);

    refresh();
}

void MaterialPage::refresh()
{
    const QList<StockSummary> summaries = m_materialService.stockSummaries(m_searchEdit->text());
    m_table->setRowCount(summaries.size());
    for (int row = 0; row < summaries.size(); ++row) {
        const StockSummary& s = summaries[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(s.materialId)));
        m_table->setItem(row, 1, new QTableWidgetItem(s.materialCode));
        m_table->setItem(row, 2, new QTableWidgetItem(s.materialName));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(s.currentQuantity, 'f', 3)));
        m_table->setItem(row, 4, new QTableWidgetItem(s.unit));
        m_table->setItem(row, 5, new QTableWidgetItem(s.location));
        m_table->setItem(row, 6, new QTableWidgetItem(""));
    }
    m_table->resizeColumnsToContents();
}

int MaterialPage::selectedMaterialId() const
{
    const auto selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;
    return m_table->item(selected.first()->row(), 0)->text().toInt();
}

void MaterialPage::addMaterial()
{
    MaterialDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    Material material = dialog.material();
    if (!m_materialService.save(material)) {
        MessageBoxHelper::error(this, m_materialService.lastError());
        return;
    }
    refresh();
    emit materialDataChanged();
}

void MaterialPage::editMaterial()
{
    const int id = selectedMaterialId();
    if (id < 0) {
        MessageBoxHelper::error(this, "수정할 재료를 선택하세요.");
        return;
    }
    Material material = m_materialService.materialById(id);
    MaterialDialog dialog(this);
    dialog.setMaterial(material);
    if (dialog.exec() != QDialog::Accepted) return;
    Material edited = dialog.material();
    if (!m_materialService.save(edited)) {
        MessageBoxHelper::error(this, m_materialService.lastError());
        return;
    }
    refresh();
    emit materialDataChanged();
}

void MaterialPage::deleteMaterial()
{
    const int id = selectedMaterialId();
    if (id < 0) {
        MessageBoxHelper::error(this, "삭제할 재료를 선택하세요.");
        return;
    }
    if (!MessageBoxHelper::confirm(this, "선택한 재료를 삭제하시겠습니까? BOM도 함께 삭제됩니다.")) return;
    if (!m_materialService.remove(id)) {
        MessageBoxHelper::error(this, m_materialService.lastError());
        return;
    }
    refresh();
    emit materialDataChanged();
}

void MaterialPage::purchaseSelected()
{
    const int id = selectedMaterialId();
    if (id < 0) {
        MessageBoxHelper::error(this, "입고할 재료를 선택하세요.");
        return;
    }
    StockEventDialog dialog("구매 입고", "입고 수량", this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_inventoryService.purchaseMaterial(id, dialog.quantity(), dialog.eventDate(), dialog.memo())) {
        MessageBoxHelper::error(this, m_inventoryService.lastError());
        return;
    }
    refresh();
    emit inventoryDataChanged();
}

void MaterialPage::consumeSelected()
{
    const int id = selectedMaterialId();
    if (id < 0) {
        MessageBoxHelper::error(this, "출고할 재료를 선택하세요.");
        return;
    }
    StockEventDialog dialog("사용 출고", "출고 수량", this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_inventoryService.consumeMaterial(id, dialog.quantity(), dialog.eventDate(), dialog.memo())) {
        MessageBoxHelper::error(this, m_inventoryService.lastError());
        return;
    }
    refresh();
    emit inventoryDataChanged();
}

void MaterialPage::adjustSelected()
{
    const int id = selectedMaterialId();
    if (id < 0) {
        MessageBoxHelper::error(this, "조정할 재료를 선택하세요.");
        return;
    }
    StockEventDialog dialog("재고 조정", "조정 후 수량", this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_inventoryService.adjustMaterialTo(id, dialog.quantity(), dialog.eventDate(), dialog.memo())) {
        MessageBoxHelper::error(this, m_inventoryService.lastError());
        return;
    }
    refresh();
    emit inventoryDataChanged();
}
