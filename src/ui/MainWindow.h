#pragma once

#include <QMainWindow>

#include "app/AppContext.h"

class BomPage;
class EventLogPage;
class MaterialPage;
class ProductPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(AppContext& context, QWidget* parent = nullptr);

private slots:
    void refreshAll();
    void resetLocalData();
    void showDatabasePath();

private:
    void setupMenu();

private:
    AppContext& m_context;
    ProductPage* m_productPage;
    MaterialPage* m_materialPage;
    BomPage* m_bomPage;
    EventLogPage* m_eventLogPage;
};
