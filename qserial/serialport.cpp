#include "SerialPort.h"
#include "myhelper.h"
#include <QDebug>

SerialPort::SerialPort(QWidget *parent)
{
    initSerial();
}

SerialPort::~SerialPort()
{
    delete m_settingsDialog;
}

void SerialPort::initSerial()
{
    m_settingsDialog = new SettingsDialog();//串口设置窗口

    m_serial = new QSerialPort(this);//实例化串口

    connect(m_serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPort::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialPort::readData);
}
//打开串口
bool SerialPort::OpenSerialPort()
{
    if (m_serial == nullptr) {
        return false;
    }
    SettingsDialog::Settings p = m_settingsDialog->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        emit serialPortState(State::ConnectSuccess);
        qDebug() << QString("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
        return true;
    } else {
        emit serialPortState(State::ConnectFailed);
        return false;
    }
}

void SerialPort::CloseSerialPort()
{
    if (m_serial == nullptr || !m_serial->isOpen()) {
        return;
    }
    if (m_serial->isOpen())
    {
        m_serial->flush();
        m_serial->clear();
        m_serial->close();
        emit serialPortState(State::Disconnect);
    }
}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::NoError) return;
    qDebug() << "Qserial Error" << error;
    if (error == QSerialPort::ResourceError) {
        CloseSerialPort();
        emit serialPortState(State::PortError);
    }
}

void SerialPort::readData()
{
    QMutexLocker locker(&dataMutex);
    if (m_serial == nullptr || !m_serial->isOpen()) {
        return;
    }
    if (m_serial->bytesAvailable() <= 0) {
        return;
    }
    QByteArray data = m_serial->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }
    //emit serialPortAppend(AppendType::TypeRecive,data);
    serialDataHandler(data);
}

void SerialPort::SendData(const QString &data,bool hex)
{
    QMutexLocker locker(&dataMutex);
    if (m_serial == nullptr || !m_serial->isOpen()) {
        return;
    }

    QByteArray buffer;

    if (hex) {
        buffer = MyHelper::hexStrToByteArray(data);
    } else {
        buffer = MyHelper::asciiStrToByteArray(data);
    }

    m_serial->write(buffer);
    emit serialPortAppend(AppendType::TypeSend,data);
}

SettingsDialog::Settings SerialPort::GetSerialSettingInfo()
{
    return m_settingsDialog->settings();
}

QString SerialPort::GetSerialErrorInfo()
{
    return m_serial->errorString();
}

void SerialPort::ShowSerialSettingDialog(bool show)
{
    if(show){
        m_settingsDialog->show();
    }else{
        m_settingsDialog->hide();
    }
}
