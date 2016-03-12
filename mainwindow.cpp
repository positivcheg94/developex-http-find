#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    int_validator(1,65535)
{
    ui->setupUi(this);
    ui->edit_max_threads->setValidator(&int_validator);
    ui->edit_max_urls->setValidator(&int_validator);
}

MainWindow::~MainWindow()
{
    delete ui;
}
