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

//! Define this to eliminate the boost lockfree dependency
//#define LLOG_NO_LOCKFREE_QUEUE

//! Supress boost thread exceptions
#define LLOG_PROPAGATE_EXCEPTIONS

//! Comment this out if your compiler doesn't support __attribute__
#define LLOG_DECLARE_WITH_ATTRIBUTES

//! The writer thread will sleep between each batch of writes
//  instead of being triggered by a condvar
//#define LLOG_SLEEP_BASED_WRITE_THREAD

#if defined (LLOG_SLEEP_BASED_WRITE_THREAD)
#        define LLOG_WRITER_THREAD_SLEEP_SECONDS 1
#endif

#if !defined (LLOG_NO_LOCKFREE_QUEUE)
#        define LLOG_INITIAL_LOCKFREE_QUEUE_SIZE 1000
#endif
