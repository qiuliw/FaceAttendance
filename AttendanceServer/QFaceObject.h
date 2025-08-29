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

public slots:
    int64_t faceRegister(cv::Mat& image);
    int64_t faceQuery(cv::Mat& image);

signals:
    
private:
    FaceEnginePtr faceEnginePtr_;

};




