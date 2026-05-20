#include "BomItemDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>

BomItemDialog::BomItemDialog(const QList<Material>& materials, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("BOM 항목");
    resize(420, 160);

    m_materialCombo = new QComboBox(this);
    for (const Material& material : materials) {
        m_materialCombo->addItem(QString("%1 - %2 [%3]").arg(material.code, material.name, material.unit), material.id);
    }

    m_quantitySpin = new QDoubleSpinBox(this);
    m_quantitySpin->setRange(0.000001, 1000000000.0);
    m_quantitySpin->setDecimals(3);
    m_quantitySpin->setValue(1.0);

    auto* form = new QFormLayout;
    form->addRow("재료", m_materialCombo);
    form->addRow("1세트당 필요 수량", m_quantitySpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &BomItemDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &BomItemDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

int BomItemDialog::materialId() const
{
    if (m_materialCombo->currentIndex() < 0) return -1;
    return m_materialCombo->currentData().toInt();
}

double BomItemDialog::quantityPerSet() const
{
    return m_quantitySpin->value();
}

void BomItemDialog::setInitial(int materialId, double quantityPerSet)
{
    const int idx = m_materialCombo->findData(materialId);
    if (idx >= 0) m_materialCombo->setCurrentIndex(idx);
    m_quantitySpin->setValue(quantityPerSet);
}
