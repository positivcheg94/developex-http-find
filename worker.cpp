#include "worker.h"

worker::worker(thread_pool *man) : manager(man)
{
}

worker::~worker()
{
}

void worker::run()
{
    a_uint *active_workers = &manager->active_workers;

    event_loop eloop;
    net_manager network;
    timer t;
    t.setSingleShot(true);
    t.setTimerType(Qt::PreciseTimer);
    t.setInterval(time_to_wait);

    connect(&t, SIGNAL(timeout()), &eloop, SLOT(quit()));

    set_s *visited = &manager->shared.visited;
    uint *n_visited = &manager->shared.n_visited;
    uint max_to_visit = manager->shared.max_to_visit;
    QString *text_to_search = &manager->shared.text_to_search;

    pstate *state = &manager->state;

    mutex *m_list = &manager->sync.m_list;
    wait_condition &wc_list = manager->sync.wc_list;
    mutex *m = &manager->sync.m;
    wait_condition &wc = manager->sync.wc;

    QString url_to_scan;
    int current_row;

    forever {
        if (*state == pstate::paused) {
            m->lock();
            wc.wait(m);
            m->unlock();
            continue;
        } else if (*state == pstate::down) {
            break;
        } else {
            emit list_pop_front_into(&url_to_scan);
            if (!url_to_scan.isNull()) {
                emit reserve_last_row(&current_row);

                emit set_table_item(current_row, 0, url_to_scan);
                emit set_table_item(current_row, 1, request_status::downloading);

                net_reply *reply = network.get(net_request(url(url_to_scan)));
                connect(reply, SIGNAL(finished()), &eloop, SLOT(quit()));
                t.start();
                eloop.exec();
                if (t.isActive()) {
                    t.stop();
                    if (reply->error() == QNetworkReply::NoError) {
                        QString html_page(reply->readAll());

                        if (html_page.contains(*text_to_search))
                            emit set_table_item(current_row, 1, request_status::found);
                        else
                            emit set_table_item(current_row, 1, request_status::not_found);
                        regex_match_iter matches = link_regex.globalMatch(html_page);

                        while (matches.hasNext() && visited->size() < max_to_visit) {
                            QString next_url = matches.next().captured();
                            if (!visited->contains(next_url)) {
                                emit list_push_back(next_url);
                                visited->insert(next_url);
                            }
                        }
                        wc_list.wakeAll();
                    } else {
                        emit set_table_item(current_row, 1,
                                            request_status::error+reply->errorString());
                    }
                } else {
                    emit set_table_item(current_row, 1, request_status::timeout_reached);
                }
                int new_progress = ++(*n_visited)*100/max_to_visit;
                emit upate_progress(new_progress);
                delete reply;
            } else {
                if (--(*active_workers) == 0) {
                    wc_list.wakeAll();
                    break;
                } else {
                    m_list->lock();
                    wc_list.wait(m_list);
                    m_list->unlock();
                }
                if (active_workers->load() > 0)
                    ++(*active_workers);
                else
                    break;
            }
        }
    }
    emit finished();
}
