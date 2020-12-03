#include "myhelper.h"


/******************应用程序全局变量/读写配置参数******************/
QString App::ConfigFile = "config.ini";
QString App::SendFileName = "send.txt";
QString App::DeviceFileName = "device.txt";
int     App::CurrentIndex = 0;
QString App::appPath()
{
#ifdef Q_OS_ANDROID
  //return QString("/sdcard/Android/%1").arg(appName());
  return QString("/storage/emulated/0/%1").arg(appName());
#else
  return qApp->applicationDirPath();
#endif
}
QString App::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
    }
    return name;
}
void App::readConfig()
{
  if (!checkConfig()) {
    return;
  }

  QSettings set(App::ConfigFile, QSettings::IniFormat);

  set.beginGroup("AppConfig");
  App::CurrentIndex = set.value("CurrentIndex").toInt();
  set.endGroup();
}

void App::writeConfig()
{
  QSettings set(App::ConfigFile, QSettings::IniFormat);

  set.beginGroup("AppConfig");
  set.setValue("CurrentIndex", App::CurrentIndex);
  set.endGroup();
}

void App::newConfig()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#endif
  writeConfig();
}

bool App::checkConfig()
{
  //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
  qDebug() << App::ConfigFile;
  QFile file(App::ConfigFile);
  if (file.size() == 0) {
    newConfig();
    return false;
  }

  //如果配置文件不完整,则以初始值继续运行,并生成配置文件
  if (file.open(QFile::ReadOnly)) {
    bool ok = true;
    while (!file.atEnd()) {
      QString line = file.readLine();
      line = line.replace("\r", "");
      line = line.replace("\n", "");
      QStringList list = line.split("=");

      if (list.count() == 2) {
        if (list.at(1) == "") {
          ok = false;
          break;
        }
      }
    }

    if (!ok) {
      newConfig();
      return false;
    }
  } else {
    newConfig();
    return false;
  }
  return true;
}
QStringList App::Intervals = QStringList();
QStringList App::Datas = QStringList();
QStringList App::Keys = QStringList();
QStringList App::Values = QStringList();

void App::readSendData()
{
    //读取发送数据列表
    App::Datas.clear();
    QString fileName = QString("%1/%2").arg(App::appPath()).arg(App::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                App::Datas.append(line);
            }
        }
        file.close();
    }
}

void App::readDeviceData()
{
    //读取转发数据列表
    App::Keys.clear();
    App::Values.clear();
    QString fileName = QString("%1/%2").arg(App::appPath()).arg(App::DeviceFileName);
    qDebug() << fileName;
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            //qDebug() << fileName << line;

            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                App::Keys.append(key);
                App::Values.append(value);
            }
        }
        file.close();
    }
    //qDebug() << App::Keys << App::Values;
}

/******************积累的帮助函数******************/
int MyHelper::deskWidth()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int width = 0;
    if (width == 0) {
        width = qApp->desktop()->availableGeometry().width();
    }
    return width;
}

int MyHelper::deskHeight()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int height = 0;
    if (height == 0) {
        height = qApp->desktop()->availableGeometry().height();
    }
    return height;
}

void MyHelper::initRand()
{
    //初始化随机数种子
    QTime t = QTime::currentTime();
    qsrand(t.msec() + t.second() * 1000);
}

void MyHelper::initDb(const QString &dbName)
{
    initFile(QString(":/%1.db").arg(App::appName()), dbName);
}

void MyHelper::initFile(const QString &sourceName, const QString &targetName)
{
    //判断文件是否存在,不存在则从资源文件复制出来
    QFile file(targetName);
    if (!file.exists() || file.size() == 0) {
        file.remove();
        MyHelper::copyFile(sourceName, targetName);
    }
}

void MyHelper::newDir(const QString &dirName)
{
    QString strDir = dirName;

    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!strDir.startsWith("/") && !strDir.contains(":/")) {
        strDir = QString("%1/%2").arg(App::appPath()).arg(strDir);
    }

    QDir dir(strDir);
    if (!dir.exists()) {
        dir.mkpath(strDir);
    }
}

