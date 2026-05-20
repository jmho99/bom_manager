#include "ProductDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

ProductDialog::ProductDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("제품 정보");
    resize(420, 260);

    m_codeEdit = new QLineEdit(this);
    m_nameEdit = new QLineEdit(this);
    m_memoEdit = new QPlainTextEdit(this);

    auto* form = new QFormLayout;
    form->addRow("제품 코드", m_codeEdit);
    form->addRow("제품명", m_nameEdit);
    form->addRow("메모", m_memoEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ProductDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ProductDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void ProductDialog::setProduct(const Product& product)
{
    m_id = product.id;
    m_codeEdit->setText(product.code);
    m_nameEdit->setText(product.name);
    m_memoEdit->setPlainText(product.memo);
}

Product ProductDialog::product() const
{
    Product product;
    product.id = m_id;
    product.code = m_codeEdit->text();
    product.name = m_nameEdit->text();
    product.memo = m_memoEdit->toPlainText();
    return product;
}
