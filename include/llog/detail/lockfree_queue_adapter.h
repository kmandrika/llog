#ifndef __LLOG_DETAIL_LOCKFREE_QUEUE_ADAPTER__
#define __LLOG_DETAIL_LOCKFREE_QUEUE_ADAPTER__

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

#include <cstddef>
#include <boost/lockfree/fifo.hpp>
#include <boost/scoped_ptr.hpp>

namespace llog { namespace detail {

//! lockfree_queue_adapter -----------------------------------------------------

template<typename Message>
struct lockfree_queue_adapter {
public:
    typedef Message message_type;

public:
    lockfree_queue_adapter();
   ~lockfree_queue_adapter();

    bool enqueue(message_type* message);            //! Enqueue a message.

    template<typename SmartPtr>
    bool dequeue(SmartPtr& message);                //! Dequeue a message.

    bool empty() const;

private:
    boost::lockfree::fifo<message_type *> queue_;
};

}}

//! implementation -------------------------------------------------------------

template<typename Message>
llog::detail::lockfree_queue_adapter<Message>::lockfree_queue_adapter()
      : queue_(LLOG_INITIAL_LOCKFREE_QUEUE_SIZE)
{
}

template<typename Message>
llog::detail::lockfree_queue_adapter<Message>::~lockfree_queue_adapter()
{
    boost::scoped_ptr<message_type> message;
    while (dequeue(message)) {}
}

template<typename Message>
inline bool llog::detail::lockfree_queue_adapter<Message>::enqueue(message_type* message)
{
    return queue_.enqueue(message);
}

template<typename Message>
template<typename SmartPtr>
inline bool llog::detail::lockfree_queue_adapter<Message>::dequeue(SmartPtr& message)
{
    return queue_.dequeue(message);
}

template<typename Message>
inline bool llog::detail::lockfree_queue_adapter<Message>::empty() const
{
    return const_cast<boost::lockfree::fifo<message_type *> &>(queue_).empty();
}

#endif /* __LLOG_DETAIL_LOCKFREE_QUEUE_ADAPTER__ */
