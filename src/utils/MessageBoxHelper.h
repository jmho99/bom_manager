#pragma once

#include <QString>
#include <QWidget>

class MessageBoxHelper
{
public:
    static void showError(QWidget* parent, const QString& message);
    static void showInfo(QWidget* parent, const QString& message);
    static bool confirm(QWidget* parent, const QString& message);

    // 기존 코드에서 error/info 이름을 쓰고 있었다면 호환용으로 유지
    static void error(QWidget* parent, const QString& message);
    static void info(QWidget* parent, const QString& message);
};