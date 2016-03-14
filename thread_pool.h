#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "general.h"
#include "worker.h"

#include <thread>
#include <vector>
#include <atomic>

#include <QException>

typedef std::atomic<uint> a_uint;

class EmptyThreadPool : public QException
{
};
class MultiplePauseCall : public QException
{
};
class PauseCallOnDownPool : public QException
{
};
class UnpauseCallOnDownPool : public QException
{
};
class ModifyingOnWorkingPool : public QException
{
};

class worker;

class thread_pool : public QObject
{
    Q_OBJECT
    friend class worker;

    static worker* create_worker_connected_to_pool(thread_pool* pool);

    sync_v sync;
    pstate state;
    shared_gui gui;
    shared_c shared;

    mutex internal_mutex;

    uint internal_active_workers_counter;
    std::vector<worker*> workers;
    a_uint active_workers;

public:
    thread_pool();

    void load_gui(list_w *list, table_w *table, progress_bar *progress);
    void load_shared(uint mtv, QString tts);


    void set_pool_size(uint num_threads);
    bool is_up();
    bool is_up_fast();

    ~thread_pool();

private:
    inline uint current_pool_size(){ return workers.size(); }
    worker* create_worker();
    void stop_sync();

signals:
    void finished();

public slots:

    void start(QString start_url);
    void pause();
    void unpause();
    void stop();

private slots:
    void worker_finished();

    void reserve_last_row(int *row);
    void set_table_item(int row, int cell, QString text);
    void list_pop_front_into(QString *into);
    void list_push_back(QString text);
    void update_progress(int progress);
};

#endif // THREAD_POOL_H
