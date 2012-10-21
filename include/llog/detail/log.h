#ifndef __LLOG_DETAIL_LOG__
#define __LLOG_DETAIL_LOG__

//! Llog.
/*!
    A log layer around std::ostream which offers locked and lock-free, single
    and multi-threaded regimes. The default ostream is std::cout.

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

#include <ostream>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace llog { namespace detail {

//! emit_to_stream -------------------------------------------------------------

template<typename Message, typename Queue>
inline void emit_to_stream(std::ostream& stream, Queue& queue)
{
    boost::scoped_ptr<Message> dequeued;
    while (queue.dequeue(dequeued)) { stream << *dequeued << std::endl; }
}

//! log ------------------------------------------------------------------------

#if defined (LLOG_SLEEP_BASED_WRITE_THREAD)
#        define parameters__ std::ostream& stream, Queue& queue
#else
#        define parameters__ std::ostream& stream, Queue& queue, boost::condition_variable& cv, boost::mutex& mutex
#endif

#if defined (LLOG_DECLARE_WITH_ATTRIBUTES)
template<typename Message, typename Queue> void log(parameters__) __attribute__((noreturn));
#endif

template<typename Message, typename Queue> void log(parameters__)
{
    for (;;) {
#if defined (LLOG_SLEEP_BASED_WRITE_THREAD)
        emit_to_stream<Message, Queue>(stream, queue);
        boost::this_thread::sleep(boost::posix_time::time_duration(
                0, 0, LLOG_WRITER_THREAD_SLEEP_SECONDS, 0
        ));
#else
        boost::unique_lock<boost::mutex> lock(mutex);

        while (queue.empty()) cv.wait(lock);
        emit_to_stream<Message, Queue>(stream, queue);
#endif
    }
}

//! append (specialize for other string types) ---------------------------------

template<typename Message>
inline void append(Message* message, const std::string& value);

template<>
inline void append(std::string* message, const std::string& value)
{
    message->append(value);
}

//! expand ---------------------------------------------------------------------

template<typename Message>
inline Message* expand(const Message& message, const Message& level)
{
    Message* m = new Message;

    llog::detail::append(m, "[");
    llog::detail::append(m, boost::posix_time::to_simple_string(
            boost::posix_time::microsec_clock::universal_time()
    ));
    llog::detail::append(m, "] ");
    llog::detail::append(m, level);
    llog::detail::append(m, ": ");
    llog::detail::append(m, message);

    return m;
}

}}

#endif /* __LLOG_DETAIL_LOG__ */
