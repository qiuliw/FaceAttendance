#ifndef FACEATTENDANCE_H
#define FACEATTENDANCE_H

#include "opencv2/videoio.hpp"
#include <QMainWindow>
#include <opencv2/opencv.hpp>

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

};
#endif // FACEATTENDANCE_H
