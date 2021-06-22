#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>

#include <time.h> 

namespace Svc {

    ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName) :
        PassiveTextLoggerComponentBase(compName) {
    }

    void ConsoleTextLoggerImpl::init(void) {
        PassiveTextLoggerComponentBase::init();
    }

    ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl(void) {}

    void ConsoleTextLoggerImpl::TextLogger_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text) {
        time_t time;
        char time_c[18];
        struct tm * ptm;

        const char *severityString = "UNKNOWN";
        switch (severity) {
            case Fw::TEXT_LOG_FATAL:
                severityString = "FATAL";
                break;
            case Fw::TEXT_LOG_WARNING_HI:
                severityString = "WARNING_HI";
                break;
            case Fw::TEXT_LOG_WARNING_LO:
                severityString = "WARNING_LO";
                break;
            case Fw::TEXT_LOG_COMMAND:
                severityString = "COMMAND";
                break;
            case Fw::TEXT_LOG_ACTIVITY_HI:
                severityString = "ACTIVITY_HI";
                break;
            case Fw::TEXT_LOG_ACTIVITY_LO:
                severityString = "ACTIVITY_LO";
                break;
            case Fw::TEXT_LOG_DIAGNOSTIC:
                severityString = "DIAGNOSTIC";
                break;
            default:
                severityString = "SEVERITY ERROR";
                break;
        }

        time = timeTag.getSeconds();
        ptm = gmtime(&time);
        strftime(time_c, 18, "%d/%m/%y %H:%M:%S", ptm);

        Fw::Logger::logMsgRaw("[%s] (%d:%d,%d) - EVENT %s 0x%02X: %s\n",
                reinterpret_cast<POINTER_CAST>(time_c), timeTag.getTimeBase(), timeTag.getSeconds(), timeTag.getUSeconds(), 
                reinterpret_cast<POINTER_CAST>(severityString), id, reinterpret_cast<POINTER_CAST>(text.toChar()));
    }
}
