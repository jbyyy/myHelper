#include "mydialog.h"
#include "myhelper.h"

QChar QUIConfig::IconMain = QChar(0xf072);
QChar QUIConfig::IconMenu = QChar(0xf0d7);
QChar QUIConfig::IconMin = QChar(0xf068);
QChar QUIConfig::IconMax = QChar(0xf2d2);
QChar QUIConfig::IconNormal = QChar(0xf2d0);
QChar QUIConfig::IconClose = QChar(0xf00d);

#ifdef __arm__
QString QUIConfig::FontName = "WenQuanYi Micro Hei";
int QUIConfig::FontSize = 18;
#else
QString QUIConfig::FontName = "Microsoft Yahei";
int QUIConfig::FontSize = 12;
#endif

QString QUIConfig::TextColor = "#F0F0F0";
QString QUIConfig::PanelColor = "#F0F0F0";
QString QUIConfig::BorderColor = "#F0F0F0";
QString QUIConfig::NormalColorStart = "#F0F0F0";
QString QUIConfig::NormalColorEnd = "#F0F0F0";
QString QUIConfig::DarkColorStart = "#F0F0F0";
QString QUIConfig::DarkColorEnd = "#F0F0F0";
QString QUIConfig::HighColor = "#F0F0F0";

QScopedPointer<myDialog> myDialog::self;
myDialog *myDialog::Instance()
{
  if (self.isNull()) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new myDialog);
    }
  }

  return self.data();
}

myDialog::myDialog(QObject *parent) : QObject(parent)
{
  //判断图形字体是否存在,不存在则加入
  QFontDatabase fontDb;
  if (!fontDb.families().contains("FontAwesome")) {
    int fontId = fontDb.addApplicationFont(":/image/fontawesome-webfont.ttf");
    QStringList fontName = fontDb.applicationFontFamilies(fontId);
    if (fontName.count() == 0) {
      qDebug() << "load fontawesome-webfont.ttf error";
    }
  }

  if (fontDb.families().contains("FontAwesome")) {
    iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
  }
}
int myDialog::showMessageBox(const QString &info, int type, int closeSec, bool exec)
{
  int result = 0;
  if (type == 0) {
    showMessageBoxInfo(info, closeSec, exec);
  } else if (type == 1) {
    showMessageBoxError(info, closeSec, exec);
  } else if (type == 2) {
    result = showMessageBoxQuestion(info);
  }

  return result;
}

void myDialog::showMessageBoxInfo(const QString &info, int closeSec, bool exec)
{
  if (exec) {
    QUIMessageBox msg;
    msg.setMessage(info, 0, closeSec);
    msg.exec();
  } else {
    QUIMessageBox::Instance()->setMessage(info, 0, closeSec);
    QUIMessageBox::Instance()->show();
  }
}

void myDialog::showMessageBoxError(const QString &info, int closeSec, bool exec)
{
  if (exec) {
    QUIMessageBox msg;
    msg.setMessage(info, 2, closeSec);
    msg.exec();
  } else {
    QUIMessageBox::Instance()->setMessage(info, 2, closeSec);
    QUIMessageBox::Instance()->show();
  }
}

int myDialog::showMessageBoxQuestion(const QString &info)
{
  QUIMessageBox msg;
  msg.setMessage(info, 1);
  return msg.exec();
}

void myDialog::showTipBox(const QString &title, const QString &tip, bool fullScreen, bool center, int closeSec)
{
  QUITipBox::Instance()->setTip(title, tip, fullScreen, center, closeSec);
  QUITipBox::Instance()->show();
}

void myDialog::hideTipBox()
{
  QUITipBox::Instance()->hide();
}

QString myDialog::showInputBox(const QString &title, int type, int closeSec,
                               const QString &placeholderText, bool pwd,
                               const QString &defaultValue)
{
  QUIInputBox input;
  input.setParameter(title, type, closeSec, placeholderText, pwd, defaultValue);
  input.exec();
  return input.getValue();
}

void myDialog::showDateSelect(QString &dateStart, QString &dateEnd, const QString &format)
{
  QUIDateSelect select;
  select.setFormat(format);
  select.exec();
  dateStart = select.getStartDateTime();
  dateEnd = select.getEndDateTime();
}

QStringList myDialog::showQFileDialog()
{
  //定义文件对话框类
  QFileDialog fileDialog;
  //定义文件对话框标题
  fileDialog.setWindowTitle(tr("打开文件"));
  //设置默认文件路径
  fileDialog.setDirectory(".");
  //设置文件过滤器
  //fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
  //fileDialog->setNameFilter(tr("text(*.txt)"));
  fileDialog.setNameFilter(tr("All files (*.*);;Images (*.jpg *.jpeg *.tif *.bmp *.png);;Binary File(*.bin *.txt *.hex;;"));
  //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFile
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  //设置视图模式
  fileDialog.setViewMode(QFileDialog::Detail);
  //打印所有选择的文件的路径
  QStringList fileNames;
  if (fileDialog.exec())
  {
    fileNames = fileDialog.selectedFiles();
  }
  for (auto tmp : fileNames)
  {
    qDebug() << tmp << endl;
  }
  return fileNames;
}

