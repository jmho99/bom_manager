#include "MainWindow.h"

#include <QTabWidget>

#include "ui/bom/BomPage.h"
#include "ui/event/EventLogPage.h"
#include "ui/material/MaterialPage.h"
#include "ui/product/ProductPage.h"

MainWindow::MainWindow(AppContext& context, QWidget* parent)
    : QMainWindow(parent),
      m_context(context)
{
    setWindowTitle("BOM 기반 범용 재고 관리 프로그램");
    resize(1280, 760);

    auto* tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    m_productPage = new ProductPage(m_context.productService(), this);
    m_materialPage = new MaterialPage(m_context.materialService(), m_context.inventoryService(), this);
    m_bomPage = new BomPage(m_context.productService(), m_context.materialService(), m_context.bomService(), m_context.inventoryService(), this);
    m_eventLogPage = new EventLogPage(m_context.inventoryService(), m_context.logImportExportService(), this);

    tabs->addTab(m_productPage, "제품 관리");
    tabs->addTab(m_materialPage, "재료 / 재고");
    tabs->addTab(m_bomPage, "BOM / 제작 가능 수량");
    tabs->addTab(m_eventLogPage, "로그 / 동기화");

    connect(m_productPage, &ProductPage::productDataChanged, this, &MainWindow::refreshAll);
    connect(m_materialPage, &MaterialPage::materialDataChanged, this, &MainWindow::refreshAll);
    connect(m_materialPage, &MaterialPage::inventoryDataChanged, this, &MainWindow::refreshAll);
    connect(m_bomPage, &BomPage::inventoryDataChanged, this, &MainWindow::refreshAll);
    connect(m_eventLogPage, &EventLogPage::dataImported, this, &MainWindow::refreshAll);
}

void MainWindow::refreshAll()
{
    m_productPage->refresh();
    m_materialPage->refresh();
    m_bomPage->refresh();
    m_eventLogPage->refresh();
}
