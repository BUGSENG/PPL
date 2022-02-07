#ifndef PPL_make_threadable_hh
#define PPL_make_threadable_hh 1

#ifdef PPL_THREAD_SAFE

#    include "Init_defs.hh"
#    include "globals_defs.hh"
#    include <utility>
#    include <type_traits>
#    include <thread>

namespace Parma_Polyhedra_Library
{

namespace Implementation
{

template <typename Function>
class Threadable
{
public:
explicit Threadable (Function f)
    : fun(f)
{
}

template <typename ... Args>
typename std::result_of<Function(Args...)>::type
operator() (Args &&... args) const
{
    Thread_Init thread_init;

    return fun(std::forward<Args>(args)...);
}

private:
Function fun;
}; // class Threadable

// RAII helper for the Interruptible callable.
struct Abandon_Init
{
    Abandon_Init ();
    ~Abandon_Init ();
    // Disable other special methods.
    Abandon_Init (const Abandon_Init &)            = delete;
    Abandon_Init (Abandon_Init &&)                 = delete;
    Abandon_Init & operator=(const Abandon_Init &) = delete;
    Abandon_Init & operator=(Abandon_Init &&)      = delete;

}; // struct Abandon_Init

// Similar to Threadable, but it also allows for thread interruption.
template <typename Function>
class Interruptible
{
public:
explicit Interruptible (Function f)
    : fun(f)
{
}

template <typename ... Args>
typename std::result_of<Function(Args...)>::type
operator() (Args &&... args) const
{
    Thread_Init  thread_init;
    Abandon_Init abandon_init;

    return fun(std::forward<Args>(args)...);
}

private:
Function fun;
}; // class Interruptible

} // namespace Implementation


//! Wraps function \p f to make it PPL-threadable.
/*!
   The resulting PPL-threadable function automatically performs
   PPL-related thread initialization and finalization steps.
   \example
   Suppose that a new thread (using the PPL) has to be created that
   executes function \c foo on arguments \c arg1 and \c arg2. Then
   the following code can be used:
   <code>std::thread t(make_threadable(foo), arg1, arg2);</code>
 */
template <typename Function>
inline Implementation::Threadable<Function>
make_threadable (Function f)
{
    return Implementation::Threadable<Function>(f);
};


//! Wraps function \p f to make it PPL-interruptible.
/*!
   The resulting PPL-interruptible function automatically performs
   PPL-related thread initialization and finalization steps.
   The corresponding thread is interruptible by calling function
   <code>interrupt_thread</code>, providing as input the thread id
   and the pointer to a Throwable object.
   As an example, see tests/Polyhedron/threadsafe2.cc
 */
template <typename Function>
inline Implementation::Interruptible<Function>
make_interruptible (Function f)
{
    return Implementation::Interruptible<Function>(f);
};

/*!
   \brief
   Tries to abandon the expensive computation in the given thread.

   Calling this function will try to record a request of abandoning
   the expensive computation going on in the given thread.
   The thread is assumed to be executing a callable object obtained
   by <code>make_interruptible</code>.

   If \c false is returned, then the request to interrupt the thread
   failed. Possible reasons are: the thread hasn't started yet; the thread
   already finished; the thread is executing a function which was not
   obtained by the <code>make_interruptible</code> helper.

   If \c true is returned, then the request to interrupt the thread
   was successfully recorded. In this case, interruption is not immediate:
   the thread will either complete normally in a limitd time or,
   if it entered an expensive computation path, will be interrupted
   by calling method <code>throw_me()</code> of object \p throwable.
 */
bool
interrupt_thread(const std::thread     & interruptible_thread,
                 const Throwable * const throwable);

} // namespace Parma_Polyhedra_Library

#endif // defined(PPL_THREAD_SAFE)

#endif // !defined(PPL_make_threadable_hh)

