#pragma once

#include <QWidget>

#include "core/services/InventoryService.h"
#include "core/services/LogImportExportService.h"

class QTableWidget;

class EventLogPage : public QWidget
{
    Q_OBJECT
public:
    EventLogPage(InventoryService& inventoryService,
                 LogImportExportService& logService,
                 QWidget* parent = nullptr);

public slots:
    void refresh();

signals:
    void dataImported();

private slots:
    void exportJsonl();
    void importJsonl();

private:
    InventoryService& m_inventoryService;
    LogImportExportService& m_logService;
    QTableWidget* m_table;
};
