#include "QFaceObject.h"
#include <qdebug.h>
#include <qtmetamacros.h>
#include <seeta/Struct.h>

QFaceObject::QFaceObject(QObject *parent) : QObject(parent)
{
    // 初始化引擎
    // 使用默认构造参数，实际使用时可能需要根据具体模型路径调整
    seeta::ModelSetting fdModel("D:/Software/SeetaFace2/models/fd_2_00.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting pdModel("D:/Software/SeetaFace2/models/pd_2_00_pts5.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting frModel("D:/Software/SeetaFace2/models/fr_2_10.dat",seeta::ModelSetting::CPU,0);
    // faceEnginePtr_ = FaceEnginePtr(new seeta::FaceEngine(fdModel,pdModel,frModel));
    faceEnginePtr_ = std::make_unique<seeta::FaceEngine>(fdModel,pdModel,frModel);
    
    // 加载数据库，没有会自动创建
    faceEnginePtr_->Load("./face.db");
}

QFaceObject::~QFaceObject()
{
    // 使用智能指针，无需手动删除
}

int64_t QFaceObject::faceRegister(cv::Mat& image)
{
    // 把图片转换成SeetaImageData
    SeetaImageData seetaImage;
    seetaImage.data = image.data;
    seetaImage.width = image.cols;
    seetaImage.height = image.rows;
    seetaImage.channels = image.channels();
    // 注册后会返回人脸ID
    int64_t faceId = faceEnginePtr_->Register(seetaImage);
    if(faceId >= 0){
        faceEnginePtr_->Save("./face.db"); // 保存数据库
        qDebug() << "注册成功，人脸ID为：" << faceId;
    }

    return faceId;
}

// 查询人脸
// QFaceObject.cpp
void QFaceObject::faceQuery(cv::Mat image, qint64 requestId)
{
    SeetaImageData seetaImage;
    seetaImage.data = image.data;
    seetaImage.width = image.cols;
    seetaImage.height = image.rows;
    seetaImage.channels = image.channels();

    int64_t faceID = faceEnginePtr_->Query(seetaImage);

    if(faceID < 0){
        qDebug() << "未找到人脸";
    }else{
        qDebug() << "查询成功，人脸ID为：" << faceID;
    }

    emit faceQueryResult(faceID, requestId);
}

