#ifndef WORKER_H
#define WORKER_H

#include "general.h"
#include "thread_pool.h"

#include <QThread>

#include <QListWidgetItem>
#include <QTableWidgetItem>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QEventLoop>
#include <QTimer>


typedef QListWidgetItem list_w_item;
typedef QTableWidgetItem table_w_item;

typedef QNetworkAccessManager net_manager;
typedef QNetworkRequest net_request;
typedef QNetworkReply net_reply;

typedef QEventLoop event_loop;
typedef QTimer timer;
typedef QUrl url;

typedef QRegularExpression regex;
typedef QRegularExpressionMatchIterator regex_match_iter;
typedef QRegularExpressionMatch regex_match;

static const regex link_regex("https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}[-a-zA-Z0-9@:%_\+.~#?&//=]*",regex::OptimizeOnFirstUsageOption);
static const int time_to_wait = 5000;

class thread_pool;

class worker : public QThread
{
    Q_OBJECT

    thread_pool* manager;

public:
    worker(thread_pool *man);

    ~worker();

    void run();

signals:
    void reserve_last_row(int* row);
    void set_table_item(int row, int cell, QString text);
    void list_pop_front_into(QString* into);
    void list_push_back(QString text);
    void upate_progress(int progress);
    void finished();
};

#endif // WORKER_H
