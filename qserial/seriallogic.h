#ifndef SERIALLOGIC_H
#define SERIALLOGIC_H

#include <QObject>
#include "serialport.h"

class SerialLogic : public SerialPort
{
    Q_OBJECT
    struct stmInfo{
       char stmCmd;
       QByteArray stmData;
    };
public:
    explicit SerialLogic(QObject *parent = nullptr);
    virtual void serialDataHandler(const QByteArray& data);//校验并处理数据
    void serialPackData(const stmInfo& info);//校验并处理数据
private:
    bool serialDataCheck();//检验数据帧是否正确
    void serialDataHandle(const stmInfo& info);//处理数据
    QString taskName(char stmCmd);
signals:

public slots:

private:
    QByteArray m_receivedData = "";//从串口接受到待解析处理的数据帧
    stmInfo m_stmData;
};

#endif // SERIALLOGIC_H
