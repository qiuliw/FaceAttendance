#pragma once
#include <QObject>
#include <memory>

#include <opencv2/opencv.hpp>
#include "FaceEngine.h"
#include "seeta/CStruct.h"

/*
    人脸数据存储
    人脸检测
    人脸识别
*/

typedef std::unique_ptr<seeta::FaceEngine> FaceEnginePtr;

class QFaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QFaceObject(QObject *parent = nullptr);
    ~QFaceObject();

signals:
    void faceQueryResult(int64_t faceID, qint64 requestId);

public slots:
    void faceQuery(cv::Mat image, qint64 requestId);
    int64_t faceRegister(cv::Mat& image);
    
private:
    FaceEnginePtr faceEnginePtr_;

};




