#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <myhelper.h>
#include <settingsdialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyHelper::setFormInCenter(this);//界面居中
    serialStatus = new QLabel;
    ui->statusBar->addWidget(serialStatus);
    initSerial();
    initActions();//设置aciton
    showStatusMessage("1111");
}

MainWindow::~MainWindow()
{
    delete ui;
}
//! [4]
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = m_settingsDialog->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
       // console->setEnabled(true);
       // console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
   // console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}


void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActions()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    connect(ui->actionAbout_App, &QAction::triggered,this, &MainWindow::about);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, m_settingsDialog, &SettingsDialog::show);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    serialStatus->setText(message);
}

void MainWindow::initSerial()
{
    m_settingsDialog = new SettingsDialog(this);//串口设置窗口
    serial = new QSerialPort(this);//实例化串口

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);

//! [2]
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
//! [2]
   // connect(console, &Console::getData, this, &MainWindow::writeData);
}


void MainWindow::readData()
{
    if (serial->bytesAvailable() <= 4) {
        return;
    }
    QByteArray data = serial->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    /*QString buffer;
    if (ui->ckHexReceive->isChecked()) {
        buffer = MyHelper::byteArrayToHexStr(data);
    } else {
        //buffer = QUIHelper::byteArrayToAsciiStr(data);
        buffer = QString::fromLocal8Bit(data);
    }

    //启用调试则模拟调试数据
    if (ui->ckDebug->isChecked()) {
        int count = App::Keys.count();
        for (int i = 0; i < count; i++) {
            if (buffer.startsWith(App::Keys.at(i))) {
                sendData(App::Values.at(i));
                break;
            }
        }
    }

    append(1, buffer);
    receiveCount = receiveCount + data.size();
    ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));

    //启用网络转发则调用网络发送数据
    if (tcpOk) {
        socket->write(data);
        append(4, QString(buffer));
    }*/

    qDebug() << "readData" << QString(data);
    // console->putData(data);
}

void MainWindow::sendData()
{
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        ui->cboxData->setFocus();
        return;
    }

    sendData(str);

    if (ui->ckAutoClear->isChecked()) {
        ui->cboxData->setCurrentIndex(-1);
        ui->cboxData->setFocus();
    }
}

void MainWindow::sendData(const QString &data)
{
    if (serial == 0 || !serial->isOpen()) {
        return;
    }

    //短信猫调试
    /*if (data.startsWith("AT")) {
        data += "\r";
    }*/

    QByteArray buffer;

    if (ui->ckHexSend->isChecked()) {
        buffer = MyHelper::hexStrToByteArray(data);
    } else {
        buffer = MyHelper::asciiStrToByteArray(data);
    }

    serial->write(buffer);
    append(0, data);
    static int sendCount = 0;
    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void MainWindow::saveData()
{

}

void MainWindow::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

//    if (!isShow) {
//        return;
//    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "串口发送 >>";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        strType = "串口接收 <<";
        ui->txtMain->setTextColor(QColor("red"));
    } else if (type == 2) {
        strType = "处理延时 >>";
        ui->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        strType = "正在校验 >>";
        ui->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        strType = "网络发送 >>";
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    } else if (type == 5) {
        strType = "网络接收 <<";
        ui->txtMain->setTextColor(QColor(255, 107, 107));
    } else if (type == 6) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}
