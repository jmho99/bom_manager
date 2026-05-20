#include "BuildDialog.h"

#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QVBoxLayout>

BuildDialog::BuildDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("제품 제작 처리");
    resize(420, 240);

    m_setCountSpin = new QSpinBox(this);
    m_setCountSpin->setRange(1, 1000000);
    m_setCountSpin->setValue(1);

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");

    m_memoEdit = new QPlainTextEdit(this);

    auto* form = new QFormLayout;
    form->addRow("제작 세트 수", m_setCountSpin);
    form->addRow("제작일", m_dateEdit);
    form->addRow("메모", m_memoEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &BuildDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &BuildDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

int BuildDialog::setCount() const { return m_setCountSpin->value(); }
QString BuildDialog::eventDate() const { return m_dateEdit->date().toString(Qt::ISODate); }
QString BuildDialog::memo() const { return m_memoEdit->toPlainText(); }
