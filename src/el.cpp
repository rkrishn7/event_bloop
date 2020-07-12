#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <fstream>

#include "el.h"
#include "utils.h"

using namespace std;

EventLoop::~EventLoop() {}

void EventLoop::set_max_threads(unsigned int mt) {
    this->max_threads = mt;
}

void* EventLoop::timer_wrapper(void* t) {
    timer* ref = static_cast<timer*>(t);
    ref->cb->executing = true;
    sleep(ref->seconds);
    ref->cb->fn(ref->cb->a);
    ref->cb->executing = false;
    ref->cb->completed = true;
    pthread_exit(NULL);
}

void EventLoop::start() {
    while(true) {
        if(this->registered_timers.size() == 0
        && this->cb_queue.size() == 0) {
            Utils::log("Finished");
            break;
        }
        else {
            // Check to see if there are any timers that need to be cleaned up
            this->cleanup_timers();
            this->launch_timers();
            this->cleanup_tasks();
            this->handle_tasks();
        }
    }
}

void EventLoop::cleanup_timers() {
    vector<vector<timer*>::iterator> to_erase;
    for(auto it = this->registered_timers.begin(); it != this->registered_timers.end(); ++it) {
        if(*it != nullptr && (*it)->cb->completed == true) {
            delete (*it)->cb;
            delete *it;
            to_erase.push_back(it);
            --this->active_thread_count;
        }
    }
    for(auto it : to_erase) this->registered_timers.erase(it);
}

void EventLoop::cleanup_tasks() {
    vector<vector<callback*>::iterator> to_erase;
    for(auto it = this->cb_queue.begin(); it != this->cb_queue.end(); ++it) {
        if(*it != nullptr && (*it)->completed == true) {
            delete *it;
            to_erase.push_back(it);
            --this->active_thread_count;
        }
    }
    for(auto it : to_erase) this->cb_queue.erase(it);
}

void EventLoop::launch_timers() {
    for(unsigned int i = 0; i < this->registered_timers.size(); ++i) {
        if(this->active_thread_count >= this->max_threads) return;
        if(this->registered_timers[i]->cb->executing == false && this->registered_timers[i]->cb->completed == false) {
            pthread_t pt;
            int rc = pthread_create(&pt, NULL, this->timer_wrapper, (void*)this->registered_timers[i]);

            if(rc) {
                Utils::err(rc, "Unable to create thread");
            } else {
                Utils::log("Launched new thread");
                ++this->active_thread_count;
            }
        }
    }
}

void EventLoop::set_timer(void* (*fn)(void*), void* a, unsigned int s) {
    callback* cb = new callback(fn, a, TIMER);
    timer* obj = new timer(cb, s);
    this->registered_timers.push_back(obj);
}

void EventLoop::handle_tasks() {
    for(unsigned int i = 0; i < this->cb_queue.size(); ++i) {
        if(this->active_thread_count >= this->max_threads) return;
        if(this->cb_queue[i]->executing == false && this->cb_queue[i]->completed == false) {
            if(this->cb_queue[i]->type == FILE_READ) {
                pthread_t pt;
                int rc = pthread_create(&pt, NULL, this->fs_wrapper, (void*)this->cb_queue[i]);

                if(rc) {
                    Utils::err(rc, "Unable to create thread");
                } else {
                    Utils::log("Launched new thread");
                    this->cb_queue[i]->executing = true;
                    ++this->active_thread_count;
                }
            }
        }
    }
}

void* EventLoop::fs_wrapper(void* cb) {
    callback* ref = static_cast<callback*>(cb);
    ifstream ifs(static_cast<const char*>(ref->a));

    string line;

    if(ifs.is_open()) {
        while(getline(ifs, line)) {
            ref->fn((void*)(line.c_str()));
        }
        ifs.close();
    }
    else {
        Utils::err("Error reading file");
    }

    ref->completed = true;
    ref->executing = false;
    pthread_exit(NULL);
}

void EventLoop::stream_file_async(string& filename, void* (*stream_handler)(void*)) {
    callback* cb = new callback(stream_handler, (void*)filename.c_str(), FILE_READ);
    this->cb_queue.push_back(cb);
}