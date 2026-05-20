#pragma once

#include <QString>
#include <QWidget>

class MessageBoxHelper
{
public:
    static void error(QWidget* parent, const QString& message);
    static void info(QWidget* parent, const QString& message);
    static bool confirm(QWidget* parent, const QString& message);
};
