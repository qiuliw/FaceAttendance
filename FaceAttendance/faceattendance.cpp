#include "faceattendance.h"
#include "./ui_faceattendance.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
#include <qlogging.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstringview.h>
#include <qtypes.h>
#include <vector>
#include <QTcpSocket>
#include <QTimer>

using namespace cv;

FaceAttendance::FaceAttendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttendance)
    , socket_(new QTcpSocket(this))
    , timer_(new QTimer(this))
{
    ui->setupUi(this);
    // 打开摄像头
    cap.open(0); // dev/video0
    // 启动定时器
    startTimer(100);

    // 导入级联分类器文件
    cascade.load("D:/Software/opencv/build/etc/haarcascades/haarcascade_frontalface_alt2.xml");

    /*
        未连接或连接断开，启动定时器5s后尝试一次连接。连接成功停止定时器。
    */

    // QTcpSocket当断开连接的时候会触发disconnected()信号，连接成功会触发connected()信号
    connect(socket_, &QTcpSocket::disconnected,[=](){
        timer_->start(5000);
        qDebug() << "已断开服务器...";
    });

    // 定时连接服务器
    connect(timer_, &QTimer::timeout, [=](){
        socket_->connectToHost("127.0.0.1", 9999);
        qDebug() << "尝试连接服务器...";
    });

    // 连接成功后，停止定时器
    connect(socket_, &QTcpSocket::connected, [=](){
        timer_->stop();
        qDebug() << "已连接服务器...";
    });

    // 关联接受数据的槽函数
    connect(socket_, &QTcpSocket::readyRead, this,&FaceAttendance::RecvData);

    // 启动定时器
    timer_->start(5000); // 每5秒尝试一次连接，直到连接成功就不再连接

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

    std::vector<cv::Rect> faceRects;
    // 检测人脸
    cascade.detectMultiScale(
        srcImage,           // 输入图像
        faceRects,          // 被检测的物体矩形框向量
        1.1,                // 每次图像尺寸减小的比例
        5,                  // 每个候选矩形应保留的邻近矩形数
        0,                  // flags参数 CV_HAAR_DO_CANNY_PRUNING
        cv::Size(30, 30)    // 最小人脸尺寸
    );
    // 绘制人脸矩形框
    if( faceRects.size() > 0){
        Rect rect = faceRects.at(0); // 第一个人脸的矩形框
        rectangle(srcImage, rect, Scalar(0, 255, 0), 2);
        
        // 把Mat数据转为QByteArray --> 编码成 jpg 格式

        std::vector<uchar> buf;
        imencode(
            ".jpg", 
            srcImage, 
            buf, 
            {IMWRITE_JPEG_QUALITY, 100}
        );
        // 转换为QByteArray
        QByteArray byteArray(reinterpret_cast<const char*>(buf.data()), buf.size());
        // 准备发送
        quint64 backsize = byteArray.size();
        QByteArray sendBuffer;
        QDataStream stream(&sendBuffer, QIODevice::WriteOnly); // 使用QDataStream流去写入数据，自动处理边界
        stream.setVersion(QDataStream::Qt_5_14);
        stream << backsize << byteArray;
        // 发送数据
        socket_->write(sendBuffer);

    }


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

void FaceAttendance::RecvData()
{
    QString msg = socket_->readAll();
    qDebug() << msg;
}

