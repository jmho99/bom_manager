#include "StockEventDialog.h"

#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPlainTextEdit>
#include <QVBoxLayout>

StockEventDialog::StockEventDialog(const QString& title, const QString& quantityLabel, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    resize(420, 240);

    m_quantitySpin = new QDoubleSpinBox(this);
    m_quantitySpin->setRange(0.000001, 1000000000.0);
    m_quantitySpin->setDecimals(3);
    m_quantitySpin->setValue(1.0);

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");

    m_memoEdit = new QPlainTextEdit(this);

    auto* form = new QFormLayout;
    form->addRow(quantityLabel, m_quantitySpin);
    form->addRow("일자", m_dateEdit);
    form->addRow("메모", m_memoEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &StockEventDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &StockEventDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

double StockEventDialog::quantity() const { return m_quantitySpin->value(); }
QString StockEventDialog::eventDate() const { return m_dateEdit->date().toString(Qt::ISODate); }
QString StockEventDialog::memo() const { return m_memoEdit->toPlainText(); }
