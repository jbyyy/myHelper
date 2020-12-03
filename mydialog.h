#ifndef MYDIALOG_H
#define MYDIALOG_H


#include <QDialog>
#include <QObject>
#include <QtWidgets>


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

class myDialog : public QObject
{
  Q_OBJECT
public:
  static myDialog *Instance();
  explicit myDialog(QObject *parent = nullptr);

  //弹出框
  static int showMessageBox(const QString &info, int type = 0, int closeSec = 0, bool exec = false);
  //弹出消息框
  static void showMessageBoxInfo(const QString &info, int closeSec = 0, bool exec = false);
  //弹出错误框
  static void showMessageBoxError(const QString &info, int closeSec = 0, bool exec = false);
  //弹出询问框
  static int showMessageBoxQuestion(const QString &info);

  //弹出+隐藏右下角信息框
  static void showTipBox(const QString &title, const QString &tip, bool fullScreen = false,
                         bool center = true, int closeSec = 0);
  static void hideTipBox();

  //弹出输入框
  static QString showInputBox(const QString &title, int type = 0, int closeSec = 0,
                              const QString &placeholderText = QString(), bool pwd = false,
                              const QString &defaultValue = QString());
  //弹出日期选择框
  static void showDateSelect(QString &dateStart, QString &dateEnd, const QString &format = "yyyy-MM-dd");

  //弹出文件框
  static QStringList showQFileDialog();

  //弹出字体框
  static QFont showQFontDialog();

  //弹出图像
  static QColor showQColorDialog();


  //获取图形字体
  QFont getIconFont();

  //设置图形字体到标签
  void setIcon(QLabel *lab, const QChar &str, quint32 size = 12);
  //设置图形字体到按钮
  void setIcon(QAbstractButton *btn, const QChar &str, quint32 size = 12);

  //设置无边框窗体
  static void setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle, QLabel *labIco, QPushButton *btnClose, bool tool = true);

protected:

private:
  static QScopedPointer<myDialog> self;
  QFont iconFont;             //图形字体
};

//弹出信息框类
class QUIMessageBox : public QDialog
{
    Q_OBJECT

public:
    static QUIMessageBox *Instance();
    explicit QUIMessageBox(QWidget *parent = 0);
    ~QUIMessageBox();

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIMessageBox> self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout3;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout4;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout4;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIcoMain;
    QSpacerItem *horizontalSpacer1;
    QLabel *labInfo;
    QHBoxLayout *horizontalLayout2;
    QSpacerItem *horizontalSpacer2;
    QPushButton *btnOk;
    QPushButton *btnCancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setMessage(const QString &msg, int type, int closeSec = 0);
};

//右下角弹出框类
class QUITipBox : public QDialog
{
    Q_OBJECT

public:
    static QUITipBox *Instance();
    explicit QUITipBox(QWidget *parent = 0);
    ~QUITipBox();

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUITipBox> self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout2;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QLabel *labInfo;

    QPropertyAnimation *animation;
    bool fullScreen;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setTip(const QString &title, const QString &tip, bool fullScreen = false, bool center = true, int closeSec = 0);
    void hide();
};


//弹出输入框类
class QUIInputBox : public QDialog
{
    Q_OBJECT

public:
    static QUIInputBox *Instance();
    explicit QUIInputBox(QWidget *parent = 0);
    ~QUIInputBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIInputBox> self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout2;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout3;
    QLabel *labInfo;
    QLineEdit *txtValue;
    QComboBox *cboxValue;
    QHBoxLayout *lay;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间
    QString value;                  //当前值

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    QString getValue()const;

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setParameter(const QString &title, int type = 0, int closeSec = 0,
                      QString placeholderText = QString(), bool pwd = false,
                      const QString &defaultValue = QString());

};

//弹出日期选择对话框
class QUIDateSelect : public QDialog
{
    Q_OBJECT

public:
    static QUIDateSelect *Instance();
    explicit QUIDateSelect(QWidget *parent = 0);
    ~QUIDateSelect();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIDateSelect> self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout1;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *labStart;
    QPushButton *btnOk;
    QLabel *labEnd;
    QPushButton *btnClose;
    QDateTimeEdit *dateStart;
    QDateTimeEdit *dateEnd;

private:
    QString startDateTime;          //开始时间
    QString endDateTime;            //结束时间
    QString format;                 //日期时间格式

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    //获取当前选择的开始时间和结束时间
    QString getStartDateTime()  const;
    QString getEndDateTime()    const;

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setFormat(const QString &format);

};

//全局变量控制
class QUIConfig
{
public:
    //全局图标
    static QChar IconMain;          //标题栏左上角图标
    static QChar IconMenu;          //下拉菜单图标
    static QChar IconMin;           //最小化图标
    static QChar IconMax;           //最大化图标
    static QChar IconNormal;        //还原图标
    static QChar IconClose;         //关闭图标

    static QString FontName;        //全局字体名称
    static int FontSize;            //全局字体大小

    //样式表颜色值
    static QString TextColor;       //文字颜色
    static QString PanelColor;      //面板颜色
    static QString BorderColor;     //边框颜色
    static QString NormalColorStart;//正常状态开始颜色
    static QString NormalColorEnd;  //正常状态结束颜色
    static QString DarkColorStart;  //加深状态开始颜色
    static QString DarkColorEnd;    //加深状态结束颜色
    static QString HighColor;       //高亮颜色
};

#endif // MYDIALOG_H
