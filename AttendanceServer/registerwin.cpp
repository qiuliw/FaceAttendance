#include "registerwin.h"
#include "./ui_registerwin.h"
#include "QFaceobject.h"
#include "opencv2/imgcodecs.hpp"

#include <qdatetime.h>
#include <qimage.h>
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

void RegisterWin::timerEvent(QTimerEvent *e)
{
    // 获取摄像头数据并且显示在界面上
    if(!cap_.isOpened()) return;
    cap_>>image_;
    if(image_.empty()) return;
    // Mat >> QImage
    cv::Mat rgbImage;
    cv::cvtColor(image_, rgbImage, cv::COLOR_BGR2RGB); // 转换为RGB
    QImage qimage(rgbImage.data,rgbImage.cols, rgbImage.rows,rgbImage.step1(), QImage::Format_RGB888);
    // 在qt界面上显示
    ui->avatarLabel->setPixmap(QPixmap::fromImage(qimage).scaledToWidth(ui->avatarLabel->width()));
}

/*
    拍照 --> 文件 --> 路径 --> 数据库
    打开文件 --> 路径 --> 数据库
*/

// 注册
void RegisterWin::onAddBtnClicked()
{
    // 1. 基本检查
    if (ui->nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "注册提示", "请输入姓名");
        return;
    }
    if (image_.empty()) {
        QMessageBox::warning(this, "注册提示", "请先选择头像或拍照");
        return;
    }

    // 2. 生成头像最终保存路径
    QString safeName = QString(ui->nameEdit->text().toUtf8().toBase64());
    QString headfile = QString("./data/%1.jpg").arg(safeName);

    // 确保目录存在
    QDir().mkpath("./data");

    // 3. 把 image_ 保存到 ./data
    if(!cv::imwrite(headfile.toStdString(), image_)){
        QMessageBox::warning(this,"注册提示","无法保存头像文件");
        return;
    }
    // 显示目录
    ui->avatarPathEdit->setText(headfile);

    // 4. 人脸注册
    QFaceObject faceObj;
    int faceID = faceObj.faceRegister(image_);
    if (faceID == -1) {
        QMessageBox::warning(this, "注册提示", "人脸注册失败，请确保照片中包含清晰的人脸");
        return;
    }

    // 把个人信息存储到数据库employee
    QSqlTableModel model;
    model.setTable("employee"); // 表名
    QSqlRecord record = model.record();

    // 设置数据
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
        // 插入成功，提交到数据库
        if (model.submitAll()) {
            QMessageBox::information(this, "注册提示", "注册成功");
        } else {
            // 提交失败，显示具体错误信息
            QMessageBox::warning(this, "注册提示", "注册失败: " + model.lastError().text());
        }
    } else {
        // 插入失败，显示具体错误信息
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
// 选择头像
void RegisterWin::onAddAvatarBtnClicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "选择头像", "C:/", "*.png *.jpg *.jpeg");
    if(filepath.isEmpty()) return;

    image_ = cv::imread(filepath.toStdString());   // 直接读进 image_
    if(image_.empty()){
        QMessageBox::warning(this,"提示","无法读取图片");
        return;
    }

    // 预览
    cv::Mat rgb;
    cv::cvtColor(image_, rgb, cv::COLOR_BGR2RGB);
    QImage img(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(img).scaledToWidth(ui->avatarLabel->width()));

    ui->avatarPathEdit->clear();   // 不再记录原路径
}
// 打开摄像头
void RegisterWin::onOpenCameraBtnClicked()
{
    if(ui->openCameraBtn->text() == "打开摄像头"){
        if(cap_.open(0)){
            ui->openCameraBtn->setText("关闭摄像头");
            timerId_ = startTimer(100);     
        }
    }else{
        killTimer(timerId_);
        ui->openCameraBtn->setText("打开摄像头");
        ui->avatarLabel->clear();
        cap_.release();
    }
    
}

// 拍照
void RegisterWin::onTakePhotoBtnClicked()
{
    if(!cap_.isOpened()) return;

    // 摄像头当前帧已经在 image_ 里，直接保留即可
    killTimer(timerId_);
    ui->openCameraBtn->setText("打开摄像头");
    cap_.release();
}