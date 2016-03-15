#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    int_validator(1,65535),paused(false)
{
    ui->setupUi(this);
    // the buggy validator accepts 0 so w/e, not my fault, if c==0 return; inc in button slot
    ui->edit_max_threads->setValidator(&int_validator);
    ui->edit_max_urls->setValidator(&int_validator);

    pool = new thread_pool();

    pool->load_gui(ui->list_pending,ui->table_processing,ui->progress);
}

MainWindow::~MainWindow(){
    delete ui;
    delete pool;
}

void MainWindow::on_button_start_clicked(){
    try{
        pool->set_pool_size(ui->edit_max_threads->text().toUInt());
        pool->load_shared(ui->edit_max_urls->text().toUInt(),ui->edit_text_to_search->toPlainText());
        pool->start(ui->edit_url->text());
    }
    catch(QException &ex){
        QMessageBox msg;
        msg.setText("You clicked start too many times, but it's ok, I won't punish for it, this time...");
        msg.exec();
    }
}

void MainWindow::on_button_pause_clicked(){
    try{
        if(paused){
            pool->unpause();
            ui->button_pause->setText("Pause");
        }
        else{
            pool->pause();
            ui->button_pause->setText("Unpause");
        }
        paused =!paused;
    }
    catch(QException &ex){
        QMessageBox msg;
        msg.setText("Do not click this before pool starts");
        msg.exec();
    }
}

void MainWindow::on_button_stop_clicked()
{
    pool->stop();
}
