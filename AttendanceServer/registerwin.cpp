#include "registerwin.h"
#include "./ui_registerwin.h"
#include "QFaceobject.h"

#include <qdatetime.h>
#include <qpixmap.h>
#include <QFileDialog>
#include <opencv2/core/mat.hpp>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QMessageBox>

RegisterWin::RegisterWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegisterWin)
{ 
    ui->setupUi(this);

    connect(ui->addBtn, &QPushButton::clicked, this, &RegisterWin::onAddBtnClicked);
    connect(ui->resetBtn, &QPushButton::clicked, this, &RegisterWin::onResetBtnClicked);
    connect(ui->addAvatarBtn, &QPushButton::clicked, this, &RegisterWin::onAddAvatarBtnClicked);
    connect(ui->openCameraBtn, &QPushButton::clicked, this, &RegisterWin::onOpenCameraBtnClicked);
    connect(ui->takePhotoBtn, &QPushButton::clicked, this, &RegisterWin::onTakePhotoBtnClicked);
}

RegisterWin::~RegisterWin()
{
    delete ui;
}

/*
    拍照 --> 文件 --> 路径 --> 数据库
    打开文件 --> 路径 --> 数据库
*/

// 注册
void RegisterWin::onAddBtnClicked()
{
    // 1. 检查必要字段是否为空
    if (ui->nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "注册提示", "请输入姓名");
        return;
    }
    
    if (ui->avatarPathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "注册提示", "请选择头像");
        return;
    }
    
    // 2. 检查头像文件是否存在
    QString avatarPath = ui->avatarPathEdit->text();
    QFile avatarFile(avatarPath);
    if (!avatarFile.exists()) {
        QMessageBox::warning(this, "注册提示", "头像文件不存在，请重新选择");
        return;
    }
    
    // 3. 通过照片，结合faceObject模块得到faceID
    QFaceObject faceObj;
    cv::Mat image = cv::imread(avatarPath.toUtf8().data());
    if (image.empty()) {
        QMessageBox::warning(this, "注册提示", "无法读取头像文件，请选择有效的图片文件");
        return;
    }
    
    int faceID = faceObj.faceRegister(image);
    if (faceID == -1) {
        QMessageBox::warning(this, "注册提示", "人脸注册失败，请确保照片中包含清晰的人脸");
        return;
    }
    
    // 确保data目录存在
    QDir dataDir("./data");
    if (!dataDir.exists()) {
        dataDir.mkpath("./data");
    }
    
    // 先转 UTF-8，再 Base64，保证文件名只包含安全字符，避免特殊符号或跨平台兼容问题
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    
    // 2. 把个人信息存储到数据库employee
    QSqlTableModel model;
    model.setTable("employee"); // 表名
    QSqlRecord record = model.record();

    // 3. 设置数据
    record.setValue("name", ui->nameEdit->text());
    record.setValue("birthday", ui->birthdayEdit->date());
    record.setValue("sex", ui->maleRadio->isChecked()? "男" : "女");
    record.setValue("address", ui->addressEdit->text());
    record.setValue("phone", ui->phoneEdit->text());
    record.setValue("faceID", faceID);
    // 头像路径
    record.setValue("headfile", headfile);
    // 把记录插入到模型的数据库表格中
    bool ret = model.insertRecord(-1, record);

    if (ret) {
        // 提交
        if (model.submitAll()) {
            // 实际复制头像文件到data目录
            QFile::copy(avatarPath, headfile);
            QMessageBox::information(this, "注册提示", "注册成功");
        } else {
            QMessageBox::warning(this, "注册提示", "注册失败: " + model.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "注册提示", "注册失败: " + model.lastError().text());
    }
}

// 重置
void RegisterWin::onResetBtnClicked()
{
    // 清空数据
    ui->nameEdit->clear();
    ui->birthdayEdit->setDate(QDate::currentDate());
    ui->addressEdit->clear();
    ui->phoneEdit->clear();
    
    ui->avatarLabel->clear();
    ui->avatarPathEdit->clear();
}

// 添加头像
void RegisterWin::onAddAvatarBtnClicked()
{
    // 通过文件对话框，选中图片路径
    QString filepath = QFileDialog::getOpenFileName(this, "选择头像", "C:/", "*.png *.jpg *.jpeg");
    ui->avatarPathEdit->setText(filepath);

    // 显示图片
    QPixmap pixmap(filepath);
    pixmap = pixmap.scaled(ui->avatarLabel->size(), Qt::KeepAspectRatio);
    ui->avatarLabel->setPixmap(pixmap);
}

// 打开摄像头
void RegisterWin::onOpenCameraBtnClicked()
{
    
}

// 拍照
void RegisterWin::onTakePhotoBtnClicked()
{
    
}
