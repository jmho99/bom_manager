#pragma once

#include <QDialog>

class QDateEdit;
class QDoubleSpinBox;
class QPlainTextEdit;

class StockEventDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StockEventDialog(const QString& title, const QString& quantityLabel, QWidget* parent = nullptr);

    double quantity() const;
    QString eventDate() const;
    QString memo() const;

private:
    QDoubleSpinBox* m_quantitySpin;
    QDateEdit* m_dateEdit;
    QPlainTextEdit* m_memoEdit;
};
