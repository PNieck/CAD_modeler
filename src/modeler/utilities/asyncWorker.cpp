#include <CAD_modeler/utilities/asyncWorker.hpp>


void AsyncWorker::StartWork()
{
    threadStarted = true;
    threadRunning = true;

    thread = std::jthread(
        [this] (std::stop_token stop_token) {
            callback(stop_token);

            threadRunning = false;
        }
    );
}


void AsyncWorker::JoinWorker()
{
    RequestStop();
    thread.join();
    threadStarted = false;
}
