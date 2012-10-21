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

Llog depends on boost lockfree, which is included in this repo. Since the number of files is quite limited, you can easily change the source to fit your needs. I recommend checking out config.h.