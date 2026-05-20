#include "DateTimeUtils.h"

#include <QDate>
#include <QDateTime>

QString DateTimeUtils::nowIsoString()
{
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

QString DateTimeUtils::todayIsoDate()
{
    return QDate::currentDate().toString(Qt::ISODate);
}
