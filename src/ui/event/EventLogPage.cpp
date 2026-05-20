#include "EventLogPage.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "utils/MessageBoxHelper.h"

EventLogPage::EventLogPage(InventoryService& inventoryService,
                           LogImportExportService& logService,
                           QWidget* parent)
    : QWidget(parent),
      m_inventoryService(inventoryService),
      m_logService(logService)
{
    auto* refreshButton = new QPushButton("새로고침", this);
    auto* exportButton = new QPushButton("JSONL 내보내기", this);
    auto* importButton = new QPushButton("JSONL 불러오기", this);

    auto* topLayout = new QHBoxLayout;
    topLayout->addStretch(1);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(exportButton);
    topLayout->addWidget(importButton);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(12);
    m_table->setHorizontalHeaderLabels({
        "ID", "일자", "타입", "재료 코드", "재료명", "변동 수량", "단위",
        "제품 코드", "제품명", "기기", "메모", "UUID"
    });
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_table);

    connect(refreshButton, &QPushButton::clicked, this, &EventLogPage::refresh);
    connect(exportButton, &QPushButton::clicked, this, &EventLogPage::exportJsonl);
    connect(importButton, &QPushButton::clicked, this, &EventLogPage::importJsonl);

    refresh();
}

void EventLogPage::refresh()
{
    const QList<InventoryEvent> events = m_inventoryService.events();
    m_table->setRowCount(events.size());
    for (int row = 0; row < events.size(); ++row) {
        const InventoryEvent& e = events[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(e.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(e.eventDate));
        m_table->setItem(row, 2, new QTableWidgetItem(e.eventType));
        m_table->setItem(row, 3, new QTableWidgetItem(e.materialCode));
        m_table->setItem(row, 4, new QTableWidgetItem(e.materialName));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::number(e.quantityDelta, 'f', 3)));
        m_table->setItem(row, 6, new QTableWidgetItem(e.materialUnit));
        m_table->setItem(row, 7, new QTableWidgetItem(e.productCode));
        m_table->setItem(row, 8, new QTableWidgetItem(e.productName));
        m_table->setItem(row, 9, new QTableWidgetItem(e.sourceDevice));
        m_table->setItem(row, 10, new QTableWidgetItem(e.memo));
        m_table->setItem(row, 11, new QTableWidgetItem(e.eventUuid));
    }
    m_table->resizeColumnsToContents();
}

void EventLogPage::exportJsonl()
{
    const QString path = QFileDialog::getSaveFileName(this, "JSONL 내보내기", "inventory_log.jsonl", "JSONL Files (*.jsonl);;All Files (*)");
    if (path.isEmpty()) return;
    if (!m_logService.exportJsonl(path)) {
        MessageBoxHelper::error(this, m_logService.lastError());
        return;
    }
    MessageBoxHelper::info(this, "내보내기가 완료되었습니다.");
}

void EventLogPage::importJsonl()
{
    const QString path = QFileDialog::getOpenFileName(this, "JSONL 불러오기", QString(), "JSONL Files (*.jsonl);;All Files (*)");
    if (path.isEmpty()) return;
    int imported = 0;
    int skipped = 0;
    if (!m_logService.importJsonl(path, &imported, &skipped)) {
        MessageBoxHelper::error(this, m_logService.lastError());
        return;
    }
    refresh();
    emit dataImported();
    MessageBoxHelper::info(this, QString("불러오기 완료\n추가 이벤트: %1개\n중복 건너뜀: %2개").arg(imported).arg(skipped));
}