void MyHelper::writeInfo(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runinfo_%4.txt").arg(App::appPath())
                       .arg(filePath).arg(App::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}

void MyHelper::writeError(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runerror_%4.txt").arg(App::appPath())
                       .arg(filePath).arg(App::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}


void MyHelper::setStyle(const QString& qssFile)
{
  //加载样式表
  QString qss;
  QFile file(qssFile);
  //QFile file(":/qss/psblack.css");
  //QFile file(":/qss/flatwhite.css");
  //QFile file(":/qss/lightblue.css");
  if (file.open(QFile::ReadOnly)) {
#if 1
      //用QTextStream读取样式文件不用区分文件编码 带bom也行
      QStringList list;
      QTextStream in(&file);
      //in.setCodec("utf-8");
      while (!in.atEnd()) {
          QString line;
          in >> line;
          list << line;
      }

      qss = list.join("\n");
#else
      //用readAll读取默认支持的是ANSI格式,如果不小心用creator打开编辑过了很可能打不开
      qss = QLatin1String(file.readAll());
#endif
      QString paletteColor = qss.mid(20, 7);
      qDebug() << "paletteColor" << paletteColor;
      qApp->setPalette(QPalette(QColor(paletteColor)));
      qApp->setStyleSheet(qss);
      file.close();
  }
}


QPixmap MyHelper::ninePatch(const QString &picName, int horzSplit, int vertSplit, int dstWidth, int dstHeight)
{
    QPixmap pix(picName);
    return ninePatch(pix, horzSplit, vertSplit, dstWidth, dstHeight);
}

QPixmap MyHelper::ninePatch(const QPixmap &pix, int horzSplit, int vertSplit, int dstWidth, int dstHeight)
{
    int pixWidth = pix.width();
    int pixHeight = pix.height();

    QPixmap pix1 = pix.copy(0, 0, horzSplit, vertSplit);
    QPixmap pix2 = pix.copy(horzSplit, 0, pixWidth - horzSplit * 2, vertSplit);
    QPixmap pix3 = pix.copy(pixWidth - horzSplit, 0, horzSplit, vertSplit);

    QPixmap pix4 = pix.copy(0, vertSplit, horzSplit, pixHeight - vertSplit * 2);
    QPixmap pix5 = pix.copy(horzSplit, vertSplit, pixWidth - horzSplit * 2, pixHeight - vertSplit * 2);
    QPixmap pix6 = pix.copy(pixWidth - horzSplit, vertSplit, horzSplit, pixHeight - vertSplit * 2);

    QPixmap pix7 = pix.copy(0, pixHeight - vertSplit, horzSplit, vertSplit);
    QPixmap pix8 = pix.copy(horzSplit, pixHeight - vertSplit, pixWidth - horzSplit * 2, pixWidth - horzSplit * 2);
    QPixmap pix9 = pix.copy(pixWidth - horzSplit, pixHeight - vertSplit, horzSplit, vertSplit);

    //保持高度拉宽
    pix2 = pix2.scaled(dstWidth - horzSplit * 2, vertSplit, Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix4 = pix4.scaled(horzSplit, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //宽高都缩放
    pix5 = pix5.scaled(dstWidth - horzSplit * 2, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix6 = pix6.scaled(horzSplit, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //保持高度拉宽
    pix8 = pix8.scaled(dstWidth - horzSplit * 2, vertSplit);

    //生成宽高图片并填充透明背景颜色
    QPixmap resultImg(dstWidth, dstHeight);
    resultImg.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&resultImg);
    if (!resultImg.isNull()) {
        painter.drawPixmap(0, 0, pix1);
        painter.drawPixmap(horzSplit, 0, pix2);
        painter.drawPixmap(dstWidth - horzSplit, 0, pix3);

        painter.drawPixmap(0, vertSplit, pix4);
        painter.drawPixmap(horzSplit, vertSplit, pix5);
        painter.drawPixmap(dstWidth - horzSplit, vertSplit, pix6);

        painter.drawPixmap(0, dstHeight - vertSplit, pix7);
        painter.drawPixmap(horzSplit, dstHeight - vertSplit, pix8);
        painter.drawPixmap(dstWidth - horzSplit, dstHeight - vertSplit, pix9);
    }
    painter.end();
    return resultImg;
}


void MyHelper::setFormInCenter(QWidget *frm)
{
    int frmX = frm->width();
    int frmY = frm->height();
    QDesktopWidget w;
    int deskWidth = w.availableGeometry().width();
    int deskHeight = w.availableGeometry().height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
   // qDebug() << deskWidth / 2 - frmX / 2 << deskHeight / 2 - frmY / 2;
    frm->move(movePoint);
}


void MyHelper::setTranslator(const QString &qmFile)
{
    QTranslator *translator = new QTranslator(qApp);
    translator->load(qmFile);
    qApp->installTranslator(translator);
}

void MyHelper::setCode()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif
}

void MyHelper::sleep(int msec)
{
    if (msec > 0) {
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        QTime endTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < endTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
#else
        QThread::msleep(msec);
#endif
    }
}
/**
 * @brief MyHelper::setSystemDateTime
 * sudo date 100921052020.34
   2020年 10月 09日 星期五 21:05:34 CST
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param min
 * @param sec
 */
void MyHelper::setSystemDateTime(const QString &year, const QString &month, const QString &day, const QString &hour, const QString &min, const QString &sec)
{
#ifdef Q_OS_WIN
    QProcess p(0);
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
#else
    QString cmd = QString("sudo date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
    system(cmd.toLatin1());
    system("sudo hwclock -w");
#endif
}

void MyHelper::runWithSystem(const QString &strName, const QString &strPath, bool autoRun)
{
#ifdef Q_OS_WIN
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(strName, autoRun ? strPath : "");
#endif
}

bool MyHelper::isIP(const QString &ip)
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}

bool MyHelper::isMac(const QString &mac)
{
    QRegExp RegExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
    return RegExp.exactMatch(mac);
}

bool MyHelper::isTel(const QString &tel)
{
    if (tel.length() != 11) {
        return false;
    }

    if (!tel.startsWith("13") && !tel.startsWith("14") && !tel.startsWith("15") && !tel.startsWith("18")) {
        return false;
    }

    return true;
}

bool MyHelper::isEmail(const QString &email)
{
    if (!email.contains("@") || !email.contains(".com")) {
        return false;
    }

    return true;
}

int MyHelper::strHexToDecimal(const QString &strHex)
{
    bool ok;
    return strHex.toInt(&ok, 16);
}

int MyHelper::strDecimalToDecimal(const QString &strDecimal)
{
    bool ok;
    return strDecimal.toInt(&ok, 10);
}

int MyHelper::strBinToDecimal(const QString &strBin)
{
    bool ok;
    return strBin.toInt(&ok, 2);
}

QString MyHelper::strHexToStrBin(const QString &strHex)
{
    uchar decimal = strHexToDecimal(strHex);
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len < 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString MyHelper::decimalToStrBin1(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString MyHelper::decimalToStrBin2(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 16) {
        for (int i = 0; i < 16 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString MyHelper::decimalToStrHex(int decimal)
{
    QString temp = QString::number(decimal, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }

    return temp;
}

QByteArray MyHelper::intToByte(int i)
{
    QByteArray result;
    result.resize(4);
    result[3] = (uchar)(0x000000ff & i);
    result[2] = (uchar)((0x0000ff00 & i) >> 8);
    result[1] = (uchar)((0x00ff0000 & i) >> 16);
    result[0] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

QByteArray MyHelper::intToByteRec(int i)
{
    QByteArray result;
    result.resize(4);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    result[2] = (uchar)((0x00ff0000 & i) >> 16);
    result[3] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

int MyHelper::byteToInt(const QByteArray &data)
{
    int i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

int MyHelper::byteToIntRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

quint32 MyHelper::byteToUInt(const QByteArray &data)
{
    quint32 i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

quint32 MyHelper::byteToUIntRec(const QByteArray &data)
{
    quint32 i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

QByteArray MyHelper::ushortToByte(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[1] = (uchar)(0x000000ff & i);
    result[0] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

QByteArray MyHelper::ushortToByteRec(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

int MyHelper::byteToUShort(const QByteArray &data)
{
    int i = data.at(1) & 0x000000FF;
    i |= ((data.at(0) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

int MyHelper::byteToUShortRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000FF;
    i |= ((data.at(1) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

QString MyHelper::getXorEncryptDecrypt(const QString &str, char key)
{
    QByteArray data = str.toLatin1();
    int size = data.size();
    for (int i = 0; i < size; i++) {
        data[i] = data[i] ^ key;
    }

    return QLatin1String(data);
}

uchar MyHelper::getOrCode(const QByteArray &data)
{
    int len = data.length();
    uchar result = 0;

    for (int i = 0; i < len; i++) {
        result ^= data.at(i);
    }

    return result;
}

uchar MyHelper::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    uchar temp = 0;

    for (uchar i = 0; i < len; i++) {
        temp += data.at(i);
    }

    return temp % 256;
}

//函数功能：计算CRC16
//参数1：*data 16位CRC校验数据，
//参数2：len   数据流长度
//参数3：init  初始化值
//参数4：table 16位CRC查找表

//逆序CRC计算
quint16 MyHelper::getRevCrc_16(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 >> 8;
        cRc_16 = (cRc_16 << 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//正序CRC计算
quint16 MyHelper::getCrc_16(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 & 0xff;
        cRc_16 = (cRc_16 >> 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//Modbus CRC16校验
quint16 MyHelper::getModbus16(quint8 *data, int len)
{
    //MODBUS CRC-16表 8005 逆序
    const quint16 table_16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    return getCrc_16(data, len, 0xFFFF, table_16);
}

//CRC16校验
QByteArray MyHelper::getCRCCode(const QByteArray &data)
{
    quint16 result = getModbus16((quint8 *)data.data(), data.length());
    return MyHelper::ushortToByteRec(result);
}

QString MyHelper::byteArrayToAsciiStr(const QByteArray &data)
{
    QString temp;
    int len = data.size();
    for (int i = 0; i < len; i++) {
        //0x20为空格,空格以下都是不可见字符
        char b = data.at(i);

        if (0x00 == b) {
            temp += QString("\\NUL");
        } else if (0x01 == b) {
            temp += QString("\\SOH");
        } else if (0x02 == b) {
            temp += QString("\\STX");
        } else if (0x03 == b) {
            temp += QString("\\ETX");
        } else if (0x04 == b) {
            temp += QString("\\EOT");
        } else if (0x05 == b) {
            temp += QString("\\ENQ");
        } else if (0x06 == b) {
            temp += QString("\\ACK");
        } else if (0x07 == b) {
            temp += QString("\\BEL");
        } else if (0x08 == b) {
            temp += QString("\\BS");
        } else if (0x09 == b) {
            temp += QString("\\HT");
        } else if (0x0A == b) {
            temp += QString("\\LF");
        } else if (0x0B == b) {
            temp += QString("\\VT");
        } else if (0x0C == b) {
            temp += QString("\\FF");
        } else if (0x0D == b) {
            temp += QString("\\CR");
        } else if (0x0E == b) {
            temp += QString("\\SO");
        } else if (0x0F == b) {
            temp += QString("\\SI");
        } else if (0x10 == b) {
            temp += QString("\\DLE");
        } else if (0x11 == b) {
            temp += QString("\\DC1");
        } else if (0x12 == b) {
            temp += QString("\\DC2");
        } else if (0x13 == b) {
            temp += QString("\\DC3");
        } else if (0x14 == b) {
            temp += QString("\\DC4");
        } else if (0x15 == b) {
            temp += QString("\\NAK");
        } else if (0x16 == b) {
            temp += QString("\\SYN");
        } else if (0x17 == b) {
            temp += QString("\\ETB");
        } else if (0x18 == b) {
            temp += QString("\\CAN");
        } else if (0x19 == b) {
            temp += QString("\\EM");
        } else if (0x1A == b) {
            temp += QString("\\SUB");
        } else if (0x1B == b) {
            temp += QString("\\ESC");
        } else if (0x1C == b) {
            temp += QString("\\FS");
        } else if (0x1D == b) {
            temp += QString("\\GS");
        } else if (0x1E == b) {
            temp += QString("\\RS");
        } else if (0x1F == b) {
            temp += QString("\\US");
        } else if (0x7F == b) {
            temp += QString("\\x7F");
        } else if (0x5C == b) {
            temp += QString("\\x5C");
        } else if (0x20 >= b) {
            temp += QString("\\x%1").arg(decimalToStrHex((quint8)b));
        } else {
            temp += QString("%1").arg(b);
        }
    }

    return temp.trimmed();
}

QByteArray MyHelper::hexStrToByteArray(const QString &str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = str.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = str.at(i).toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);
    return senddata;
}

char MyHelper::convertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return ch - 0x30;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    } else {
        return (-1);
    }
}

QByteArray MyHelper::asciiStrToByteArray(const QString &str)
{
    QByteArray buffer;
    int len = str.length();
    QString letter;
    QString hex;

    for (int i = 0; i < len; i++) {
        letter = str.at(i);

        if (letter == "\\") {
            i++;
            letter = str.mid(i, 1);

            if (letter == "x") {
                i++;
                hex = str.mid(i, 2);
                buffer.append(strHexToDecimal(hex));
                i++;
                continue;
            } else if (letter == "N") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "L") {           //NUL=0x00
                        buffer.append((char)0x00);
                        continue;
                    }
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "K") {           //NAK=0x15
                        buffer.append(0x15);
                        continue;
                    }
                }
            } else if (letter == "S") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "H") {           //SOH=0x01
                        buffer.append(0x01);
                        continue;
                    } else {                    //SO=0x0E
                        buffer.append(0x0E);
                        i--;
                        continue;
                    }
                } else if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "X") {           //STX=0x02
                        buffer.append(0x02);
                        continue;
                    }
                } else if (hex == "I") {        //SI=0x0F
                    buffer.append(0x0F);
                    continue;
                } else if (hex == "Y") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "N") {           //SYN=0x16
                        buffer.append(0x16);
                        continue;
                    }
                } else if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "B") {           //SUB=0x1A
                        buffer.append(0x1A);
                        continue;
                    }
                }
            } else if (letter == "E") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "X") {           //ETX=0x03
                        buffer.append(0x03);
                        continue;
                    } else if (hex == "B") {    //ETB=0x17
                        buffer.append(0x17);
                        continue;
                    }
                } else if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "T") {           //EOT=0x04
                        buffer.append(0x04);
                        continue;
                    }
                } else if (hex == "N") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "Q") {           //ENQ=0x05
                        buffer.append(0x05);
                        continue;
                    }
                } else if (hex == "M") {        //EM=0x19
                    buffer.append(0x19);
                    continue;
                } else if (hex == "S") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "C") {           //ESC=0x1B
                        buffer.append(0x1B);
                        continue;
                    }
                }
            } else if (letter == "A") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "K") {           //ACK=0x06
                        buffer.append(0x06);
                        continue;
                    }
                }
            } else if (letter == "B") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "E") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "L") {           //BEL=0x07
                        buffer.append(0x07);
                        continue;
                    }
                } else if (hex == "S") {        //BS=0x08
                    buffer.append(0x08);
                    continue;
                }
            } else if (letter == "C") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "R") {               //CR=0x0D
                    buffer.append(0x0D);
                    continue;
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "N") {           //CAN=0x18
                        buffer.append(0x18);
                        continue;
                    }
                }
            } else if (letter == "D") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "L") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "E") {           //DLE=0x10
                        buffer.append(0x10);
                        continue;
                    }
                } else if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "1") {           //DC1=0x11
                        buffer.append(0x11);
                        continue;
                    } else if (hex == "2") {    //DC2=0x12
                        buffer.append(0x12);
                        continue;
                    } else if (hex == "3") {    //DC3=0x13
                        buffer.append(0x13);
                        continue;
                    } else if (hex == "4") {    //DC2=0x14
                        buffer.append(0x14);
                        continue;
                    }
                }
            } else if (letter == "F") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "F") {               //FF=0x0C
                    buffer.append(0x0C);
                    continue;
                } else if (hex == "S") {        //FS=0x1C
                    buffer.append(0x1C);
                    continue;
                }
            } else if (letter == "H") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") {               //HT=0x09
                    buffer.append(0x09);
                    continue;
                }
            } else if (letter == "L") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "F") {               //LF=0x0A
                    buffer.append(0x0A);
                    continue;
                }
            } else if (letter == "G") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") {               //GS=0x1D
                    buffer.append(0x1D);
                    continue;
                }
            } else if (letter == "R") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") {               //RS=0x1E
                    buffer.append(0x1E);
                    continue;
                }
            } else if (letter == "U") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") {               //US=0x1F
                    buffer.append(0x1F);
                    continue;
                }
            } else if (letter == "V") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") {               //VT=0x0B
                    buffer.append(0x0B);
                    continue;
                }
            } else if (letter == "\\") {
                //如果连着的是多个\\则对应添加\对应的16进制0x5C
                buffer.append(0x5C);
                continue;
            } else {
                //将对应的\[前面的\\也要加入
                buffer.append(0x5C);
                buffer.append(letter.toLatin1());
                continue;
            }
        }

        buffer.append(str.mid(i, 1).toLatin1());

    }

    return buffer;
}

