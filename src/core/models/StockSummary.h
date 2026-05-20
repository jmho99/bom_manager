#pragma once

#include <QString>

struct StockSummary
{
    int materialId = -1;
    QString materialCode;
    QString materialName;
    QString unit;
    QString location;
    double currentQuantity = 0.0;
};
