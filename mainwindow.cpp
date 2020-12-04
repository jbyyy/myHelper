#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <myhelper.h>
#include <settingsdialog.h>
#include <mySavelog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyHelper::setFormInCenter(this);//界面居中
    serialStatus = new QLabel;
    ui->statusBar->addWidget(serialStatus);
    m_serialPort = new SerialLogic(this);
    initActions();//设置aciton
    ui->cboxData->setFocus();
    connect(m_serialPort,&SerialLogic::serialPortAppend,this,&MainWindow::append);

    SaveLog::Instance()->setPath(qApp->applicationDirPath());
    qDebug() << "SaveLog Path:" << qApp->applicationDirPath();
    SaveLog::Instance()->start();//将qDebug状态到logger中
    SaveLog::Instance()->setToNet(false);//是否将log信息回复给连接上来的TCP客户端
}

MainWindow::~MainWindow()
{
    delete ui;
}
//打开串口
void MainWindow::openSerialPort()
{

    if ( m_serialPort->OpenSerialPort()) {
        // console->setEnabled(true);
        // console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        SettingsDialog::Settings p = m_serialPort->GetSerialSettingInfo();
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serialPort->GetSerialErrorInfo());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    m_serialPort->CloseSerialPort();
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
    /*qDebug() << "Qserial Error" << error;
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }*/
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
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionConfigure, &QAction::triggered,[=]{m_serialPort->ShowSerialSettingDialog();});
}

void MainWindow::showStatusMessage(const QString &message)
{
    serialStatus->setText(message);
}


void MainWindow::readData()
{
   /* if (m_serialPort->bytesAvailable() <= 0) {
        return;
    }
    QByteArray data = serial->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    QString buffer;
    if (ui->ckHexReceive->isChecked()) {
        buffer = MyHelper::byteArrayToHexStr(data);//字节数组转16进制字符串
    } else {
        //buffer = QUIHelper::byteArrayToAsciiStr(data);
        buffer = QString::fromLocal8Bit(data);
    }
    append(1, buffer);
    static int receiveCount = 0;
    receiveCount = receiveCount + data.size();
    ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));

    qDebug() << "readData" << QString(data);*/
    // console->putData(data);
}

void MainWindow::sendData()
{
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        return;
    }
    str = str.replace(" ","");
    sendData(str);
}

void MainWindow::sendData(const QString &data)
{
    static int sendCount = 0;
    if (ui->ckHexSend->isChecked()) {
        m_serialPort->SendData(data,true);
        sendCount = sendCount + data.size()/2;
    } else {
        m_serialPort->SendData(data,false);
    }
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void MainWindow::append(SerialPort::AppendType type, const QString &data)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == SerialPort::TypeSend) {
        strType = "串口发送 >>";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == SerialPort::TypeRecive) {
        strType = "串口接收 <<";
        ui->txtMain->setTextColor(QColor(255, 184, 150));
        static int receiveCount = 0;
        receiveCount = receiveCount + data.length();
        ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));
    } else if (type == SerialPort::TypeCheckSuccess) {
        strType = "校验成功 >>";
        ui->txtMain->setTextColor(QColor("green"));
    }else if (type == SerialPort::TypeCheckFailed) {
        strType = "校验失败 >>";
        ui->txtMain->setTextColor(QColor("red"));
    }
    else if (type == SerialPort::TypeMessage) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}
