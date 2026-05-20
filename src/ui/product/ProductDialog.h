#pragma once

#include <QDialog>

#include "core/models/Product.h"

class QLineEdit;
class QPlainTextEdit;

class ProductDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProductDialog(QWidget* parent = nullptr);
    void setProduct(const Product& product);
    Product product() const;

private:
    int m_id = -1;
    QLineEdit* m_codeEdit;
    QLineEdit* m_nameEdit;
    QPlainTextEdit* m_memoEdit;
};
