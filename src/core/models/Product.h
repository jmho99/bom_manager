#pragma once

#include <QString>

struct Product
{
    int id = -1;
    QString code;
    QString name;
    QString memo;
    QString createdAt;
    QString updatedAt;

    bool isValid() const { return id >= 0; }
};
