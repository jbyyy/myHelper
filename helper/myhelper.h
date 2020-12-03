#ifndef MYHELPER_H
#define MYHELPER_H

#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME            qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#define QDATE           qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME           qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME        qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME     qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS   qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))

//定义换行符
#ifdef Q_OS_WIN
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

//定义无边框标题栏高度+对话框最小宽高
#ifdef __arm__
#define TitleMinSize 40
#define DialogMinWidth 350
#define DialogMinHeight 180
#else
#define TitleMinSize 30
#define DialogMinWidth 280
#define DialogMinHeight 150
#endif

#include <QObject>
#include <QDebug>

/******************应用程序全局变量/读写配置参数******************/
#include <QtWidgets>
class App
{
public:
    static QString ConfigFile;          //配置文件路径
    static QString SendFileName;        //发送配置文件名
    static QString DeviceFileName;      //模拟设备数据文件名
    static int CurrentIndex;            //当前索引
    //读写配置参数及其他操作
    static QString appPath();          //获取可执行文件所在路径
    static QString appName();          //获取应用文件名称
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
    static void newConfig();            //以初始值新建配置文件
    static bool checkConfig();          //校验配置文件

    static QStringList Intervals;
    static QStringList Datas;
    static QStringList Keys;
    static QStringList Values;
    static void readSendData();
    static void readDeviceData();
};
/******************积累的帮助函数******************/
#include <QtCore>
#include <QtGui>
#include <QtNetwork>
class MyHelper : public QObject
{
    Q_OBJECT
public:
    //桌面宽度高度
    static int deskWidth();
    static int deskHeight();

    //初始化随机数种子
    static void initRand();

    //初始化数据库
    static void initDb(const QString &dbName);
    //初始化文件,不存在则拷贝
    static void initFile(const QString &sourceName, const QString &targetName);

    //新建目录
    static void newDir(const QString &dirName);

    //写入消息到额外的的消息日志文件
    static void writeInfo(const QString &info, bool needWrite = false, const QString &filePath = "log");
    static void writeError(const QString &info, bool needWrite = false, const QString &filePath = "log");

    //设置全局样式
    static void setStyle(const QString &qssFile);

    //九宫格图片 horzSplit-宫格1/3/7/9宽度  vertSplit-宫格1/3/7/9高度  dstWidth-目标图片宽度  dstHeight-目标图片高度
    static QPixmap ninePatch(const QString &picName, int horzSplit, int vertSplit, int dstWidth, int dstHeight);
    static QPixmap ninePatch(const QPixmap &pix, int horzSplit, int vertSplit, int dstWidth, int dstHeight);

    //设置窗体居中显示
    static void setFormInCenter(QWidget *frm);


    //设置翻译文件
    static void setTranslator(const QString &qmFile = ":/image/qt_zh_CN.qm");
    //设置编码
    static void setCode();
    //设置延时
    static void sleep(int msec);
    //设置系统时间
    static void setSystemDateTime(const QString &year, const QString &month, const QString &day,
                                  const QString &hour, const QString &min, const QString &sec);
    //设置开机自启动
    static void runWithSystem(const QString &strName, const QString &strPath, bool autoRun = true);

    //判断是否是IP地址
    static bool isIP(const QString &ip);
    //判断是否是MAC地址
    static bool isMac(const QString &mac);
    //判断是否是合法的电话号码
    static bool isTel(const QString &tel);
    //判断是否是合法的邮箱地址
    static bool isEmail(const QString &email);


    //16进制字符串转10进制
    static int strHexToDecimal(const QString &strHex);
    //10进制字符串转10进制
    static int strDecimalToDecimal(const QString &strDecimal);
    //2进制字符串转10进制
    static int strBinToDecimal(const QString &strBin);

    //16进制字符串转2进制字符串
    static QString strHexToStrBin(const QString &strHex);
    //10进制转2进制字符串一个字节
    static QString decimalToStrBin1(int decimal);
    //10进制转2进制字符串两个字节
    static QString decimalToStrBin2(int decimal);
    //10进制转16进制字符串,补零.
    static QString decimalToStrHex(int decimal);

