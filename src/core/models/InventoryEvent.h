#pragma once

#include <QString>

struct InventoryEvent
{
    int id = -1;
    QString eventUuid;
    QString eventType;

    int materialId = -1;
    QString materialCode;
    QString materialName;
    QString materialUnit;

    int productId = -1;
    QString productCode;
    QString productName;

    double quantityDelta = 0.0;
    QString eventDate;
    QString createdAt;
    QString sourceDevice;
    QString memo;
};
