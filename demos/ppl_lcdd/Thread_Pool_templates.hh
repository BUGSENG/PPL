#ifndef ppl_lcdd_Thread_Pool_templates_hh
#define ppl_lcdd_Thread_Pool_templates_hh 1

template <typename T>
bool
Threadsafe_Queue<T>::empty () const
{
    std::lock_guard<std::mutex> lock(mut);

    return data_queue.empty();
}

template <typename T>
void Threadsafe_Queue<T>::push (T new_value)
{
    std::lock_guard<std::mutex> lock(mut);

    data_queue.push(std::move(new_value));
}

template <typename T>
bool
Threadsafe_Queue<T>::try_pop (T & value)
{
    std::lock_guard<std::mutex> lock(mut);

    if (data_queue.empty())
    {
        return false;
    }

    value = data_queue.front();
    data_queue.pop();
    return true;
}

template <typename Work>
Thread_Pool<Work>::Thread_Pool (const unsigned num_tasks)
    : done(false)
{
    unsigned thread_count = std::thread::hardware_concurrency();

    if ((num_tasks > 0) && (num_tasks < thread_count))
    {
        thread_count = num_tasks;
    }

    try
    {
        for (unsigned i = 0; i < thread_count; ++i)
        {
            std::thread worker(&Thread_Pool::worker_thread, this);

            threads.push_back(std::move(worker));
        }
    }
    catch (...)
    {
        done = true;
        throw;
    }
}

template <typename Work>
void
Thread_Pool<Work>::worker_thread ()
{
    while (!done || !work_queue.empty())
    {
        work_type work;

        if (work_queue.try_pop(work))
        {
            work();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

template <typename Work>
Thread_Pool<Work>::~Thread_Pool ()
{
    done = true;

    // Join all threads.
    for (auto & t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

template <typename Work>
void
Thread_Pool<Work>::submit (work_type work)
{
    work_queue.push(work);
}

template <typename Work>
void
Thread_Pool<Work>::finalize ()
{
    done = true;
}

#endif // !defined(ppl_lcdd_Thread_Pool_templates_hh)