    //int转字节数组
    static QByteArray intToByte(int i);
    static QByteArray intToByteRec(int i);

    //字节数组转int
    static int byteToInt(const QByteArray &data);
    static int byteToIntRec(const QByteArray &data);
    static quint32 byteToUInt(const QByteArray &data);
    static quint32 byteToUIntRec(const QByteArray &data);

    //ushort转字节数组
    static QByteArray ushortToByte(ushort i);
    static QByteArray ushortToByteRec(ushort i);

    //字节数组转ushort
    static int byteToUShort(const QByteArray &data);
    static int byteToUShortRec(const QByteArray &data);

    //异或加密算法
    static QString getXorEncryptDecrypt(const QString &str, char key);
    //异或校验
    static uchar getOrCode(const QByteArray &data);
    //计算校验码
    static uchar getCheckCode(const QByteArray &data);

    //CRC校验
    static quint16 getRevCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getModbus16(quint8 *data, int len);
    static QByteArray getCRCCode(const QByteArray &data);

    //字节数组转Ascii字符串
    static QString byteArrayToAsciiStr(const QByteArray &data);
    //16进制字符串转字节数组
    static QByteArray hexStrToByteArray(const QString &str);
    static char convertHexChar(char ch);

    //Ascii字符串转字节数组
    static QByteArray asciiStrToByteArray(const QString &str);
    //字节数组转16进制字符串
    static QString byteArrayToHexStr(const QByteArray &data);

    //获取保存的文件
    static QString getSaveName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());
    //获取选择的文件
    static QString getFileName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());
    //非阻塞保存文件对话框
    static QString saveFileName(const QString &filter, const QString &defaultDir = "", const QString &fileName = "");
    //获取选择的文件集合
    static QStringList getFileNames(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());
    //获取选择的目录
    static QString getFolderName();

    //获取文件名,含拓展名
    static QString getFileNameWithExtension(const QString &strFilePath);
    //获取选择文件夹中的文件
    static QStringList getFolderFileNames(const QStringList &filter);

    //文件夹是否存在
    static bool folderIsExist(const QString &strFolder);
    //文件是否存在
    static bool fileIsExist(const QString &strFile);
    //复制文件
    static bool copyFile(const QString &sourceFile, const QString &targetFile);
    //删除文件夹下所有文件
    static void deleteDirectory(const QString &path);

    //判断IP地址及端口是否在线
    static bool ipLive(const QString &ip, int port, int timeout = 1000);
    //获取网页所有源代码
    static QString getHtml(const QString &url);
    //获取本机公网IP地址
    static QString getNetIP(const QString &webCode);
    //获取本机IP
    static QString getLocalIP();
    //获取本机IP地址集合
    static QStringList getLocalIPs();
    //Url地址转为IP地址
    static QString urlToIP(const QString &url);

    //字符串补全 输出是16进制 value需要改变的数值 length输出长度 type是多少进制显示
    static QString getValue(int value,int length = 2,int type = 16);
    //判断是否通外网
    static bool isWebOk();
};
/****************UI_QSS*****************/
class QPushButton;
class QLineEdit;
class QProgressBar;
class QSlider;
class QRadioButton;
class QCheckBox;
class QScrollBar;
#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT FlatUI : public QObject
#else
class FlatUI : public QObject
#endif

