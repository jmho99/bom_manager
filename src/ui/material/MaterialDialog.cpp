#include "MaterialDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

MaterialDialog::MaterialDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("재료 정보");
    resize(440, 320);

    m_codeEdit = new QLineEdit(this);
    m_nameEdit = new QLineEdit(this);
    m_unitEdit = new QLineEdit(this);
    m_unitEdit->setText("ea");
    m_locationEdit = new QLineEdit(this);
    m_memoEdit = new QPlainTextEdit(this);

    auto* form = new QFormLayout;
    form->addRow("재료 코드", m_codeEdit);
    form->addRow("재료명", m_nameEdit);
    form->addRow("단위", m_unitEdit);
    form->addRow("보관 위치", m_locationEdit);
    form->addRow("메모", m_memoEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &MaterialDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MaterialDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void MaterialDialog::setMaterial(const Material& material)
{
    m_id = material.id;
    m_codeEdit->setText(material.code);
    m_nameEdit->setText(material.name);
    m_unitEdit->setText(material.unit);
    m_locationEdit->setText(material.location);
    m_memoEdit->setPlainText(material.memo);
}

Material MaterialDialog::material() const
{
    Material material;
    material.id = m_id;
    material.code = m_codeEdit->text();
    material.name = m_nameEdit->text();
    material.unit = m_unitEdit->text();
    material.location = m_locationEdit->text();
    material.memo = m_memoEdit->toPlainText();
    return material;
}
