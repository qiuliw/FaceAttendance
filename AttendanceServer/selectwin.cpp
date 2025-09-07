#include "selectwin.h"
#include "ui_selectwin.h"

SelectWin::SelectWin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SelectWin)
{
    ui->setupUi(this);
    model_ = new QSqlTableModel();

    connect(ui->selectBtn, &QPushButton::clicked, this, &SelectWin::onSelectBtnClicked);
}

SelectWin::~SelectWin()
{
    delete ui;
}

void SelectWin::onSelectBtnClicked()
{
    if(ui->employeeBtn->isChecked()){
        model_->setTable("employee");
    }else if(ui->attendanceBtn->isChecked()){
        model_->setTable("attendance");
    }
    // 设置过滤器
    // model_->setFilter("name='张三'");

    // 查询
    model_->select();

    ui->tableView->setModel(model_);
}   

