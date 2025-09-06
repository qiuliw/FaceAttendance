#ifndef ATTENDANCEWIN_H
#define ATTENDANCEWIN_H

#include "QFaceObject.h"
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <qsqltablemodel.h>
#include <qtcpsocket.h>
#include <QMap>
#include <QDateTime>
#include <memory>

// 前置声明
QT_BEGIN_NAMESPACE
namespace Ui {
class AttendanceWin;
}
QT_END_NAMESPACE

// 客户端资源结构体
struct ClientResource {
    quint64 bsize = 0;
    QByteArray data;
    // 可以添加更多客户端相关的资源字段
};

// 可视化的服务端考勤窗口
class AttendanceWin : public QMainWindow
{
    Q_OBJECT

public:
    AttendanceWin(QWidget *parent = nullptr);
    ~AttendanceWin();

private:
    Ui::AttendanceWin *ui;

    QTcpServer server_;
    QTcpSocket *socket_ = nullptr;
    
    // 为每个客户端连接维护独立的资源结构体
    QMap<QTcpSocket*, std::shared_ptr<ClientResource>> clientResources_;

    QFaceObject fobj_; // 识别引擎
    QSqlTableModel model_; // 数据库模型
    
    // 存储每个人脸最后一次识别的时间（毫秒）
    QMap<int, qint64> lastRecognitionTime_;

};
#endif // ATTENDANCEWIN_H