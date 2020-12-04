#include "seriallogic.h"
#include "myhelper.h"

#include <QDebug>
SerialLogic::SerialLogic(QObject *parent)
{

}

void SerialLogic::serialDataHandler(const QByteArray &data)
{
    qDebug() << "SerialLogic serialDataHandler";
    m_receivedData += data;
    if(serialDataCheck()) {
        serialDataHandle(m_stmData);
        m_receivedData = m_receivedData.mid(m_receivedData.indexOf(char(0x0d))+1);
        qDebug() << "m_receivedData" << m_receivedData;
    }else {

    }
}

void SerialLogic::serialPackData(const SerialLogic::stmInfo &info)
{
    QString data;
    /***
     *打包*
     **/
    SendData(data);
}

bool SerialLogic::serialDataCheck()
{
    qDebug() << "serialDataCheck";

    if(m_receivedData.contains(char(0x51)) && m_receivedData.contains(char(0x0d)))
    {
        /* 解析过程
          */
        if(m_receivedData.size()>=6){
            m_stmData.stmCmd =m_receivedData.at(2);
            m_stmData.stmData = m_receivedData.mid(3,5);
        }
        return true;
    }else {
        return false;
    }
}

void SerialLogic::serialDataHandle(const SerialLogic::stmInfo &info)
{
    qDebug() << "serialDataHandle" << info.stmCmd << info.stmData << info.stmData.size();
    emit serialPortAppend(AppendType::TypeMessage,QString("cmd: %1 data: %2").arg(QString::number(info.stmCmd)).arg(MyHelper::byteArrayToHexStr(info.stmData)));
    switch (info.stmCmd)
    {
    case 0x01://心跳
    {
        break;
    }
    case 0x02://运动控制
    {
        quint8 dec = static_cast<quint8>(info.stmData[0]);          //十六进制转十进制
        qDebug() << taskName(info.stmCmd) << dec;
        break;
    }
    case 0x07://电量信息
    {
        quint8 SDJ_BatteryCapacity = static_cast<quint8>(info.stmData[0]);
        quint16 SDJ_BatteryValue = static_cast<quint16>(static_cast<quint8>(info.stmData[1])<<8  | static_cast<quint8>(info.stmData[2])) *0.1;
        qDebug() << taskName(info.stmCmd) << SDJ_BatteryCapacity << SDJ_BatteryValue;
        break;
    }
    default:
    {
        qDebug() << "未包含此命令" << info.stmCmd;
        break;
    }
    }
}

QString SerialLogic::taskName(char stmCmd)
{
    QString taskName;
    switch (stmCmd)
    {
    case 0x02://写入标签信息
        taskName = "运动控制";
        break;
    case 0x07://获取标签信息
        taskName = "电量信息";
        break;
    default:
        taskName = "没这命令行";
        break;
    }
    return taskName;
}
