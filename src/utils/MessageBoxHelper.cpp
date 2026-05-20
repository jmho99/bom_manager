#include "MessageBoxHelper.h"

#include <QMessageBox>

void MessageBoxHelper::error(QWidget* parent, const QString& message)
{
    QMessageBox::critical(parent, "오류", message);
}

void MessageBoxHelper::info(QWidget* parent, const QString& message)
{
    QMessageBox::information(parent, "알림", message);
}

bool MessageBoxHelper::confirm(QWidget* parent, const QString& message)
{
    return QMessageBox::question(parent, "확인", message) == QMessageBox::Yes;
}