QString MyHelper::byteArrayToHexStr(const QByteArray &data)
{
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    return temp.trimmed().toUpper();
}

QString MyHelper::getSaveName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getSaveFileName(0, "选择文件", defaultDir, filter);
}

QString MyHelper::getFileName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileName(0, "选择文件", defaultDir, filter);
}

QString MyHelper::saveFileName(const QString &filter, const QString &defaultDir, const QString &fileName)
{
    QString file;
    QFileDialog dialog;
    dialog.setFixedSize(900, 600);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowTitle("保存文件");
    dialog.setLabelText(QFileDialog::Accept, "保存(&S)");
    dialog.setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog.selectFile(fileName);
    dialog.setNameFilter(filter);
    dialog.setDirectory(defaultDir);

    if (dialog.exec() == 1) {
        file = dialog.selectedFiles().value(0);
        file = QFileInfo(file).suffix().isEmpty() ? "" : file;
    }

    return file;
}

QStringList MyHelper::getFileNames(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileNames(0, "选择文件", defaultDir, filter);
}

QString MyHelper::getFolderName()
{
    return QFileDialog::getExistingDirectory();
}

QString MyHelper::getFileNameWithExtension(const QString &strFilePath)
{
    QFileInfo fileInfo(strFilePath);
    return fileInfo.fileName();
}

