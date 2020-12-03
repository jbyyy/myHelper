#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <myhelper.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyHelper::setFormInCenter(this);

    button = new QPushButton("QSS_Button",this);

    FlatUI::setPushButtonQss(button,10);
    button->move(this->width()/2-button->width()/2,this->height()/2-button->height()/2);
}

MainWindow::~MainWindow()
{
    delete ui;
}
