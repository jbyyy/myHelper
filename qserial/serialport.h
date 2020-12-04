#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QMutex>
#include <QObject>

#include <QSerialPort>
#include "settingsdialog.h"

class SettingsDialog;

class SerialPort : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(AppendType)
public:
    enum State{
        ConnectSuccess = 0, //串口连接成功
        ConnectFailed = 1,  //串口连接失败
        PortError = 2,       //串口异常错误
        Disconnect = 3       //串口未连接
    };
    enum AppendType{
        TypeSend =0,         //串口发送
        TypeRecive =1,       //串口接受
        TypeMessage =2,      //串口信息
        TypeCheckFailed =3,  //校验错误
        TypeCheckSuccess =4  //校验成功
    };

    SerialPort(QWidget *parent = nullptr);
    ~SerialPort();


    bool OpenSerialPort();   //串口连接
    void CloseSerialPort();  //串口断开
    SettingsDialog::Settings GetSerialSettingInfo();//获取串口信息的结构体
    QString GetSerialErrorInfo();//显示串口错误信息
    void ShowSerialSettingDialog(bool show = true);//显示串口设置界面
    void SendData(const QString& data,bool hex = true);//发送串口数据带参数
private:
    void initSerial();//初始化串口
    virtual void serialDataHandler(const QByteArray& data) {;};//校验并处理数据
signals:
    void serialPortState(SerialPort::State);               //串口状态
    void serialPortAppend(SerialPort::AppendType,QString); //串口信息显示
private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();            //读取串口数据
    //void append(int type, const QString &data, bool clear = false);

private:
    QSerialPort *m_serial = nullptr;
    SettingsDialog *m_settingsDialog;
    QMutex dataMutex;
};

#endif // SERIALPORT_H
