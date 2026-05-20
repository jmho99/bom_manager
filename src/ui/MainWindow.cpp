#include "MainWindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabWidget>

#include "ui/bom/BomPage.h"
#include "ui/event/EventLogPage.h"
#include "ui/material/MaterialPage.h"
#include "ui/product/ProductPage.h"
#include "utils/MessageBoxHelper.h"

MainWindow::MainWindow(AppContext& context, QWidget* parent)
    : QMainWindow(parent),
      m_context(context)
{
    setWindowTitle("BOM 기반 범용 재고 관리 프로그램");
    resize(1280, 760);

    setupMenu();

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

void MainWindow::setupMenu()
{
    QMenu* dataMenu = menuBar()->addMenu("데이터(&D)");

    QAction* refreshAction = dataMenu->addAction("새로고침");
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshAll);

    QAction* showPathAction = dataMenu->addAction("DB 저장 위치 보기");
    connect(showPathAction, &QAction::triggered, this, &MainWindow::showDatabasePath);

    dataMenu->addSeparator();

    QAction* resetAction = dataMenu->addAction("로컬 데이터 초기화...");
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetLocalData);
}

void MainWindow::resetLocalData()
{
    const QMessageBox::StandardButton firstAnswer = QMessageBox::warning(
        this,
        "로컬 데이터 초기화",
        "현재 PC의 제품, 재료, BOM, 입출고 로그를 모두 삭제합니다.\n\n"
        "이미 JSONL로 내보낸 로그 파일은 삭제하지 않습니다.\n"
        "계속하시겠습니까?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (firstAnswer != QMessageBox::Yes) {
        return;
    }

    const QMessageBox::StandardButton secondAnswer = QMessageBox::question(
        this,
        "초기화 재확인",
        "정말 초기화하시겠습니까? 이 작업은 되돌릴 수 없습니다.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (secondAnswer != QMessageBox::Yes) {
        return;
    }

    if (!m_context.resetAllData()) {
        MessageBoxHelper::showError(this, m_context.lastError());
        return;
    }

    refreshAll();
    MessageBoxHelper::showInfo(this, "로컬 데이터가 초기화되었습니다.");
}

void MainWindow::showDatabasePath()
{
    QMessageBox::information(
        this,
        "DB 저장 위치",
        "현재 로컬 DB 저장 위치:\n" + m_context.databasePath()
    );
}
