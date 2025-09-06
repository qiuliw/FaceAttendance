#pragma once
#include "opencv2/videoio.hpp"
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <qtcpsocket.h>
#include <QMap>
#include <memory>
#include <qtmetamacros.h>
#include <qwindowdefs.h>

// 前置声明
QT_BEGIN_NAMESPACE
namespace Ui {
class RegisterWin;
}
QT_END_NAMESPACE

// 服务端本地注册窗口
class RegisterWin : public QMainWindow
{ 
    Q_OBJECT
public:
    RegisterWin(QWidget *parent = nullptr);
    ~RegisterWin();
    void timerEvent(QTimerEvent *e) override;

public slots:
    
    void onAddBtnClicked(); // 注册
    void onResetBtnClicked(); // 重置

    void onAddAvatarBtnClicked(); // 添加头像
    void onOpenCameraBtnClicked(); // 打开摄像头
    void onTakePhotoBtnClicked(); // 拍照

private:
    Ui::RegisterWin *ui;
    int timerId_;
    cv::VideoCapture cap_;
    cv::Mat image_;

};
