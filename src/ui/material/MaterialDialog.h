#pragma once

#include <QDialog>

#include "core/models/Material.h"

class QLineEdit;
class QPlainTextEdit;

class MaterialDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MaterialDialog(QWidget* parent = nullptr);
    void setMaterial(const Material& material);
    Material material() const;

private:
    int m_id = -1;
    QLineEdit* m_codeEdit;
    QLineEdit* m_nameEdit;
    QLineEdit* m_unitEdit;
    QLineEdit* m_locationEdit;
    QPlainTextEdit* m_memoEdit;
};
