#pragma once

#include <QString>

struct Material
{
    int id = -1;
    QString code;
    QString name;
    QString unit;
    QString location;
    QString memo;
    QString createdAt;
    QString updatedAt;

    bool isValid() const { return id >= 0; }
};