QFont myDialog::showQFontDialog()
{
  QFontDialog dlg;

  dlg.setWindowTitle("Font Dialog Test");
  dlg.setCurrentFont(QFont("Courier New", 10, QFont::Bold));

  if( dlg.exec() == QFontDialog::Accepted )
  {
    qDebug() << dlg.selectedFont();
  }
  return dlg.selectedFont();
}

QColor myDialog::showQColorDialog()
{
  QColor color = QColorDialog::getColor(Qt::black);
  if (color.isValid())
  {
    qDebug() << color.redF() << color.greenF() << color.blueF();
    return color;
  }
  else if (!color.isValid()){
    return color;
  }
}


void myDialog::setIcon(QLabel *lab, const QChar &str, quint32 size)
{
  iconFont.setPixelSize(size);
  lab->setFont(iconFont);
  lab->setText(str);
}

void myDialog::setIcon(QAbstractButton *btn, const QChar &str, quint32 size)
{
  iconFont.setPixelSize(size);
  btn->setFont(iconFont);
  btn->setText(str);
}

void myDialog::setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle, QLabel *labIco, QPushButton *btnClose, bool tool)
{
        labIco->setFixedWidth(TitleMinSize);
        btnClose->setFixedWidth(TitleMinSize);
        widgetTitle->setFixedHeight(TitleMinSize);
        widgetTitle->setProperty("form", "title");
        widgetMain->setProperty("form", true);
        widgetMain->setProperty("canMove", true);

    #ifdef __arm__
        if (tool) {
            widgetMain->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        } else {
            widgetMain->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        }
    #else
        if (tool) {
            widgetMain->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        } else {
            widgetMain->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        }
    #endif

        myDialog::Instance()->setIcon(labIco, QUIConfig::IconMain, QUIConfig::FontSize + 2);
        myDialog::Instance()->setIcon(btnClose, QUIConfig::IconClose, QUIConfig::FontSize);
}


QScopedPointer<QUIMessageBox> QUIMessageBox::self;
QUIMessageBox *QUIMessageBox::Instance()
{
  if (self.isNull()) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new QUIMessageBox);
    }
  }

  return self.data();
}

QUIMessageBox::QUIMessageBox(QWidget *parent) : QDialog(parent)
{
  this->initControl();
  this->initForm();
  MyHelper::setFormInCenter(this);
}

QUIMessageBox::~QUIMessageBox()
{
  delete widgetMain;
}

void QUIMessageBox::closeEvent(QCloseEvent *)
{
  closeSec = 0;
  currentSec = 0;
}

