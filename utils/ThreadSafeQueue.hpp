/**
 * @class   ThreadSafeQueue
 * @brief   A thread-safe queue for synchronized inter-thread communication.
 *
 * This template class encapsulates a standard queue along with synchronization primitives
 * to ensure thread-safe operations. It supports pushing elements to the queue, popping
 * elements from it, and marking the queue as finished for operations.
 *
 * @tparam T The type of elements stored in the queue.
 */


#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

template <typename T>
class ThreadSafeQueue {
private:
    std::mutex mutex;
    std::condition_variable cond_var;
    std::queue<T> queue;
    bool finished = false;

public:

    /**
     * @fn      void push(const T& value)
     * @brief   Adds an element to the end of the queue in a thread-safe manner.
     *
     * Locks the queue, adds the provided element to it, and then notifies one waiting thread
     * about the availability of new data.
     *
     * @param value The element to add to the queue.
     */
    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
        }
        
        cond_var.notify_one();
    }

    
    /**
     * @fn      bool pop(T& value)
     * @brief   Removes and returns the front element from the queue in a thread-safe manner.
     *
     * Waits for an element to become available (if the queue is currently empty) and then
     * removes and returns the front element. If the queue is marked as finished and empty,
     * it returns false to indicate no more elements are forthcoming.
     *
     * @param value Reference to the variable where the popped element will be stored.
     * @return True if an element was successfully popped; false if the queue is empty and finished.
     */
    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex);
        
        cond_var.wait(lock, [this]{ return !queue.empty() || finished; });
        
        if ( queue.empty() ) {
            return false;
        }
        
        value = queue.front();
        queue.pop();
        
        return true;
    }


    /**
     * @fn      void markFinished()
     * @brief   Marks the queue as finished for operations.
     *
     * Signals that no more elements will be added to the queue, allowing waiting threads to
     * complete their operations gracefully once the queue is empty.
     */
    void markFinished() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            finished = true;
        }
        cond_var.notify_all();
    }


    /**
     * @fn      bool isFinished() const
     * @brief   Checks if the queue has been marked as finished.
     *
     * Allows threads to check whether the queue has been marked as finished, indicating that
     * no more elements will be added and operations can wind down once the queue is empty.
     *
     * @return True if the queue is marked as finished; otherwise, false.
     */
    bool isFinished() const {
        return finished;
    }
};

#endif