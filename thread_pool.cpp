#include "thread_pool.h"


worker* thread_pool::create_worker_connected_to_pool(thread_pool *pool){
    worker* w = new worker(pool);

    connect(w,SIGNAL(reserve_last_row(int*)),pool,SLOT(reserve_last_row(int*)),Qt::BlockingQueuedConnection);
    connect(w,SIGNAL(set_table_item(int,int,QString)),pool,SLOT(set_table_item(int,int,QString)),Qt::QueuedConnection);

    connect(w,SIGNAL(list_pop_front_into(QString*)),pool,SLOT(list_pop_front_into(QString*)),Qt::BlockingQueuedConnection);
    connect(w,SIGNAL(list_push_back(QString)),pool,SLOT(list_push_back(QString)),Qt::QueuedConnection);

    connect(w,SIGNAL(upate_progress(int)),pool,SLOT(update_progress(int)),Qt::QueuedConnection);

    connect(w,SIGNAL(finished()),pool,SLOT(worker_finished()),Qt::QueuedConnection);
    return w;
}

thread_pool::thread_pool():state(pstate::down)
{
}

void thread_pool::load_gui(list_w* list,table_w* table, progress_bar* progress){
    gui.list = list;
    gui.table = table;
    gui.progress = progress;
}
void thread_pool::load_shared(uint mtv,QString tts){
    shared.restart(mtv,tts);
}

void thread_pool::set_pool_size(uint num_threads)
{
    mutex_locker locker(&internal_mutex);
    if(state!=pstate::down || is_up())
        throw ModifyingOnWorkingPool();
    uint current_size = workers.size();
    if(current_size < num_threads){
        workers.reserve(num_threads);
        uint difference = num_threads - current_size;
        for(uint i = 0; i < difference;i++)
            workers.push_back(thread_pool::create_worker_connected_to_pool(this));
    }
    if(current_size > num_threads){
        uint difference = current_size - num_threads;
        for(uint i = 0; i < difference;i++){
            delete workers.back();
            workers.pop_back();
        }
    }
}


bool thread_pool::is_up(){
    for(auto thread : workers)
        if(thread->isRunning())
            return true;
    return false;
}

bool thread_pool::is_up_fast(){
    return internal_active_workers_counter != 0;
}

void thread_pool::start(QString start_url){
    mutex_locker locker(&internal_mutex);
    if(current_pool_size() == 0)
        throw EmptyThreadPool();
    gui.list->clear();
    gui.table->clear();
    gui.table->setRowCount(0);
    gui.progress->reset();

    shared.n_visited = 0;
    shared.visited.clear();
    shared.visited.insert(start_url);

    gui.list->addItem(start_url);

    internal_active_workers_counter = workers.size();
    active_workers = workers.size();

    state = pstate::up;
    auto end = workers.end();
    for(auto iter=workers.begin();iter!=end;iter++)
        (*iter)->start();
}

void thread_pool::pause(){
    mutex_locker locker(&internal_mutex);
    if (state == pstate::paused)
        throw MultiplePauseCall();
    else if (state == pstate::down)
        throw PauseCallOnDownPool();

    state = pstate::paused;
    sync.wc_list.wakeAll();
}

void thread_pool::unpause()
{
    mutex_locker locker(&internal_mutex);
    if (state == pstate::down)
        throw UnpauseCallOnDownPool();
    if (state == pstate::up)
        return;
    state = pstate::up;
    sync.wc.wakeAll();
}

void thread_pool::stop(){
    mutex_locker locker(&internal_mutex);
    state = pstate::down;
    sync.wc.wakeAll();
    sync.wc_list.wakeAll();
}

void thread_pool::stop_sync(){
    stop();
    for(auto thread: workers)
        thread->wait();
}

void thread_pool::worker_finished(){
    if(--internal_active_workers_counter == 0){
        gui.progress->setValue(gui.progress->maximum());
        state = pstate::down;
        emit finished();
    }
}

thread_pool::~thread_pool(){
    stop_sync();
    auto end = workers.end();
    for(auto iter=workers.begin();iter!=end;iter++)
        delete (*iter);
}

// gui related
void thread_pool::reserve_last_row(int *row){
    *row = gui.table->rowCount();
    gui.table->insertRow(*row);
}

void thread_pool::set_table_item(int row, int cell, QString text){
    gui.table->setItem(row,cell,new table_w_item(text));
}

void thread_pool::list_pop_front_into(QString* into){
    into->clear();
    list_w_item* item = gui.list->takeItem(0);
    if (item != nullptr){
        into->append(item->text());
        delete item;
    }
}

void thread_pool::list_push_back(QString text){
    gui.list->insertItem(gui.list->count(),text);
}

void thread_pool::update_progress(int progress){
    if(progress>gui.progress->value())
        gui.progress->setValue(progress);
}