QStringList MyHelper::getFolderFileNames(const QStringList &filter)
{
    QStringList fileList;
    QString strFolder = QFileDialog::getExistingDirectory();

    if (!strFolder.length() == 0) {
        QDir myFolder(strFolder);

        if (myFolder.exists()) {
            fileList = myFolder.entryList(filter);
        }
    }

    return fileList;
}

bool MyHelper::folderIsExist(const QString &strFolder)
{
    QDir tempFolder(strFolder);
    return tempFolder.exists();
}

bool MyHelper::fileIsExist(const QString &strFile)
{
    QFile tempFile(strFile);
    return tempFile.exists();
}

bool MyHelper::copyFile(const QString &sourceFile, const QString &targetFile)
{
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

void MyHelper::deleteDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();

    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}

bool MyHelper::ipLive(const QString &ip, int port, int timeout)
{
    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(ip, port);
    eventLoop.exec();
    bool ok = (tcpSocket.state() == QAbstractSocket::ConnectedState);
    return ok;
}

QString MyHelper::getHtml(const QString &url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    return QString(responseData);
}

QString MyHelper::getNetIP(const QString &webCode)
{
    QString web = webCode;
    web = web.replace(' ', "");
    web = web.replace("\r", "");
    web = web.replace("\n", "");
    QStringList list = web.split("<br/>");
    QString tar = list.at(3);
    QStringList ip = tar.split("=");
    return ip.at(1);
}