{
    Q_OBJECT
public:
    //设置按钮样式
    static QString setPushButtonQss(QPushButton *btn,                               //按钮对象
                                    int radius = 5,                                 //圆角半径
                                    int padding = 8,                                //间距
                                    const QString &normalColor = "#34495E",         //正常颜色
                                    const QString &normalTextColor = "#FFFFFF",     //文字颜色
                                    const QString &hoverColor = "#4E6D8C",          //悬停颜色
                                    const QString &hoverTextColor = "#F0F0F0",      //悬停文字颜色
                                    const QString &pressedColor = "#2D3E50",        //按下颜色
                                    const QString &pressedTextColor = "#B8C6D1");   //按下文字颜色

    //设置文本框样式
    static QString setLineEditQss(QLineEdit *txt,                                   //文本框对象
                                  int radius = 3,                                   //圆角半径
                                  int borderWidth = 2,                              //边框大小
                                  const QString &normalColor = "#DCE4EC",           //正常颜色
                                  const QString &focusColor = "#34495E");           //选中颜色

    //设置进度条样式
    static QString setProgressQss(QProgressBar *bar,
                                     int barHeight = 8,                             //进度条高度
                                     int barRadius = 5,                             //进度条半径
                                     int fontSize = 9,                              //文字字号
                                     const QString &normalColor = "#E8EDF2",        //正常颜色
                                     const QString &chunkColor = "#E74C3C");        //进度颜色

    //设置滑块条样式
    static QString setSliderQss(QWidget *slider,                                    //滑动条对象
                                int sliderHeight = 8,                               //滑动条高度
                                const QString &normalColor = "#E8EDF2",             //正常颜色
                                const QString &grooveColor = "#1ABC9C",             //滑块颜色
                                const QString &handleBorderColor = "#1ABC9C",       //指示器边框颜色
                                const QString &handleColor = "#FFFFFF");            //指示器颜色

    //设置单选框样式
    static QString setRadioButtonQss(QRadioButton *rbtn,                            //单选框对象
                                     int indicatorRadius = 8,                       //指示器圆角角度
                                     const QString &normalColor = "#D7DBDE",        //正常颜色
                                     const QString &checkColor = "#34495E");        //选中颜色

    //设置滚动条样式
    static QString setScrollBarQss(QWidget *scroll,                                 //滚动条对象
                                   int radius = 6,                                  //圆角角度
                                   int min = 120,                                   //指示器最小长度
                                   int max = 12,                                    //滚动条最大长度
                                   const QString &bgColor = "#606060",              //背景色
                                   const QString &handleNormalColor = "#34495E",    //指示器正常颜色
                                   const QString &handleHoverColor = "#1ABC9C",     //指示器悬停颜色
                                   const QString &handlePressedColor = "#E74C3C");  //指示器按下颜色
};
/***************托盘图标控件***************/
/**  使用方法  main.cpp
 *  MainWindow w;
    w.show();
    //w.setFixedSize(w.sizeHint());//设置固定大小

    //启动托盘类
    TrayIcon::Instance()->setMainWidget(&w);
    TrayIcon::Instance()->setIcon(":/image/image/main.ico");
    TrayIcon::Instance()->setToolTip("w.windowTitle()");
    TrayIcon::Instance()->setVisible(true);
    QObject::connect(&w, SIGNAL(destroyed(QObject *)), TrayIcon::Instance(), SLOT(closeAll()));
*/
#include <QSystemTrayIcon>

class QMenu;

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT TrayIcon : public QObject
#else
class TrayIcon : public QObject
#endif

{
    Q_OBJECT
public:
    static TrayIcon *Instance();
    explicit TrayIcon(QObject *parent = 0);

private:
    static QScopedPointer<TrayIcon> self;
    QWidget *mainWidget;            //对应所属主窗体
    QSystemTrayIcon *trayIcon;      //托盘对象
    QMenu *menu;                    //右键菜单
    bool exitDirect;                //是否直接退出

private slots:
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);

public:
    bool getVisible()       const;

public Q_SLOTS:
    //设置是否直接退出,如果不是直接退出则发送信号给主界面
    void setExitDirect(bool exitDirect);

    //设置所属主窗体
    void setMainWidget(QWidget *mainWidget);

    //显示消息
    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 5000);

    //设置图标
    void setIcon(const QString &strIcon);
    //设置提示信息
    void setToolTip(const QString &tip);
    //设置是否可见
    void setVisible(bool visible);
    //退出所有
    void closeAll();

Q_SIGNALS:
    void trayIconExit();
};

#endif // MYHELPER_H
