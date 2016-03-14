#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "thread_pool.h"

#include <exception>

#include <QMainWindow>
#include <QIntValidator>

#include <QMessageBox>
#include <QDebug>

struct current_setup{
    QString* url;
    uint max_threads;
    QString* text_to_search;
    uint max_urls;
    current_setup(QString* u,uint m_t,QString* tx,uint m_u):url(u),max_threads(m_t),text_to_search(tx),max_urls(m_u){}
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_start_clicked();
    void on_button_pause_clicked();
    void on_button_stop_clicked();

private:
    Ui::MainWindow *ui;
    QIntValidator int_validator;
    thread_pool* pool;
    bool started;
    bool paused;

};

#endif // MAINWINDOW_H
