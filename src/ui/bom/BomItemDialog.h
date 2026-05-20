#pragma once

#include <QDialog>
#include <QList>

#include "core/models/Material.h"

class QComboBox;
class QDoubleSpinBox;

class BomItemDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BomItemDialog(const QList<Material>& materials, QWidget* parent = nullptr);

    int materialId() const;
    double quantityPerSet() const;
    void setInitial(int materialId, double quantityPerSet);

private:
    QComboBox* m_materialCombo;
    QDoubleSpinBox* m_quantitySpin;
};
