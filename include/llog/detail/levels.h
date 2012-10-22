#ifndef __LLOG_DETAIL_LEVELS__
#define __LLOG_DETAIL_LEVELS__

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

namespace llog { namespace detail {

//! level_base -----------------------------------------------------------------

template<typename Message>
struct level_base {
public:
    level_base(const Message& message) : message_(message) {}
    virtual ~level_base() {}

    virtual const Message& prefix() const = 0;
    const Message& message() const { return message_; }

private:
    Message message_;
};

//! levels ---------------------------------------------------------------------

#define LLOG_DETAIL_DEFINE_LOG_LEVEL(Level, Prefix)            \
    template<typename Message>                                  \
    struct Level : level_base<Message> {                        \
    public:                                                     \
        typedef level_base<Message> super;                      \
        typedef Message message_type;                           \
                                                                \
    public:                                                     \
        Level(const Message& message)                           \
            : super(message), prefix_(Prefix) {}                \
       ~Level() {}                                              \
                                                                \
        const Message& prefix() const { return prefix_; }       \
                                                                \
    private:                                                    \
        Message prefix_;                                        \
    }

LLOG_DETAIL_DEFINE_LOG_LEVEL(_I_impl, "i");       //! Informational.
LLOG_DETAIL_DEFINE_LOG_LEVEL(_W_impl, "w");       //! Warning.
LLOG_DETAIL_DEFINE_LOG_LEVEL(_E_impl, "e");       //! Error.
LLOG_DETAIL_DEFINE_LOG_LEVEL(_P_impl, "!panic!"); //! Panic.

}}

//! Convenience (specialize for other string types) ----------------------------
//! Ideally, these functions will be optimized via RVO.

#define LLOG_DETAIL_DEFINE_FOR_CONCRETE_TYPE(Level, Type)                  \
    inline llog::detail::Level ## _impl<Type> Level(const Type& message) { \
        return llog::detail::Level ## _impl<Type>(message);                \
    }

LLOG_DETAIL_DEFINE_FOR_CONCRETE_TYPE(_I, std::string);
LLOG_DETAIL_DEFINE_FOR_CONCRETE_TYPE(_W, std::string);
LLOG_DETAIL_DEFINE_FOR_CONCRETE_TYPE(_E, std::string);
LLOG_DETAIL_DEFINE_FOR_CONCRETE_TYPE(_P, std::string);

#endif /* __LLOG_DETAIL_LEVELS__ */
