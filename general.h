#ifndef GENERAL_H
#define GENERAL_H

#include <QSet>

#include <QMutex>
#include <QWaitCondition>

#include <QListWidget>
#include <QTableWidget>
#include <QProgressBar>

typedef QSet<QString> set_s;

typedef QListWidget list_w;
typedef QTableWidget table_w;
typedef QProgressBar progress_bar;

typedef QMutex mutex;
typedef QMutexLocker mutex_locker;
typedef QWaitCondition wait_condition;

enum pstate {
    down, up, paused, finished
};

struct request_status{
    static QString downloading;
    static QString found;
    static QString not_found;
    static QString timeout_reached;
    static QString error;
};

struct sync_v {
    mutex m_list;
    wait_condition wc_list;
    mutex m;
    wait_condition wc;
};

struct shared_gui {
    list_w *list;
    table_w *table;
    progress_bar* progress;
};

struct shared_c{
    set_s visited;
    uint n_visited;
    uint max_to_visit;
    QString text_to_search;
    void restart(uint mtv,QString tts){
        visited.clear();
        n_visited = 0;
        max_to_visit = mtv;
        text_to_search = tts;
    }
};

#endif // GENERAL_H