QString MyHelper::getLocalIP()
{
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    QStringList ips = getLocalIPs();
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            break;
        }
    }

    return ip;
}

QStringList MyHelper::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface  &netInterface, netInterfaces) {
            //移除虚拟机和抓包工具的虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") || humanReadableName.startsWith("npcap loopback adapter")) {
                continue;
            }

            //过滤当前网络接口
            bool flag = (netInterface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast | QNetworkInterface::CanMulticast));
            if (flag) {
                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                foreach (QNetworkAddressEntry addr, addrs) {
                    //只取出IPV4的地址
                    if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        QString ip4 = addr.ip().toString();
                        if (ip4 != "127.0.0.1") {
                            ips.append(ip4);
                        }
                    }
                }
            }
        }
    }

    return ips;
}

QString MyHelper::urlToIP(const QString &url)
{
    QHostInfo host = QHostInfo::fromName(url);
    return host.addresses().at(0).toString();
}

QString MyHelper::getValue(int value,int length, int type)
{
    QString result = QString("%1").arg(value,length,type,QLatin1Char('0'));
    return result;
}

bool MyHelper::isWebOk()
{
    //能接通百度IP说明可以通外网
    return ipLive("47.96.232.122", 5975);
}
/****************UI_QSS*****************/
#pragma execution_character_set("utf-8")
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qprogressbar.h"
#include "qslider.h"
#include "qradiobutton.h"
#include "qcheckbox.h"
#include "qscrollbar.h"
#include "qdebug.h"