bool QUIMessageBox::eventFilter(QObject *watched, QEvent *event)
{
  static QPoint mousePoint;
  static bool mousePressed = false;

  QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
  if (mouseEvent->type() == QEvent::MouseButtonPress) {
    if (mouseEvent->button() == Qt::LeftButton) {
      mousePressed = true;
      mousePoint = mouseEvent->globalPos() - this->pos();
      return true;
    }
  } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
    mousePressed = false;
    return true;
  } else if (mouseEvent->type() == QEvent::MouseMove) {
    if (mousePressed) {
      this->move(mouseEvent->globalPos() - mousePoint);
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}

void QUIMessageBox::initControl()
{
  this->setObjectName(QString::fromUtf8("QUIMessageBox"));

  verticalLayout1 = new QVBoxLayout(this);
  verticalLayout1->setSpacing(0);
  verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
  verticalLayout1->setContentsMargins(1, 1, 1, 1);
  widgetTitle = new QWidget(this);
  widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
  widgetTitle->setSizePolicy(sizePolicy);
  horizontalLayout3 = new QHBoxLayout(widgetTitle);
  horizontalLayout3->setSpacing(0);
  horizontalLayout3->setObjectName(QString::fromUtf8("horizontalLayout3"));
  horizontalLayout3->setContentsMargins(0, 0, 0, 0);
  labIco = new QLabel(widgetTitle);
  labIco->setObjectName(QString::fromUtf8("labIco"));
  QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
  labIco->setSizePolicy(sizePolicy1);
  labIco->setAlignment(Qt::AlignCenter);

  horizontalLayout3->addWidget(labIco);

  labTitle = new QLabel(widgetTitle);
  labTitle->setObjectName(QString::fromUtf8("labTitle"));
  labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

  horizontalLayout3->addWidget(labTitle);

  labTime = new QLabel(widgetTitle);
  labTime->setObjectName(QString::fromUtf8("labTime"));
  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(labTime->sizePolicy().hasHeightForWidth());
  labTime->setSizePolicy(sizePolicy2);
  labTime->setAlignment(Qt::AlignCenter);

  horizontalLayout3->addWidget(labTime);

  widgetMenu = new QWidget(widgetTitle);
  widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
  sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
  widgetMenu->setSizePolicy(sizePolicy1);
  horizontalLayout4 = new QHBoxLayout(widgetMenu);
  horizontalLayout4->setSpacing(0);
  horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
  horizontalLayout4->setContentsMargins(0, 0, 0, 0);
  btnMenu_Close = new QPushButton(widgetMenu);
  btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
  QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
  btnMenu_Close->setSizePolicy(sizePolicy3);
  btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
  btnMenu_Close->setFocusPolicy(Qt::NoFocus);
  btnMenu_Close->setFlat(true);

  horizontalLayout4->addWidget(btnMenu_Close);
  horizontalLayout3->addWidget(widgetMenu);
  verticalLayout1->addWidget(widgetTitle);

  widgetMain = new QWidget(this);
  widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
  verticalLayout2 = new QVBoxLayout(widgetMain);
  verticalLayout2->setSpacing(5);
  verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
  verticalLayout2->setContentsMargins(5, 5, 5, 5);
  frame = new QFrame(widgetMain);
  frame->setObjectName(QString::fromUtf8("frame"));
  frame->setFrameShape(QFrame::Box);
  frame->setFrameShadow(QFrame::Sunken);
  verticalLayout4 = new QVBoxLayout(frame);
  verticalLayout4->setObjectName(QString::fromUtf8("verticalLayout4"));
  verticalLayout4->setContentsMargins(-1, 9, -1, -1);
  horizontalLayout1 = new QHBoxLayout();
  horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
  labIcoMain = new QLabel(frame);
  labIcoMain->setObjectName(QString::fromUtf8("labIcoMain"));
  labIcoMain->setAlignment(Qt::AlignCenter);
  horizontalLayout1->addWidget(labIcoMain);
  horizontalSpacer1 = new QSpacerItem(5, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
  horizontalLayout1->addItem(horizontalSpacer1);

  labInfo = new QLabel(frame);
  labInfo->setObjectName(QString::fromUtf8("labInfo"));
  QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy4.setHorizontalStretch(0);
  sizePolicy4.setVerticalStretch(0);
  sizePolicy4.setHeightForWidth(labInfo->sizePolicy().hasHeightForWidth());
  labInfo->setSizePolicy(sizePolicy4);
  labInfo->setMinimumSize(QSize(0, TitleMinSize));
  labInfo->setScaledContents(false);
  labInfo->setWordWrap(true);
  horizontalLayout1->addWidget(labInfo);
  verticalLayout4->addLayout(horizontalLayout1);

  horizontalLayout2 = new QHBoxLayout();
  horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
  horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  horizontalLayout2->addItem(horizontalSpacer2);

  btnOk = new QPushButton(frame);
  btnOk->setObjectName(QString::fromUtf8("btnOk"));
  btnOk->setMinimumSize(QSize(85, 0));
  btnOk->setFocusPolicy(Qt::StrongFocus);
  btnOk->setIcon(QIcon(":/image/btn_ok.png"));
  horizontalLayout2->addWidget(btnOk);

  btnCancel = new QPushButton(frame);
  btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
  btnCancel->setMinimumSize(QSize(85, 0));
  btnCancel->setFocusPolicy(Qt::StrongFocus);
  btnCancel->setIcon(QIcon(":/image/btn_close.png"));
  horizontalLayout2->addWidget(btnCancel);

  verticalLayout4->addLayout(horizontalLayout2);
  verticalLayout2->addWidget(frame);
  verticalLayout1->addWidget(widgetMain);

  widgetTitle->raise();
  widgetMain->raise();
  frame->raise();

  btnOk->setText("确定");
  btnCancel->setText("取消");

  connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
  connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIMessageBox::initForm()
{
  myDialog::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
  this->setWindowTitle(this->labTitle->text());
  this->setFixedSize(DialogMinWidth, DialogMinHeight);
  labIcoMain->setFixedSize(TitleMinSize, TitleMinSize);

#ifdef __arm__
  int width = 90;
  int iconWidth = 22;
  int iconHeight = 22;
#else
  int width = 80;
  int iconWidth = 18;
  int iconHeight = 18;
#endif

  QList<QPushButton *> btns = this->frame->findChildren<QPushButton *>();
  foreach (QPushButton *btn, btns) {
    btn->setMinimumWidth(width);
    btn->setIconSize(QSize(iconWidth, iconHeight));
  }

  closeSec = 0;
  currentSec = 0;

  QTimer *timer = new QTimer(this);
  timer->setInterval(1000);
  connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
  timer->start();

  this->installEventFilter(this);
}

void QUIMessageBox::checkSec()
{
  if (closeSec == 0) {
    return;
  }

  if (currentSec < closeSec) {
    currentSec++;
  } else {
    this->close();
  }

  QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
  this->labTime->setText(str);
}

void QUIMessageBox::on_btnOk_clicked()
{
  done(QMessageBox::Yes);
  close();
}

void QUIMessageBox::on_btnMenu_Close_clicked()
{
  done(QMessageBox::No);
  close();
}

void QUIMessageBox::setIconMain(const QChar &str, quint32 size)
{
  myDialog::Instance()->setIcon(this->labIco, str, size);
}

void QUIMessageBox::setMessage(const QString &msg, int type, int closeSec)
{
  this->closeSec = closeSec;
  this->currentSec = 0;
  this->labTime->clear();
  checkSec();

  //图片存在则取图片,不存在则取图形字体
  int size = this->labIcoMain->size().height();
  bool exist = !QImage(":/image/msg_info.png").isNull();
  if (type == 0) {//信息
    if (exist) {
      this->labIcoMain->setStyleSheet("border-image: url(:/image/msg_info.png);");
    } else {
      myDialog::Instance()->setIcon(this->labIcoMain, 0xf05a, size);
    }

    this->btnCancel->setVisible(false);
    this->labTitle->setText("提示");
  } else if (type == 1) {
    if (exist) {
      this->labIcoMain->setStyleSheet("border-image: url(:/image/msg_question.png);");
    } else {
      myDialog::Instance()->setIcon(this->labIcoMain, 0xf059, size);
    }

    this->labTitle->setText("询问");
  } else if (type == 2) {
    if (exist) {
      this->labIcoMain->setStyleSheet("border-image: url(:/image/msg_error.png);");
    } else {
      myDialog::Instance()->setIcon(this->labIcoMain, 0xf057, size);
    }

    this->btnCancel->setVisible(false);
    this->labTitle->setText("错误");
  }

  this->labInfo->setText(msg);
  this->setWindowTitle(this->labTitle->text());
  //设置对话框的大小总以最合适的大小显示
  if (msg.length() < 70) {
    this->layout()->setSizeConstraint(QLayout::SetMinimumSize);
    this->setFixedSize(DialogMinWidth, DialogMinHeight);
  } else {
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
  }
}


QScopedPointer<QUITipBox> QUITipBox::self;
QUITipBox *QUITipBox::Instance()
{
  if (self.isNull()) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new QUITipBox);
    }
  }

  return self.data();
}

