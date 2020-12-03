#include "mainwindow.h"
#include <QApplication>
#include <myhelper.h>
#include <mydialog.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //翻译者 在实例化 QApplication a之后 实例化MainWindow w之前
    MyHelper::setTranslator("/home/jbyyy/QH-KJ/QTAPP/QT界面/jby总结/常规积累/myHelper/myHelper/test.qm");

    MainWindow w;

    w.show();
    App::ConfigFile = QString("%1/%2.ini").arg(App::appPath()).arg(App::appName());
    App::readConfig();

    MyHelper::setCode();//文本编码格式
    MyHelper::initRand();//初始化随机数

    myDialog::showTipBox("title","tip11");
    myDialog::showMessageBox("message");
    myDialog::showInputBox("input box");
    QString date1,date2;
    myDialog::showDateSelect(date1,date2);
    myDialog::showMessageBoxInfo("messageInfo");
    myDialog::showMessageBoxError("messae error");
    myDialog::showMessageBoxQuestion("message qusetion");

    //QStringList str = myDialog::showQFileDialog();
    //myDialog::showQFontDialog();
    qDebug() << myDialog::showQColorDialog();
    //启动托盘类
    //w.hide();
    TrayIcon::Instance()->setMainWidget(&w);
    TrayIcon::Instance()->setIcon(":/image/image/main.ico");
    TrayIcon::Instance()->setToolTip("w.windowTitle()");
    TrayIcon::Instance()->setVisible(true);
    QObject::connect(&w, SIGNAL(destroyed(QObject *)), TrayIcon::Instance(), SLOT(closeAll()));

    return a.exec();
}
