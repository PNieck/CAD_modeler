#pragma once

#include <thread>
#include <atomic>
#include <functional>


class AsyncWorker {
public:
    explicit AsyncWorker(const std::function<void(std::stop_token stop_token)>& callback):
        callback(callback) {}

    void StartWork();

    void JoinWorker();

    void RequestStop()
        { thread.request_stop(); }

    bool IsWorking() const
        { return threadStarted; }

    bool WaitsForJoin() const
        { return !threadRunning; }

private:
    std::function<void(std::stop_token stop_token)> callback;
    std::jthread thread;
    bool threadStarted = false;
    std::atomic_bool threadRunning = false;
};
