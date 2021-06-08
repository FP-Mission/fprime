// ======================================================================
// \title  LinuxSerialDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include "Fw/Types/BasicTypes.hpp"

//#define DEBUG_PRINT(x, ...) printf(x, ##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

bool LinuxSerialDriverComponentImpl::open(const char* const device,
                                          UartBaudRate baud, UartFlowControl fc,
                                          UartParity parity, bool block) {
    /*
     Their config:

     2-wire UART on BLSP2 is accessible via /dev/ttyHS0
     2-wire UART on BLSP9 is accessible via /dev/ttyHS3
     4-wire UART on BLSP6 is accessible via /dev/ttyHS2

     Our config:
     tty-1 bam-2 2-wire
     tty-2 bam-6 4-wire
     tty-3 bam-9 4-wire
     */

    // TODO remove printf

    NATIVE_INT_TYPE fd;
    NATIVE_INT_TYPE stat;

    this->m_device = device;

    DEBUG_PRINT("Opening UART device %s\n", device);

    // TODO cant use O_NDELAY and block (it is same as O_NONBLOCK), so removing
    // NDELAY for now
    /*
     The O_NOCTTY flag tells UNIX that this program doesn't want to be the
     "controlling terminal" for that port. If you don't specify this then any
     input (such as keyboard abort signals and so forth) will affect your
     process. Programs like getty(1M/8) use this feature when starting the login
     process, but normally a user program does not want this behavior.

     The O_NDELAY flag tells UNIX that this program doesn't care what state the
     DCD signal line is in - whether the other end of the port is up and
     running. If you do not specify this flag, your process will be put to sleep
     until the DCD signal line is the space voltage.
     */
    fd = ::open(device, O_RDWR | O_NOCTTY);  // | O_NDELAY);
    // fd = open(device, O_RDWR | O_NONBLOCK | O_SYNC);

    if (fd == -1) {
        DEBUG_PRINT("open UART device %s failed.\n", device);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_DR_OpenError(_arg, this->m_fd, _err);
        return false;
    } else {
        DEBUG_PRINT("Successfully opened UART device %s fd %d\n", device, fd);
    }

    this->m_fd = fd;

    // Configure blocking reads
    struct termios cfg;

    stat = tcgetattr(fd, &cfg);
    if (-1 == stat) {
        DEBUG_PRINT("tcgetattr failed: (%d): %s\n", stat, strerror(errno));
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
        return false;
    } else {
        DEBUG_PRINT("tcgetattr passed.\n");
    }

    /*
     If MIN > 0 and TIME = 0, MIN sets the number of characters to receive
     before the read is satisfied. As TIME is zero, the timer is not used.

     If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be
     satisfied if a single character is read, or TIME is exceeded (t = TIME *0.1
     s). If TIME is exceeded, no character will be returned.

     If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read
     will be satisfied if MIN characters are received, or the time between two
     characters exceeds TIME. The timer is restarted every time a character is
     received and only becomes active after the first character has been
     received.

     If MIN = 0 and TIME = 0, read will be satisfied immediately. The number of
     characters currently available, or the number of characters requested will
     be returned. According to Antonino (see contributions), you could issue a
     fcntl(fd, F_SETFL, FNDELAY); before reading to get the same result.
     */
    // wait for at least 1 byte for 1 second
    // TODO VMIN should be 0 when using VTIME, and then it would give the
    // timeout behavior Tim wants
    cfg.c_cc[VMIN] = 0;    // TODO back to 0
    cfg.c_cc[VTIME] = 10;  // 1 sec, TODO back to 10

    stat = tcsetattr(fd, TCSANOW, &cfg);
    if (-1 == stat) {
        DEBUG_PRINT("tcsetattr failed: (%d): %s\n", stat, strerror(errno));
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
        return false;
    } else {
        DEBUG_PRINT("tcsetattr passed.\n");
    }

    // Set flow control
    if (fc == HW_FLOW) {
        struct termios t;

        int stat = tcgetattr(fd, &t);
        if (-1 == stat) {
            DEBUG_PRINT("tcgetattr UART fd %d failed\n", fd);
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
            return false;
        }

        // modify flow control flags
        t.c_cflag |= CRTSCTS;

        stat = tcsetattr(fd, TCSANOW, &t);
        if (-1 == stat) {
            DEBUG_PRINT("tcsetattr UART fd %d failed\n", fd);
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
            return false;
        }
    }

    NATIVE_INT_TYPE relayRate = B0;
    switch (baud) {
        // TODO add more as needed
        case BAUD_9600:
            relayRate = B9600;
            break;
        case BAUD_19200:
            relayRate = B19200;
            break;
        case BAUD_38400:
            relayRate = B38400;
            break;
        case BAUD_57600:
            relayRate = B57600;
            break;
        case BAUD_115K:
            relayRate = B115200;
            break;
        case BAUD_230K:
            relayRate = B230400;
            break;
#if defined TGT_OS_TYPE_LINUX
        case BAUD_460K:
            relayRate = B460800;
            break;
        case BAUD_921K:
            relayRate = B921600;
            break;
#endif
        default:
            FW_ASSERT(0, baud);
            break;
    }

    struct termios newtio;

    stat = tcgetattr(fd, &newtio);
    if (-1 == stat) {
        DEBUG_PRINT("tcgetattr UART fd %d failed\n", fd);
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
        return false;
    }

    // CS8 = 8 data bits, CLOCAL = Local line, CREAD = Enable Receiver
    // TODO PARENB for parity bit
    /*
      Even parity (7E1):
      options.c_cflag |= PARENB
      options.c_cflag &= ~PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
      Odd parity (7O1):
      options.c_cflag |= PARENB
      options.c_cflag |= PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
     */
    newtio.c_cflag = CS8 | CLOCAL | CREAD;

    switch (parity) {
        case PARITY_ODD:
            newtio.c_cflag |= (PARENB | PARODD);
            break;
        case PARITY_EVEN:
            newtio.c_cflag |= PARENB;
            break;
        case PARITY_NONE:
            newtio.c_cflag &= ~PARENB;
            break;
        default:
            FW_ASSERT(0, parity);
            break;
    }

    // Set baud rate:
    stat = cfsetispeed(&newtio, relayRate);
    stat = cfsetospeed(&newtio, relayRate);

    // Raw output:
    newtio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;

    // TODO if parity, then do input parity too
    // options.c_iflag |=INPCK;
    newtio.c_iflag = INPCK;

    // Flush old data:
    (void)tcflush(fd, TCIFLUSH);

    // Set attributes:
    stat = tcsetattr(fd, TCSANOW, &newtio);
    if (-1 == stat) {
        DEBUG_PRINT("tcsetattr UART fd %d failed\n", fd);
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_DR_OpenError(_arg, fd, _err);
        return false;
    }

    // All done!
    Fw::LogStringArg _arg = device;
    this->log_ACTIVITY_HI_DR_PortOpened(_arg);

    // Default mode is non-binary
    this->binaryMode = 0;
    this->endChar = '\n';

    return true;
}

