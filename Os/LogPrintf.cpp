/**
 * File: Os/LogPrintf.cpp
 * Description: an implementation on the Os::Log abstraction that routes log messages into standard
 * printf calls.
 */
#include <Os/Log.hpp>

#include <stdio.h>
#include <time.h>

namespace Os {
    Log::Log() {

        // Register myself as a logger at construction time. If used in unison with LogDefault.cpp, this will
        // automatically create this as a default logger.
        this->registerLogger(this);
    }

    void Log::log(
        const char* fmt,
        POINTER_CAST a0,
        POINTER_CAST a1,
        POINTER_CAST a2,
        POINTER_CAST a3,
        POINTER_CAST a4,
        POINTER_CAST a5,
        POINTER_CAST a6,
        POINTER_CAST a7,
        POINTER_CAST a8,
        POINTER_CAST a9
    ) {
        timespec stime;
        time_t time;
        char time_c[18];
        struct tm * ptm;

        (void)clock_gettime(CLOCK_REALTIME,&stime);
        
        time = stime.tv_sec;
        ptm = gmtime(&time);
        strftime(time_c, 18, "%d/%m/%y %H:%M:%S", ptm);

        printf("[%s] ", time_c);
        this->logRaw(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }

    // Instance implementation
    void Log::logRaw(
        const char* fmt,
        POINTER_CAST a0,
        POINTER_CAST a1,
        POINTER_CAST a2,
        POINTER_CAST a3,
        POINTER_CAST a4,
        POINTER_CAST a5,
        POINTER_CAST a6,
        POINTER_CAST a7,
        POINTER_CAST a8,
        POINTER_CAST a9
    ) {
        (void) printf(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
        (void) fflush(stdout);
    }
}

