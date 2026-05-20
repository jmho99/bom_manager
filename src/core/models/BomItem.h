#pragma once

#include <QString>

struct BomItem
{
    int id = -1;
    int productId = -1;
    int materialId = -1;
    QString productCode;
    QString productName;
    QString materialCode;
    QString materialName;
    QString unit;
    double quantityPerSet = 0.0;

    bool isValid() const { return id >= 0; }
};
