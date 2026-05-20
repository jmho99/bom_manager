#include <QApplication>
#include <QMessageBox>

#include "app/AppContext.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Inventory BOM Manager");
    app.setOrganizationName("Local");

    AppContext context;
    if (!context.initialize()) {
        QMessageBox::critical(nullptr, "초기화 오류", context.lastError());
        return 1;
    }

    MainWindow window(context);
    window.show();

    return app.exec();
}
