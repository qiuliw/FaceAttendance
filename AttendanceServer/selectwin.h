#pragma once

#include <QWidget>
#include <qsqltablemodel.h>

namespace Ui {
class SelectWin;
}

class SelectWin : public QWidget
{
    Q_OBJECT

public:
    explicit SelectWin(QWidget *parent = nullptr);
    ~SelectWin();

public slots:
    void onSelectBtnClicked();

private:
    Ui::SelectWin *ui;
    QSqlTableModel *model_;

};
