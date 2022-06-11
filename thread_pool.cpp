#ifndef THREAD_POOL_CPP
#define THREAD_POOL_CPP

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <class E>
class SafeUnboundedQueue {
    std::queue<E> elements;
    std::mutex lock;
    std::condition_variable not_empty;

   public:
    SafeUnboundedQueue<E>() {}
    void push(const E& element);
    E pop();
    bool is_empty() const { return this->elements.empty(); }
};

template <class E>
void SafeUnboundedQueue<E>::push(const E& element) {
    std::unique_lock lk(lock);
    elements.push(element);

    if (elements.size() == 1)
        not_empty.notify_all();
}

template <class E>
E SafeUnboundedQueue<E>::pop() {
    std::unique_lock lk(lock);
    not_empty.wait(lk, [&] { return !is_empty(); });  // equiv to the while loop https://en.cppreference.com/w/cpp/thread/condition_variable/wait

    E e = elements.front();
    elements.pop();
    return e;
}

//-----------------------------------------------------------------------------

class ThreadPool {
    unsigned int num_workers;
    std::vector<std::thread> workers;
    SafeUnboundedQueue<std::function<bool()>> tasks;
    void do_work();  // create workers: workers[i] = std::thread(&do_work)
   public:
    ThreadPool(unsigned int num_workers = 0);
    ~ThreadPool();
    template <class F, class... Args>
    void push(F f, Args... args);
    void stop();
};

void ThreadPool::do_work() {
    while (tasks.pop()());
}

ThreadPool::ThreadPool(unsigned int num_workers) {
    this->num_workers = num_workers;
    workers = std::vector<std::thread>(num_workers);

    for (unsigned int i = 0; i < num_workers; i++) {
        workers[i] = std::thread(&ThreadPool::do_work, this);
    }
}

ThreadPool::~ThreadPool() {
    for (unsigned int i = 0; i < num_workers; i++) {
        if (workers[i].joinable())
            workers[i].join();
    }
}

template <class F, class... Args>
void ThreadPool::push(F f, Args... args) {
    tasks.push([=]() -> bool {f(args...); return true; });
}

void ThreadPool::stop() {
    for (unsigned int i = 0; i < num_workers; i++)
        tasks.push([]() -> bool { return false; });

    for (unsigned int i = 0; i < num_workers; i++) {
        workers[i].join();
    }
}

#endif
