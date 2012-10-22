llog - a simple, threaded, header-only logger.
=================================
Sometimes you want a threaded logger. Sometimes you want it to come in two flavors, locked and lock-free. Do I have excellent news for you, friend!
Usage is simple:

```c++
#include <llog/llog.h>

int main(int argc, char* argv[])
{
    typedef llog::logger<> logger_t;

    logger_t logger;
    logger.start();

    logger(_I("This is an informational"));
    logger(_W("This is a warning"));
    logger(_P("Uh-oh, panic!"));
}
```

The above usage example will instantiate a locked logger which operates on std::string. Lets say, for some crazy reason, you think you need it to be lockfree:

```c++
#include <llog/llog.h>

int main(int argc, char* argv[])
{
    typedef llog::logger<
            std::string
          , llog::detail::lockfree_queue_adapter<std::string>
    > logger_t;

    logger_t logger;
    logger.start();

    logger(_I("This is an informational"));
}

```

Since this is merely a wrapper around an std::ostream, the default stream is std::cout. It is the responsibility of the consumer to open a file and provide a stream (via `logger.rebind(&stream)`). Rebind is not thread safe, so one needs to call `::stop()`, `::rebind(...)`, and `::start()`.

Llog was compiled using g++ 4.4.6 and boost 1.49. The two referenced submodules are not yet part of boost (as of 20121022), but are available in their respective repos.