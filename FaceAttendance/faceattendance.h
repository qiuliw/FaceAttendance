#ifndef FACEATTENDANCE_H
#define FACEATTENDANCE_H

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <qtcpsocket.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class FaceAttendance;
}
QT_END_NAMESPACE

class FaceAttendance : public QMainWindow
{
    Q_OBJECT

public:
    FaceAttendance(QWidget *parent = nullptr);
    ~FaceAttendance();

    // 定时器事件
    void timerEvent(QTimerEvent *event);

private:
    Ui::FaceAttendance *ui;

    // Camera
    cv::VideoCapture cap;
    // haar--级联分类器
    cv::CascadeClassifier cascade;

    // 创建网络套接字，定时器
    QTcpSocket *socket_;
    QTimer *timer_;

};
#endif // FACEATTENDANCE_H