QString FlatUI::setPushButtonQss(QPushButton *btn, int radius, int padding,
                                 const QString &normalColor,
                                 const QString &normalTextColor,
                                 const QString &hoverColor,
                                 const QString &hoverTextColor,
                                 const QString &pressedColor,
                                 const QString &pressedTextColor)
{
  QStringList list;
  list.append(QString("QPushButton{border-style:none;padding:%1px;border-radius:%2px;color:%3;background:%4;}")
              .arg(padding).arg(radius).arg(normalTextColor).arg(normalColor));
  list.append(QString("QPushButton:hover{color:%1;background:%2;}")
              .arg(hoverTextColor).arg(hoverColor));
  list.append(QString("QPushButton:pressed{color:%1;background:%2;}")
              .arg(pressedTextColor).arg(pressedColor));

  QString qss = list.join("");
  btn->setStyleSheet(qss);
  return qss;
}

QString FlatUI::setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                               const QString &normalColor,
                               const QString &focusColor)
{
  QStringList list;
  list.append(QString("QLineEdit{border-style:none;padding:3px;border-radius:%1px;border:%2px solid %3;}")
              .arg(radius).arg(borderWidth).arg(normalColor));
  list.append(QString("QLineEdit:focus{border:%1px solid %2;}")
              .arg(borderWidth).arg(focusColor));

  QString qss = list.join("");
  txt->setStyleSheet(qss);
  return qss;
}

