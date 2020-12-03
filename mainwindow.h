#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSerialPort>

class SettingsDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initActions();//初始化Action
    void initSerial();//初始化串口
    void showStatusMessage(const QString &message);//状态栏显示
private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();//关于程序简洁
    void handleError(QSerialPort::SerialPortError error);

    void readData();            //读取串口数据
    void sendData();            //发送串口数据
    void sendData(const QString& data);//发送串口数据带参数
    void saveData();            //保存串口数据
    void append(int type, const QString &data, bool clear = false);

private:

    Ui::MainWindow *ui;
    QPushButton *button;
    SettingsDialog *m_settingsDialog;
    QSerialPort *serial;
    QLabel *serialStatus;

};

#endif // MAINWINDOW_H
