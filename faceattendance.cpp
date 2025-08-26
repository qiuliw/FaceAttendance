#include "faceattendance.h"
#include "./ui_faceattendance.h"
#include "opencv2/opencv.hpp"
#include <qpixmap.h>
FaceAttendance::FaceAttendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttendance)
{
    ui->setupUi(this);
    // 打开摄像头
    cap.open(0); // dev/video0
    // 启动定时器
    startTimer(100);
}

FaceAttendance::~FaceAttendance()
{
    delete ui;
}

void FaceAttendance::timerEvent(QTimerEvent *event)
{
    // 创建一个Mat对象用于存储从摄像头捕获的图像数据
    cv::Mat srcImage;    
    // 尝试从摄像头抓取一帧图像
    if(cap.grab()){
        // 读取抓取到的图像数据到srcImage中
        cap.read(srcImage); // 采集一帧数据
    }
    
    // 检查图像数据是否为空，如果为空则直接返回，避免后续处理空数据
    if(srcImage.data == nullptr) return;

    // 将OpenCV使用的BGR颜色格式转换为Qt使用的RGB颜色格式
    // OpenCV默认使用BGR，而Qt使用RGB，所以需要进行转换以确保颜色正确显示
    cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);

    // 将OpenCV的Mat对象转换为Qt的QImage对象
    // 参数说明：图像数据指针、宽度、高度、图像格式（RGB888）
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, QImage::Format_RGB888);

    // 将QImage转换为QPixmap，因为QLabel需要QPixmap来显示图像
    QPixmap pixmap = QPixmap::fromImage(image);
    
    // 缩放图像以适应videoLabel控件的大小，保持宽高比并使用平滑变换提高图像质量
    pixmap = pixmap.scaled(ui->videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 将处理好的图像设置到videoWidget控件上进行显示
    ui->videoLabel->setPixmap(pixmap);

}