QUITipBox::QUITipBox(QWidget *parent) : QDialog(parent)
{
  this->initControl();
  this->initForm();
}

QUITipBox::~QUITipBox()
{
  delete widgetMain;
}

void QUITipBox::closeEvent(QCloseEvent *)
{
  closeSec = 0;
  currentSec = 0;
}

bool QUITipBox::eventFilter(QObject *watched, QEvent *event)
{
  static QPoint mousePoint;
  static bool mousePressed = false;

  QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
  if (mouseEvent->type() == QEvent::MouseButtonPress) {
    if (mouseEvent->button() == Qt::LeftButton) {
      mousePressed = true;
      mousePoint = mouseEvent->globalPos() - this->pos();
      return true;
    }
  } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
    mousePressed = false;
    return true;
  } else if (mouseEvent->type() == QEvent::MouseMove) {
    if (mousePressed) {
      this->move(mouseEvent->globalPos() - mousePoint);
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}

void QUITipBox::initControl()
{
  this->setObjectName(QString::fromUtf8("QUITipBox"));

  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setSpacing(0);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  verticalLayout->setContentsMargins(1, 1, 1, 1);
  widgetTitle = new QWidget(this);
  widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
  widgetTitle->setSizePolicy(sizePolicy);
  horizontalLayout2 = new QHBoxLayout(widgetTitle);
  horizontalLayout2->setSpacing(0);
  horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
  horizontalLayout2->setContentsMargins(0, 0, 0, 0);
  labIco = new QLabel(widgetTitle);
  labIco->setObjectName(QString::fromUtf8("labIco"));
  labIco->setAlignment(Qt::AlignCenter);
  horizontalLayout2->addWidget(labIco);

  labTitle = new QLabel(widgetTitle);
  labTitle->setObjectName(QString::fromUtf8("labTitle"));
  labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
  horizontalLayout2->addWidget(labTitle);

  labTime = new QLabel(widgetTitle);
  labTime->setObjectName(QString::fromUtf8("labTime"));
  QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(labTime->sizePolicy().hasHeightForWidth());
  labTime->setSizePolicy(sizePolicy1);
  labTime->setAlignment(Qt::AlignCenter);
  horizontalLayout2->addWidget(labTime);

  widgetMenu = new QWidget(widgetTitle);
  widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
  QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
  widgetMenu->setSizePolicy(sizePolicy2);
  horizontalLayout = new QHBoxLayout(widgetMenu);
  horizontalLayout->setSpacing(0);
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  horizontalLayout->setContentsMargins(0, 0, 0, 0);
  btnMenu_Close = new QPushButton(widgetMenu);
  btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
  QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
  btnMenu_Close->setSizePolicy(sizePolicy3);
  btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
  btnMenu_Close->setFocusPolicy(Qt::NoFocus);
  btnMenu_Close->setFlat(true);

  horizontalLayout->addWidget(btnMenu_Close);
  horizontalLayout2->addWidget(widgetMenu);
  verticalLayout->addWidget(widgetTitle);

  widgetMain = new QWidget(this);
  widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
  widgetMain->setAutoFillBackground(true);
  verticalLayout2 = new QVBoxLayout(widgetMain);
  verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
  labInfo = new QLabel(widgetMain);
  labInfo->setObjectName(QString::fromUtf8("labInfo"));
  labInfo->setScaledContents(true);
  verticalLayout2->addWidget(labInfo);
  verticalLayout->addWidget(widgetMain);

  connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUITipBox::initForm()
{
  myDialog::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
  this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
  this->setFixedSize(350, 180);
#else
  this->setFixedSize(280, 150);
#endif

  closeSec = 0;
  currentSec = 0;

  QTimer *timer = new QTimer(this);
  timer->setInterval(1000);
  connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
  timer->start();

  this->installEventFilter(this);

  //字体加大
  QFont font;
  font.setPixelSize(QUIConfig::FontSize + 3);
  font.setBold(true);
  this->labInfo->setFont(font);

  //显示和隐藏窗体动画效果
  animation = new QPropertyAnimation(this, "pos");
  animation->setDuration(500);
  animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void QUITipBox::checkSec()
{
  if (closeSec == 0) {
    return;
  }

  if (currentSec < closeSec) {
    currentSec++;
  } else {
    this->close();
  }

  QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
  this->labTime->setText(str);
}

void QUITipBox::on_btnMenu_Close_clicked()
{
  done(QMessageBox::No);
  close();
}

void QUITipBox::setIconMain(const QChar &str, quint32 size)
{
  myDialog::Instance()->setIcon(this->labIco, str, size);
}

void QUITipBox::setTip(const QString &title, const QString &tip, bool fullScreen, bool center, int closeSec)
{
  this->closeSec = closeSec;
  this->currentSec = 0;
  this->labTime->clear();
  checkSec();

  this->fullScreen = fullScreen;
  this->labTitle->setText(title);
  this->labInfo->setText(tip);
  this->labInfo->setAlignment(center ? Qt::AlignCenter : Qt::AlignLeft);
  this->setWindowTitle(this->labTitle->text());

  QRect rect = fullScreen ? qApp->desktop()->geometry() : qApp->desktop()->availableGeometry();
  int width = rect.width();
  int height = rect.height();
  int x = width - this->width();
  int y = height - this->height();

  //移到右下角
  this->move(x, y);

  //启动动画
  animation->stop();
  animation->setStartValue(QPoint(x, height));
  animation->setEndValue(QPoint(x, y));
  animation->start();
}

void QUITipBox::hide()
{
  QRect rect = fullScreen ? qApp->desktop()->availableGeometry() : qApp->desktop()->geometry();
  int width = rect.width();
  int height = rect.height();
  int x = width - this->width();
  int y = height - this->height();

  //启动动画
  animation->stop();
  animation->setStartValue(QPoint(x, y));
  animation->setEndValue(QPoint(x, qApp->desktop()->geometry().height()));
  animation->start();
}


QScopedPointer<QUIInputBox> QUIInputBox::self;
QUIInputBox *QUIInputBox::Instance()
{
  if (self.isNull()) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new QUIInputBox);
    }
  }

  return self.data();
}

