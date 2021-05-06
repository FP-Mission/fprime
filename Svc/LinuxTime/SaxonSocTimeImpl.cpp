/*
 * SaxonSocTime.cpp
 * Fake time provider for test with SaxonSoc
 */

#include <Svc/LinuxTime/LinuxTimeImpl.hpp>
#include <Fw/Time/Time.hpp>
#include <time.h>

namespace Svc {

    LinuxTimeImpl::LinuxTimeImpl(const char* name) : TimeComponentBase(name)
    {
    }

    LinuxTimeImpl::~LinuxTimeImpl() {
    }

    void LinuxTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timespec stime;
        stime.tv_sec = 15;
        stime.tv_nsec = 1200;
        time.set(TB_WORKSTATION_TIME,0, stime.tv_sec, stime.tv_nsec);
    }

    void LinuxTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }

}
