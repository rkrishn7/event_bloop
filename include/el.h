#ifndef EL_H
#define EL_H

#include <pthread.h>
#include <vector>

#define DEFAULT_MAX_THREADS 4

enum ASYNC_OP_TYPE {
    TIMER,
    FILE_READ,
    FILE_WRITE
};

struct callback {
    void* (*fn)(void*);
    void* a;
    ASYNC_OP_TYPE type;
    bool completed;
    bool executing;

    callback(void* (*fn)(void*), void* a, ASYNC_OP_TYPE type): fn(fn), a(a), type(type), completed(false), executing(false) {};
};

struct timer {
    callback* cb;
    unsigned int seconds;

    timer(callback* cb, unsigned int s): cb(cb), seconds(s) {};
};

class EventLoop {
    private:
        std::vector<timer*> registered_timers;
        std::vector<callback*> cb_queue;
        unsigned int active_thread_count;
        unsigned int max_threads;
        void launch_timers();
        void cleanup_timers();
        void handle_tasks();
        void cleanup_tasks();
        static void* timer_wrapper(void* cb);
        static void* fs_wrapper(void* cb);
    public:
        void set_timer(void* (*fn)(void*), void* a, unsigned int s);
        void stream_file_async(std::string& filename, void* (*stream_handler)(void*));
        void set_max_threads(unsigned int mt);
        void start();
        EventLoop(): active_thread_count(0), max_threads(DEFAULT_MAX_THREADS) {}
        ~EventLoop();
        EventLoop(const EventLoop&);
        EventLoop& operator=(const EventLoop&);
        EventLoop(EventLoop&&);
        EventLoop& operator=(EventLoop&&);
};

#endif