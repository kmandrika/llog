#ifndef __LLOG_H__
#define __LLOG_H__

//! Llog.
/*!
    A log layer around std::ostream which offers multi-threaded locked and
    lock-free regimes. The default ostream is std::cout.

    Copyright (C) 2012  Konstantin Mandrika

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <llog/config.h>

#include <string>
#include <ostream>
#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>

#include <llog/detail/locked_queue_adapter.h>
#include <llog/detail/levels.h>
#include <llog/detail/log.h>

#if !defined (LLOG_NO_LOCKFREE_QUEUE)
#        include <llog/detail/lockfree_queue_adapter.h>
#endif

namespace llog {

//! logger ---------------------------------------------------------------------

template<
        typename Message = std::string
      , typename Queue = detail::locked_queue_adapter<Message>
>
struct logger : boost::noncopyable {
public:
    typedef Queue queue_type;

public:
    logger();
   ~logger();

    void rebind(std::ostream* stream);
    void start();
    void stop();

    template<template <class> class Level>
    void operator ()(const Level<Message>& what);

private:
    boost::thread thread_;
    std::ostream* stream_;

#if !defined (LLOG_SLEEP_BASED_WRITE_THREAD)
    boost::condition_variable cv_;
    boost::mutex mutex_;
#endif

    queue_type queue_;
};

} // namespace llog

//! implementation -------------------------------------------------------------

template<typename Message, typename Queue>
llog::logger<Message, Queue>::logger() : stream_(&std::cout)
{
}

template<typename Message, typename Queue>
llog::logger<Message, Queue>::~logger()
{
    stop();

    //! Write everything that is remaining in the queue to the stream.
    llog::detail::emit_to_stream<std::string, Queue>(*stream_, queue_);
}

template<typename Message, typename Queue>
void llog::logger<Message, Queue>::rebind(std::ostream* stream)
{
    stream_ = stream;
}

template<typename Message, typename Queue>
void llog::logger<Message, Queue>::start()
{
    //! Already started.
    if (thread_.get_id() != boost::thread::id()) return;

    thread_ = boost::thread(
            llog::detail::log<Message, Queue>
          , boost::ref(*stream_)
          , boost::ref(queue_)
#if !defined (LLOG_SLEEP_BASED_WRITE_THREAD)
          , boost::ref(cv_)
          , boost::ref(mutex_)
#endif
    );
}

template<typename Message, typename Queue>
void llog::logger<Message, Queue>::stop()
{
    //! Nothing to stop.
    if (thread_.get_id() == boost::thread::id()) return;

#if !defined (LLOG_PROPAGATE_EXCEPTIONS)
    try {
#endif
        thread_.interrupt();
        thread_.join();
#if !defined (LLOG_PROPAGATE_EXCEPTIONS)
    }
    catch (const boost::thread_interrupted &) {
    }
#endif
}

template<typename Message, typename Queue>
template<template <class> class Level>
void llog::logger<Message, Queue>::operator ()(const Level<Message>& what)
{
    if (thread_.get_id() == boost::thread::id()) {
#if defined (LLOG_PROPAGATE_EXCEPTIONS)
        throw std::runtime_error("Logger thread hasn't been started");
#else
        return;
#endif
    }

    queue_.enqueue(llog::detail::expand(what.message(), what.prefix()));

#if !defined (LLOG_SLEEP_BASED_WRITE_THREAD)
    cv_.notify_one();
#endif
}

#endif /* __LLOG_H__ */
