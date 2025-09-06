#include "attendancewin.h"
#include "./ui_attendancewin.h"
#include "opencv2/imgcodecs.hpp"

#include <cstring>
#include <qdatetime.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qsqlrecord.h>
#include <vector>

#include "FaceEngine.h"
#include "qsqlquery.h"

AttendanceWin::AttendanceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AttendanceWin)
{
    ui->setupUi(this);

    // 给sql模型绑定表格
    model_.setTable("employee");

    /*
        = 捕捉 this 指针
    */

    // qtcpserver当有客户端链接时，会触发newConnection信号
    connect(&server_, &QTcpServer::newConnection, [this](){ // 接受客户端连接
        QTcpSocket* clientSocket = server_.nextPendingConnection(); // 取出一个客户端连接
        // 为新客户端创建资源结构体
        this->clientResources_[clientSocket] = std::make_shared<ClientResource>();
        
        // 当客户端有数据到达会触发readyRead信号
        connect(clientSocket, &QTcpSocket::readyRead, [this, clientSocket](){  // 接收数据
           
            // 读取所有数据
            QDataStream stream(clientSocket);
            stream.setVersion(QDataStream::Qt_5_14);
            
            // 获取客户端资源
            auto& resource = this->clientResources_[clientSocket];
            
            // 获取数据大小
            if(resource->bsize == 0){
                if(clientSocket->bytesAvailable() < (int)sizeof(quint64)){
                    return;
                }
                stream >> resource->bsize;
            }
            
            // 获取数据
            if(clientSocket->bytesAvailable() < resource->bsize){ // 数据不完整，等待
                return;
            }
            stream >> resource->data;

            if(resource->data.size() == 0) // 数据为空
                return;
            // 数据完整
            resource->bsize = 0; 

            // 显示图片
            QPixmap mmp;
            mmp.loadFromData(resource->data, "jpg");
            // 修复图像缩放问题：需要使用scaled方法的返回值
            QPixmap scaledMmp = mmp.scaled(ui->pictureLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->pictureLabel->setPixmap(scaledMmp);

            // 识别人脸
            cv::Mat faceImage;
            std::vector<uchar> decode;
            decode.resize(resource->data.size());
            memcpy(decode.data(), resource->data.data(), resource->data.size());
            faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);

            int faceID = fobj_.faceQuery(faceImage); // 消耗资源较多，可以在新线程中处理
            qDebug() << faceID;

            if(faceID == -1) return;

            // 检查是否是同一人脸在短时间内重复发送
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            if (this->lastRecognitionTime_.contains(faceID) && 
                (currentTime - this->lastRecognitionTime_[faceID]) < 30000) { // 30秒内同一人脸不再发送
                return;
            }
            
            // 更新此人脸的最后识别时间
            this->lastRecognitionTime_[faceID] = currentTime;

            // 从数据库中查询faceID对应的数据
            model_.setFilter(QString("faceID = %1").arg(faceID));
            model_.select();
            if(model_.rowCount() == 1){
                // 工号，姓名，部门，时间
                // {employeeUD:%1, name:%2, department:%3, time:%4}
                QSqlRecord record = model_.record(0);
                QString sdmsg = QString("{\"employeeID\":\"%1\", \"name\":\"%2\", \"department\":\"%3\", \"time\":\"%4\"}")
                .arg(record.value("employeeID").toString())
                .arg(record.value("name").toString())
                .arg(record.value("department").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                clientSocket->write(sdmsg.toUtf8());

                // 把数据写入数据库--考勤表
                
            }
        });
        
        // 客户端断开连接时清理资源
        connect(clientSocket, &QTcpSocket::disconnected, [this, clientSocket](){
            this->clientResources_.remove(clientSocket);
            clientSocket->deleteLater();
        });

    });
    server_.listen(QHostAddress::Any, 9999); // 监听，启动服务器
}

AttendanceWin::~AttendanceWin()
{
    delete ui;
}