QUIInputBox::QUIInputBox(QWidget *parent) : QDialog(parent)
{
  this->initControl();
  this->initForm();
  MyHelper::setFormInCenter(this);
}

QUIInputBox::~QUIInputBox()
{
  delete widgetMain;
}

void QUIInputBox::showEvent(QShowEvent *)
{
  txtValue->setFocus();
  this->activateWindow();
}

void QUIInputBox::initControl()
{
  this->setObjectName(QString::fromUtf8("QUIInputBox"));

  verticalLayout1 = new QVBoxLayout(this);
  verticalLayout1->setSpacing(0);
  verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
  verticalLayout1->setContentsMargins(1, 1, 1, 1);
  widgetTitle = new QWidget(this);
  widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
  widgetTitle->setSizePolicy(sizePolicy);
  horizontalLayout1 = new QHBoxLayout(widgetTitle);
  horizontalLayout1->setSpacing(0);
  horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
  horizontalLayout1->setContentsMargins(0, 0, 0, 0);
  labIco = new QLabel(widgetTitle);
  labIco->setObjectName(QString::fromUtf8("labIco"));
  QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
  labIco->setSizePolicy(sizePolicy1);
  labIco->setAlignment(Qt::AlignCenter);

  horizontalLayout1->addWidget(labIco);

  labTitle = new QLabel(widgetTitle);
  labTitle->setObjectName(QString::fromUtf8("labTitle"));
  labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

  horizontalLayout1->addWidget(labTitle);

  labTime = new QLabel(widgetTitle);
  labTime->setObjectName(QString::fromUtf8("labTime"));
  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(labTime->sizePolicy().hasHeightForWidth());
  labTime->setSizePolicy(sizePolicy2);
  labTime->setAlignment(Qt::AlignCenter);

  horizontalLayout1->addWidget(labTime);

  widgetMenu = new QWidget(widgetTitle);
  widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
  sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
  widgetMenu->setSizePolicy(sizePolicy1);
  horizontalLayout2 = new QHBoxLayout(widgetMenu);
  horizontalLayout2->setSpacing(0);
  horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
  horizontalLayout2->setContentsMargins(0, 0, 0, 0);
  btnMenu_Close = new QPushButton(widgetMenu);
  btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
  QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
  btnMenu_Close->setSizePolicy(sizePolicy3);
  btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
  btnMenu_Close->setFocusPolicy(Qt::NoFocus);
  btnMenu_Close->setFlat(true);

  horizontalLayout2->addWidget(btnMenu_Close);
  horizontalLayout1->addWidget(widgetMenu);
  verticalLayout1->addWidget(widgetTitle);

  widgetMain = new QWidget(this);
  widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
  verticalLayout2 = new QVBoxLayout(widgetMain);
  verticalLayout2->setSpacing(5);
  verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
  verticalLayout2->setContentsMargins(5, 5, 5, 5);
  frame = new QFrame(widgetMain);
  frame->setObjectName(QString::fromUtf8("frame"));
  frame->setFrameShape(QFrame::Box);
  frame->setFrameShadow(QFrame::Sunken);
  verticalLayout3 = new QVBoxLayout(frame);
  verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
  labInfo = new QLabel(frame);
  labInfo->setObjectName(QString::fromUtf8("labInfo"));
  labInfo->setScaledContents(false);
  labInfo->setWordWrap(true);
  verticalLayout3->addWidget(labInfo);

  txtValue = new QLineEdit(frame);
  txtValue->setObjectName(QString::fromUtf8("txtValue"));
  verticalLayout3->addWidget(txtValue);

  cboxValue = new QComboBox(frame);
  cboxValue->setObjectName(QString::fromUtf8("cboxValue"));
  verticalLayout3->addWidget(cboxValue);

  lay = new QHBoxLayout();
  lay->setObjectName(QString::fromUtf8("lay"));
  horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  lay->addItem(horizontalSpacer);

  btnOk = new QPushButton(frame);
  btnOk->setObjectName(QString::fromUtf8("btnOk"));
  btnOk->setMinimumSize(QSize(85, 0));
  btnOk->setIcon(QIcon(":/image/btn_ok.png"));
  lay->addWidget(btnOk);

  btnCancel = new QPushButton(frame);
  btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
  btnCancel->setMinimumSize(QSize(85, 0));
  btnCancel->setIcon(QIcon(":/image/btn_close.png"));
  lay->addWidget(btnCancel);

  verticalLayout3->addLayout(lay);
  verticalLayout2->addWidget(frame);
  verticalLayout1->addWidget(widgetMain);

  QWidget::setTabOrder(txtValue, btnOk);
  QWidget::setTabOrder(btnOk, btnCancel);

  labTitle->setText("输入框");
  btnOk->setText("确定");
  btnCancel->setText("取消");

  connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
  connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIInputBox::initForm()
{
  myDialog::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
  this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
  int width = 90;
  int iconWidth = 22;
  int iconHeight = 22;
  this->setFixedSize(350, 180);
#else
  int width = 80;
  int iconWidth = 18;
  int iconHeight = 18;
  this->setFixedSize(280, 150);
#endif

  QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
  foreach (QPushButton *btn, btns) {
    btn->setMinimumWidth(width);
    btn->setIconSize(QSize(iconWidth, iconHeight));
  }

  closeSec = 0;
  currentSec = 0;

  QTimer *timer = new QTimer(this);
  timer->setInterval(1000);
  connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
  timer->start();

  this->installEventFilter(this);
}

void QUIInputBox::checkSec()
{
  if (closeSec == 0) {
    return;
  }

  if (currentSec < closeSec) {
    currentSec++;
  } else {
    this->close();
  }

  QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
  this->labTime->setText(str);
}

void QUIInputBox::setParameter(const QString &title, int type, int closeSec,
                               QString placeholderText, bool pwd,
                               const QString &defaultValue)
{
  this->closeSec = closeSec;
  this->currentSec = 0;
  this->labTime->clear();
  this->labInfo->setText(title);
  checkSec();

  if (type == 0) {
    this->cboxValue->setVisible(false);
    this->txtValue->setPlaceholderText(placeholderText);
    this->txtValue->setText(defaultValue);

    if (pwd) {
      this->txtValue->setEchoMode(QLineEdit::Password);
    }
  } else if (type == 1) {
    this->txtValue->setVisible(false);
    this->cboxValue->addItems(defaultValue.split("|"));
  }
}

QString QUIInputBox::getValue() const
{
  return this->value;
}

void QUIInputBox::closeEvent(QCloseEvent *)
{
  closeSec = 0;
  currentSec = 0;
}

bool QUIInputBox::eventFilter(QObject *watched, QEvent *event)
{
  static QPoint mousePoint;
  static bool mousePressed = false;

  QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
  if (mouseEvent->type() == QEvent::MouseButtonPress) {
    if (mouseEvent->button() == Qt::LeftButton) {
      mousePressed = true;
      mousePoint = mouseEvent->globalPos() - this->pos();
      return true;
    }
  } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
    mousePressed = false;
    return true;
  } else if (mouseEvent->type() == QEvent::MouseMove) {
    if (mousePressed) {
      this->move(mouseEvent->globalPos() - mousePoint);
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}

void QUIInputBox::on_btnOk_clicked()
{
  if (this->txtValue->isVisible()) {
    value = this->txtValue->text();
  } else if (this->cboxValue->isVisible()) {
    value = this->cboxValue->currentText();
  }

  done(QMessageBox::Ok);
  close();
}

void QUIInputBox::on_btnMenu_Close_clicked()
{
  done(QMessageBox::Cancel);
  close();
}

void QUIInputBox::setIconMain(const QChar &str, quint32 size)
{
  myDialog::Instance()->setIcon(this->labIco, str, size);
}


QScopedPointer<QUIDateSelect> QUIDateSelect::self;
QUIDateSelect *QUIDateSelect::Instance()
{
  if (self.isNull()) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (self.isNull()) {
      self.reset(new QUIDateSelect);
    }
  }

  return self.data();
}

QUIDateSelect::QUIDateSelect(QWidget *parent) : QDialog(parent)
{
  this->initControl();
  this->initForm();
  MyHelper::setFormInCenter(this);
}

QUIDateSelect::~QUIDateSelect()
{
  delete widgetMain;
}

bool QUIDateSelect::eventFilter(QObject *watched, QEvent *event)
{
  static QPoint mousePoint;
  static bool mousePressed = false;

  QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
  if (mouseEvent->type() == QEvent::MouseButtonPress) {
    if (mouseEvent->button() == Qt::LeftButton) {
      mousePressed = true;
      mousePoint = mouseEvent->globalPos() - this->pos();
      return true;
    }
  } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
    mousePressed = false;
    return true;
  } else if (mouseEvent->type() == QEvent::MouseMove) {
    if (mousePressed) {
      this->move(mouseEvent->globalPos() - mousePoint);
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}

void QUIDateSelect::initControl()
{
  this->setObjectName(QString::fromUtf8("QUIDateSelect"));

  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setSpacing(0);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  verticalLayout->setContentsMargins(1, 1, 1, 1);
  widgetTitle = new QWidget(this);
  widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
  widgetTitle->setSizePolicy(sizePolicy);
  horizontalLayout1 = new QHBoxLayout(widgetTitle);
  horizontalLayout1->setSpacing(0);
  horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
  horizontalLayout1->setContentsMargins(0, 0, 0, 0);
  labIco = new QLabel(widgetTitle);
  labIco->setObjectName(QString::fromUtf8("labIco"));
  QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
  labIco->setSizePolicy(sizePolicy1);
  labIco->setAlignment(Qt::AlignCenter);
  horizontalLayout1->addWidget(labIco);

  labTitle = new QLabel(widgetTitle);
  labTitle->setObjectName(QString::fromUtf8("labTitle"));
  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(labTitle->sizePolicy().hasHeightForWidth());
  labTitle->setSizePolicy(sizePolicy2);
  labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
  horizontalLayout1->addWidget(labTitle);

  widgetMenu = new QWidget(widgetTitle);
  widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
  sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
  widgetMenu->setSizePolicy(sizePolicy1);
  horizontalLayout = new QHBoxLayout(widgetMenu);
  horizontalLayout->setSpacing(0);
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  horizontalLayout->setContentsMargins(0, 0, 0, 0);
  btnMenu_Close = new QPushButton(widgetMenu);
  btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
  QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
  btnMenu_Close->setSizePolicy(sizePolicy3);
  btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
  btnMenu_Close->setFocusPolicy(Qt::NoFocus);
  btnMenu_Close->setFlat(true);

  horizontalLayout->addWidget(btnMenu_Close);
  horizontalLayout1->addWidget(widgetMenu);
  verticalLayout->addWidget(widgetTitle);

  widgetMain = new QWidget(this);
  widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
  verticalLayout1 = new QVBoxLayout(widgetMain);
  verticalLayout1->setSpacing(6);
  verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
  verticalLayout1->setContentsMargins(6, 6, 6, 6);
  frame = new QFrame(widgetMain);
  frame->setObjectName(QString::fromUtf8("frame"));
  frame->setFrameShape(QFrame::Box);
  frame->setFrameShadow(QFrame::Sunken);
  gridLayout = new QGridLayout(frame);
  gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
  labStart = new QLabel(frame);
  labStart->setObjectName(QString::fromUtf8("labStart"));
  labStart->setFocusPolicy(Qt::TabFocus);
  gridLayout->addWidget(labStart, 0, 0, 1, 1);

  btnOk = new QPushButton(frame);
  btnOk->setObjectName(QString::fromUtf8("btnOk"));
  btnOk->setMinimumSize(QSize(85, 0));
  btnOk->setCursor(QCursor(Qt::PointingHandCursor));
  btnOk->setFocusPolicy(Qt::StrongFocus);
  btnOk->setIcon(QIcon(":/image/btn_ok.png"));
  gridLayout->addWidget(btnOk, 0, 2, 1, 1);

  labEnd = new QLabel(frame);
  labEnd->setObjectName(QString::fromUtf8("labEnd"));
  labEnd->setFocusPolicy(Qt::TabFocus);
  gridLayout->addWidget(labEnd, 1, 0, 1, 1);

  btnClose = new QPushButton(frame);
  btnClose->setObjectName(QString::fromUtf8("btnClose"));
  btnClose->setMinimumSize(QSize(85, 0));
  btnClose->setCursor(QCursor(Qt::PointingHandCursor));
  btnClose->setFocusPolicy(Qt::StrongFocus);
  btnClose->setIcon(QIcon(":/image/btn_close.png"));
  gridLayout->addWidget(btnClose, 1, 2, 1, 1);

  dateStart = new QDateTimeEdit(frame);
  dateStart->setObjectName(QString::fromUtf8("dateStart"));
  QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizePolicy4.setHorizontalStretch(0);
  sizePolicy4.setVerticalStretch(0);
  sizePolicy4.setHeightForWidth(dateStart->sizePolicy().hasHeightForWidth());
  dateStart->setSizePolicy(sizePolicy4);
  dateStart->setCalendarPopup(true);
  gridLayout->addWidget(dateStart, 0, 1, 1, 1);

  dateEnd = new QDateTimeEdit(frame);
  dateEnd->setObjectName(QString::fromUtf8("dateEnd"));
  sizePolicy4.setHeightForWidth(dateEnd->sizePolicy().hasHeightForWidth());
  dateEnd->setSizePolicy(sizePolicy4);
  dateEnd->setCalendarPopup(true);

  gridLayout->addWidget(dateEnd, 1, 1, 1, 1);
  verticalLayout1->addWidget(frame);
  verticalLayout->addWidget(widgetMain);

  QWidget::setTabOrder(labStart, labEnd);
  QWidget::setTabOrder(labEnd, dateStart);
  QWidget::setTabOrder(dateStart, dateEnd);
  QWidget::setTabOrder(dateEnd, btnOk);
  QWidget::setTabOrder(btnOk, btnClose);

  labTitle->setText("日期时间选择");
  labStart->setText("开始时间");
  labEnd->setText("结束时间");
  btnOk->setText("确定");
  btnClose->setText("关闭");

  dateStart->setDate(QDate::currentDate());
  dateEnd->setDate(QDate::currentDate().addDays(1));

  dateStart->calendarWidget()->setGridVisible(true);
  dateEnd->calendarWidget()->setGridVisible(true);
  dateStart->calendarWidget()->setLocale(QLocale::Chinese);
  dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
  setFormat("yyyy-MM-dd");

  connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
  connect(btnClose, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
  connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIDateSelect::initForm()
{
  myDialog::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
  this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
  int width = 90;
  int iconWidth = 22;
  int iconHeight = 22;
  this->setFixedSize(370, 160);
#else
  int width = 80;
  int iconWidth = 18;
  int iconHeight = 18;
  this->setFixedSize(320, 130);
#endif

  QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
  foreach (QPushButton *btn, btns) {
    btn->setMinimumWidth(width);
    btn->setIconSize(QSize(iconWidth, iconHeight));
  }

  this->installEventFilter(this);
}

void QUIDateSelect::on_btnOk_clicked()
{
  if (dateStart->dateTime() > dateEnd->dateTime()) {
    myDialog::showMessageBoxError("开始时间不能大于结束时间!", 3);
    return;
  }

  startDateTime = dateStart->dateTime().toString(format);
  endDateTime = dateEnd->dateTime().toString(format);

  done(QMessageBox::Ok);
  close();
}

void QUIDateSelect::on_btnMenu_Close_clicked()
{
  done(QMessageBox::Cancel);
  close();
}

QString QUIDateSelect::getStartDateTime() const
{
  return this->startDateTime;
}

QString QUIDateSelect::getEndDateTime() const
{
  return this->endDateTime;
}

void QUIDateSelect::setIconMain(const QChar &str, quint32 size)
{
  myDialog::Instance()->setIcon(this->labIco, str, size);
}

void QUIDateSelect::setFormat(const QString &format)
{
  this->format = format;
  this->dateStart->setDisplayFormat(format);
  this->dateEnd->setDisplayFormat(format);
}
