#include "attendancewin.h"
#include "./ui_attendancewin.h"
#include "opencv2/imgcodecs.hpp"

#include <cstring>
#include <qdatetime.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qsqlerror.h>
#include <qsqlrecord.h>
#include <qtmetamacros.h>
#include <vector>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>

#include "FaceEngine.h"
#include "qsqlquery.h"

AttendanceWin::AttendanceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AttendanceWin)
{
    ui->setupUi(this);

    // 给sql模型绑定表格
    model_.setTable("employee");

    // 创建线程
    QThread *thread = new QThread();
    // 把fobj_移动到线程中执行
    fobj_.moveToThread(thread); // 移动后，函数相当于槽函数，需要绑定信号去触发
    thread->start(); // 启动线程
    connect(this, &AttendanceWin::query, &fobj_, &QFaceObject::faceQuery);
    // 关联数据处理回调
    connect(&fobj_, &QFaceObject::faceQueryResult,this, &AttendanceWin::faceIDMatch);


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

            // int faceID = fobj_.faceQuery(faceImage); // 消耗资源较多，可以在新线程中处理
            
            qint64 requestId = nextRequestId_++;
            requestSocketMap_[requestId] = clientSocket;

            emit query(faceImage, requestId);


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

void AttendanceWin::faceIDMatch(int64_t faceID, qint64 requestId)
{
    if (!requestSocketMap_.contains(requestId)) return;
    QTcpSocket* clientSocket = requestSocketMap_[requestId];
    requestSocketMap_.remove(requestId);

    if (faceID == -1 || !clientSocket || !clientSocket->isOpen()) return;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (this->lastRecognitionTime_.contains(faceID) &&
        (currentTime - this->lastRecognitionTime_[faceID]) < 30000) {
        return; // 30 秒内不重复考勤
    }
    this->lastRecognitionTime_[faceID] = currentTime;

    model_.setFilter(QString("faceID = %1").arg(faceID));
    model_.select();
    if (model_.rowCount() == 1) {
        QSqlRecord record = model_.record(0);

        QString employeeID = record.value("employeeID").toString();
        QString name       = record.value("name").toString();
        QString department = record.value("department").toString();
        QString time       = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        // --- 写入考勤表 ---
        QSqlQuery insertQuery;
        insertQuery.prepare(
            "INSERT INTO attendance (employeeID, attTime, notes) "
            "VALUES (:employeeID, :attTime, :notes)"
        );
        insertQuery.bindValue(":employeeID", employeeID);
        insertQuery.bindValue(":attTime", time);
        insertQuery.bindValue(":notes", "正常打卡");
        if (!insertQuery.exec()) {
            qDebug() << "写入考勤表失败:" << insertQuery.lastError().text();
        }

        // --- 组织 JSON 数据 ---
        QJsonObject jsonObj;
        jsonObj["employeeID"] = employeeID;
        jsonObj["name"]       = name;
        jsonObj["department"] = department;
        jsonObj["time"]       = time;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_14);
        out << (quint64)jsonData.size();
        out.writeRawData(jsonData.data(), jsonData.size());

        clientSocket->write(block);
        clientSocket->flush();
    }
}