QString FlatUI::setProgressQss(QProgressBar *bar, int barHeight,
                               int barRadius, int fontSize,
                               const QString &normalColor,
                               const QString &chunkColor)
{

  QStringList list;
  list.append(QString("QProgressBar{font:%1pt;background:%2;max-height:%3px;border-radius:%4px;text-align:center;border:1px solid %2;}")
              .arg(fontSize).arg(normalColor).arg(barHeight).arg(barRadius));
  list.append(QString("QProgressBar:chunk{border-radius:%2px;background-color:%1;}")
              .arg(chunkColor).arg(barRadius));

  QString qss = list.join("");
  bar->setStyleSheet(qss);
  return qss;
}

QString FlatUI::setSliderQss(QWidget *slider, int sliderHeight,
                             const QString &normalColor,
                             const QString &grooveColor,
                             const QString &handleBorderColor,
                             const QString &handleColor)
{
  int sliderRadius = sliderHeight / 2;
  int handleWidth = (sliderHeight * 3) / 2 + (sliderHeight / 5);
  int handleRadius = handleWidth / 2;
  int handleOffset = handleRadius / 2;

  //横着的slider  add-page:滚动条拉动时增加的部分 sub-page:滚动条拉动时减少的部分 handle:指示器
  QStringList list;
  list.append(QString("QSlider::horizontal{min-height:%1px;}").arg(sliderHeight * 2));//设置slider最小高度
  //设之槽的倒角和背景色
  list.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
              .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
  //设置最右边边到滑动头位置显示的颜色
  list.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
              .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
  //设置最左边到滑动头位置显示的颜色
  list.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
              .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
  //滑动头
  list.append(QString("QSlider::handle:horizontal{width:%3px;margin-top:-%4px;margin-bottom:-%4px;border-radius:%5px;"
                      "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
              .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

  //偏移一个像素
  handleWidth = handleWidth + 1;
  //竖着的slider
  list.append(QString("QSlider::vertical{min-width:%1px;}").arg(sliderHeight * 2));
  list.append(QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
              .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
  list.append(QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
              .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
  list.append(QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
              .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
  list.append(QString("QSlider::handle:vertical{height:%3px;margin-left:-%4px;margin-right:-%4px;border-radius:%5px;"
                      "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
              .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

  QString qss = list.join("");
  slider->setStyleSheet(qss);
  return qss;
}

QString FlatUI::setRadioButtonQss(QRadioButton *rbtn, int indicatorRadius,
                                  const QString &normalColor,
                                  const QString &checkColor)
{
  int indicatorWidth = indicatorRadius * 2;

  QStringList list;
  list.append(QString("QRadioButton::indicator{border-radius:%1px;width:%2px;height:%2px;}")
              .arg(indicatorRadius).arg(indicatorWidth));
  list.append(QString("QRadioButton::indicator::unchecked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                      "stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(normalColor));
  list.append(QString("QRadioButton::indicator::checked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                      "stop:0 %1,stop:0.3 %1,stop:0.4 #FFFFFF,stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(checkColor));

  QString qss = list.join("");
  rbtn->setStyleSheet(qss);
  return qss;
}

QString FlatUI::setScrollBarQss(QWidget *scroll, int radius, int min, int max,
                                const QString &bgColor,
                                const QString &handleNormalColor,
                                const QString &handleHoverColor,
                                const QString &handlePressedColor)
{
  //滚动条离背景间隔
  int padding = 0;

  QStringList list;

  //handle:指示器,滚动条拉动部分 add-page:滚动条拉动时增加的部分 sub-page:滚动条拉动时减少的部分 add-line:递增按钮 sub-line:递减按钮

  //横向滚动条部分
  list.append(QString("QScrollBar:horizontal{background:%1;padding:%2px;border-radius:%3px;min-height:%4px;max-height:%4px;}")
              .arg(bgColor).arg(padding).arg(radius).arg(max));
  list.append(QString("QScrollBar::handle:horizontal{background:%1;min-width:%2px;border-radius:%3px;}")
              .arg(handleNormalColor).arg(min).arg(radius));
  list.append(QString("QScrollBar::handle:horizontal:hover{background:%1;}")
              .arg(handleHoverColor));
  list.append(QString("QScrollBar::handle:horizontal:pressed{background:%1;}")
              .arg(handlePressedColor));
  list.append(QString("QScrollBar::add-page:horizontal{background:none;}"));
  list.append(QString("QScrollBar::sub-page:horizontal{background:none;}"));
  //list.append(QString("QScrollBar::add-line:horizontal{image:url(/home/jbyyy/QH-KJ/catkin_BZSDJ485/src/communication_gui/bin/image/red.png);}"));
  //list.append(QString("QScrollBar::sub-line:horizontal{image:url(/home/jbyyy/QH-KJ/catkin_BZSDJ485/src/communication_gui/bin/image/black.png);}"));
  list.append(QString("QScrollBar::add-line:horizontal{background:none;}"));
  list.append(QString("QScrollBar::sub-line:horizontal{background:none;}"));

  //纵向滚动条部分
  list.append(QString("QScrollBar:vertical{background:%1;padding:%2px;border-radius:%3px;min-width:%4px;max-width:%4px;}")
              .arg(bgColor).arg(padding).arg(radius).arg(max));
  list.append(QString("QScrollBar::handle:vertical{background:%1;min-height:%2px;border-radius:%3px;}")
              .arg(handleNormalColor).arg(min).arg(radius));
  list.append(QString("QScrollBar::handle:vertical:hover{background:%1;}")
              .arg(handleHoverColor));
  list.append(QString("QScrollBar::handle:vertical:pressed{background:%1;}")
              .arg(handlePressedColor));
  list.append(QString("QScrollBar::add-page:vertical{background:none;}"));
  list.append(QString("QScrollBar::sub-page:vertical{background:none;}"));
  list.append(QString("QScrollBar::add-line:vertical{background:none;}"));
  list.append(QString("QScrollBar::sub-line:vertical{background:none;}"));

  QString qss = list.join("");
  scroll->setStyleSheet(qss);
  return qss;
}
/***************托盘图标控件***************/
#include "qmutex.h"
#include "qmenu.h"
#include "qapplication.h"
#include "qdesktopwidget.h"
#include "qdebug.h"

#include <QtNetwork/QNetworkAccessManager>

QScopedPointer<TrayIcon> TrayIcon::self;
TrayIcon *TrayIcon::Instance()
{
  if (self.isNull()) {
    QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new TrayIcon);
    }
  }
  return self.data();
}

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
  mainWidget = 0;
  trayIcon = new QSystemTrayIcon(this);
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
  menu = new QMenu(QApplication::desktop());
  exitDirect = true;
}

void TrayIcon::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
  case QSystemTrayIcon::Trigger:
  case QSystemTrayIcon::DoubleClick: {
    mainWidget->showNormal();
    break;
  }

  default:
    break;
  }
}

bool TrayIcon::getVisible() const
{
  return trayIcon->isVisible();
}

void TrayIcon::setExitDirect(bool exitDirect)
{
  if (this->exitDirect != exitDirect) {
    this->exitDirect = exitDirect;
  }
}

void TrayIcon::setMainWidget(QWidget *mainWidget)
{
  this->mainWidget = mainWidget;
  menu->addAction("主界面", mainWidget, SLOT(showNormal()));

  if (exitDirect) {
    menu->addAction("退出", this, SLOT(closeAll()));
  } else {
    menu->addAction("退出", this, SIGNAL(trayIconExit()));
  }

  trayIcon->setContextMenu(menu);
}

void TrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msecs)
{
  trayIcon->showMessage(title, msg, icon, msecs);
}

void TrayIcon::setIcon(const QString &strIcon)
{
  trayIcon->setIcon(QIcon(strIcon));
}

void TrayIcon::setToolTip(const QString &tip)
{
  trayIcon->setToolTip(tip);
}

void TrayIcon::setVisible(bool visible)
{
  trayIcon->setVisible(visible);
}

void TrayIcon::closeAll()
{
  trayIcon->hide();
  trayIcon->deleteLater();
  qApp->exit();
}