LinuxSerialDriverComponentImpl ::~LinuxSerialDriverComponentImpl(void) {
    if (this->m_fd != -1) {
        DEBUG_PRINT("Closing UART device %d\n", this->m_fd);

        (void)close(this->m_fd);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void LinuxSerialDriverComponentImpl ::serialSend_handler(
    const NATIVE_INT_TYPE portNum, Fw::Buffer& serBuffer) {
    if (this->m_fd == -1) {
        return;
    }

    unsigned char* data = serBuffer.getData();
    NATIVE_INT_TYPE xferSize = serBuffer.getSize();

    //printf("LinuxSerialDriver buffer size %u\n", xferSize);

    NATIVE_INT_TYPE stat = ::write(this->m_fd, data, xferSize);

    // TODO no need to delay for writes b/c the write blocks
    // not sure if it will block until everything is transmitted, but seems to

    if (-1 == stat || stat != xferSize) {
        Fw::LogStringArg _arg = this->m_device;
        this->log_WARNING_HI_DR_WriteError(_arg, stat);
    }
}

void LinuxSerialDriverComponentImpl ::binaryMode_handler(
        const NATIVE_INT_TYPE portNum,
        U16 length
) {
    DEBUG_PRINT("Binary mode set (%lu)\n", length);
    this->binaryMode = length;
}

void LinuxSerialDriverComponentImpl ::serialReadTaskEntry(void* ptr) {
    Drv::SerialReadStatus serReadStat;

    LinuxSerialDriverComponentImpl* comp =
        static_cast<LinuxSerialDriverComponentImpl*>(ptr);

    // Wait one second to ensure m_buffSet array has been set 
    // by consummer component
    sleep(1);

    while(1) {
        Fw::Buffer buff;
        int stat = 0;
        char c;

        // Allocate new buffer

        comp->m_readBuffMutex.lock();
        // search for open entry
        NATIVE_INT_TYPE entryFound = false;
        for (NATIVE_INT_TYPE entry = 0; entry < DR_MAX_NUM_BUFFERS; entry++) {
            if (comp->m_buffSet[entry].available) {
                comp->m_buffSet[entry].available = false;
                buff = comp->m_buffSet[entry].readBuffer;
                buff.setSize(0);
                entryFound = true;
                break;
            }
        }
        comp->m_readBuffMutex.unLock();

        //
        if (not entryFound) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_DR_NoBuffers(_arg);
            serReadStat = Drv::SER_NO_BUFFERS;
            comp->serialRecv_out(0,buff,serReadStat);
            // to avoid spinning, wait 50 ms
            Os::Task::delay(50);
            continue;
        }

        while(true) {
            // End loop if thread has to stop
            if (comp->m_quitReadThread) {
                return;
            }

            stat = ::read(comp->m_fd, &c, 1);

            if (stat == -1) {
                Fw::LogStringArg _arg = comp->m_device;
                comp->log_WARNING_HI_DR_ReadError(_arg, errno);
                serReadStat = Drv::SER_OTHER_ERR;
                break;
            } else if (stat == 0) { // no data
                continue;   // retry
            } else if(stat == 1) {    // 1 data read

                if(buff.getSize() + 1 <= UART_READ_BUFF_SIZE) {
                    char* ptr = reinterpret_cast<char *>(buff.getData());

                    ptr += buff.getSize();      // append data
                    DEBUG_PRINT("Append data 0x%02X (%c) to %lu\n", c, c, buff.getSize());
                    memcpy(ptr, &c, 1);
                    buff.setSize(buff.getSize() + 1);        // increment size

                    if(comp->binaryMode) {
                        // Decrement counter and send data when full
                        if (--comp->binaryMode == 0)
                        {
                            serReadStat = Drv::SER_OK;
                            DEBUG_PRINT("Send binary (%u)\n", buff.getSize());
                            comp->serialRecv_out(0, buff, serReadStat);
                            break; // Allocate new buffer for next data
                        } 
                    } else {
                        // If non-binary mode, return buffer when endChar is detected
                        if(c == comp->endChar) {
                            serReadStat = Drv::SER_OK;
                            DEBUG_PRINT("Send %s\n", buff.getData());
                            comp->serialRecv_out(0, buff, serReadStat);  
                            break; // Allocate new buffer for next data
                        }
                    }

                } else {
                    // Not enough place in buffer, end character not found
                    // Throw error
                    Fw::LogStringArg deviceName(comp->m_device);
                    DEBUG_PRINT("End of received sequence never found\n");
                    comp->log_WARNING_HI_DR_BufferTooSmall(deviceName, buff.getSize(), stat);
                    serReadStat = Drv::SER_BUFFER_TOO_SMALL;
                    comp->serialRecv_out(0, buff, serReadStat);  
                }
            } else { // stat > 1
                printf("SERIAL ERROR - Should read 1 character only \n");
            }

        }

    }
}

void LinuxSerialDriverComponentImpl ::startReadThread(
    NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize,
    NATIVE_INT_TYPE cpuAffinity) {
    Fw::EightyCharString task("SerReader");
    Os::Task::TaskStatus stat = this->m_readTask.start(
        task, 0, priority, stackSize, serialReadTaskEntry, this, cpuAffinity);
    FW_ASSERT(stat == Os::Task::TASK_OK, stat);
}

void LinuxSerialDriverComponentImpl ::quitReadThread(void) {
    this->m_quitReadThread = true;
}

}  // end namespace Drv
