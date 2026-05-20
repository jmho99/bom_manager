#pragma once

#include <QDialog>

class QDateEdit;
class QPlainTextEdit;
class QSpinBox;

class BuildDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BuildDialog(QWidget* parent = nullptr);

    int setCount() const;
    QString eventDate() const;
    QString memo() const;

private:
    QSpinBox* m_setCountSpin;
    QDateEdit* m_dateEdit;
    QPlainTextEdit* m_memoEdit;
};
