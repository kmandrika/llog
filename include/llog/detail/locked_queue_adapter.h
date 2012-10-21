#ifndef __LLOG_DETAIL_LOCKED_QUEUE_ADAPTER__
#define __LLOG_DETAIL_LOCKED_QUEUE_ADAPTER__

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

#include <queue>
#include <cstddef>
#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

namespace llog { namespace detail {

//! locked_queue_adapter -------------------------------------------------------

template<typename Message>
struct locked_queue_adapter {
public:
    typedef Message message_type;

public:
    locked_queue_adapter();
   ~locked_queue_adapter();

    bool enqueue(message_type* message);            //! Enqueue a message.

    template<typename SmartPtr>
    bool dequeue(SmartPtr& message);                //! Dequeue a message.

    bool empty() const;

private:
    boost::mutex mutex_;
    std::queue<message_type *> queue_;
};

}}

//! implementation -------------------------------------------------------------

template<typename Message>
llog::detail::locked_queue_adapter<Message>::locked_queue_adapter()
{
}

template<typename Message>
llog::detail::locked_queue_adapter<Message>::~locked_queue_adapter()
{
    boost::scoped_ptr<message_type> message;
    while (dequeue(message)) {}
}

template<typename Message>
inline bool llog::detail::locked_queue_adapter<Message>::enqueue(message_type* message)
{
    boost::mutex::scoped_lock lock(mutex_);

    queue_.push(message);
    return true;
}

template<typename Message>
template<typename SmartPtr>
inline bool llog::detail::locked_queue_adapter<Message>::dequeue(SmartPtr& message)
{
    boost::mutex::scoped_lock lock(mutex_);

    if (queue_.empty())
        return false;

    message.reset(queue_.front());
    queue_.pop();
    return true;
}

template<typename Message>
inline bool llog::detail::locked_queue_adapter<Message>::empty() const
{
    return queue_.empty();
}

#endif /* __LLOG_DETAIL_LOCKED_QUEUE_ADAPTER__ */
