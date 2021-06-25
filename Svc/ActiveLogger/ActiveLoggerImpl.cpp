/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>

#include <App/Top/Dictionnary.hpp>

namespace Svc {

    ActiveLoggerImpl::ActiveLoggerImpl(const char* name) : 
        ActiveLoggerComponentBase(name)
    {
        // set filter defaults
        this->m_filterState[FILTER_WARNING_HI].enabled =
                FILTER_WARNING_HI_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;
        this->m_filterState[FILTER_WARNING_LO].enabled =
                FILTER_WARNING_LO_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;
        this->m_filterState[FILTER_COMMAND].enabled =
                FILTER_COMMAND_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;
        this->m_filterState[FILTER_ACTIVITY_HI].enabled =
                FILTER_ACTIVITY_HI_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;
        this->m_filterState[FILTER_ACTIVITY_LO].enabled =
                FILTER_ACTIVITY_LO_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;
        this->m_filterState[FILTER_DIAGNOSTIC].enabled =
                FILTER_DIAGNOSTIC_DEFAULT?FILTER_ENABLED:FILTER_DISABLED;

        memset(m_filteredIDs,0,sizeof(m_filteredIDs));

        // Custom behaviour when TELEM_ID_FILTER_SIZE is 255 length
        // All possible event (eventCode is U8) are filtered by default
        if (TELEM_ID_FILTER_SIZE == 255) {
            for(int i = 0; i < TELEM_ID_FILTER_SIZE; i++) {
                this->m_filteredIDs[i] = i + 1;     // @todo Set to "i + 1" to disable all events - Currently to "0" for debug purpose
            }
            this->m_filteredIDs[DICT_LOG_PR_PingReceived - 1] = 0;  // Remove filter for PR_PingReceived - Debugging purpose
            this->m_filteredIDs[DICT_LOG_TLM_NOT_UPDATED - 1] = 0;
            this->m_filteredIDs[DICT_LOG_OpCodeCompleted - 1] = 0;
            this->m_filteredIDs[DICT_LOG_OpCodeError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_MalformedCommand - 1] = 0;
            this->m_filteredIDs[DICT_LOG_InvalidCommand - 1] = 0;
            this->m_filteredIDs[DICT_LOG_TooManyCommands - 1] = 0;
            this->m_filteredIDs[DICT_LOG_NoOpReceived - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_0 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_1 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_2 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_3 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_4 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_5 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_ASSERT_6 - 1] = 0;
            this->m_filteredIDs[DICT_LOG_AF_UNEXPECTED_ASSERT - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_PING_WARN - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_PING_LATE - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_PING_WRONG_KEY - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_CHECK_PING - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_CHECK_LOOKUP_ERROR - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_PING_UPDATED - 1] = 0;
            this->m_filteredIDs[DICT_LOG_HLTH_PING_INVALID_VALUES - 1] = 0;
            this->m_filteredIDs[DICT_LOG_GroundInterfaceError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_ID_FILTER_ENABLED - 1] = 0;
            this->m_filteredIDs[DICT_LOG_ID_FILTER_LIST_FULL - 1] = 0;
            this->m_filteredIDs[DICT_LOG_ID_FILTER_REMOVED - 1] = 0;
            this->m_filteredIDs[DICT_LOG_ID_FILTER_NOT_FOUND - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_OpenError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_ConfigError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_WriteError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_ReadError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_PortOpened - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_NoBuffers - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR1_BufferTooSmall - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_OpenError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_ConfigError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_WriteError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_ReadError - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_PortOpened - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_NoBuffers - 1] = 0;
            this->m_filteredIDs[DICT_LOG_DR2_BufferTooSmall - 1] = 0;
            this->m_filteredIDs[DICT_LOG_Gps_LockState - 1] = 0;
            this->m_filteredIDs[DICT_LOG_PiCam_PictureTaken - 1] = 0;
            this->m_filteredIDs[DICT_LOG_PiCam_PictureError - 1] = 0;
            // this->m_filteredIDs[DICT_LOG_RckBlck_CSQ - 1] = 0;
            this->m_filteredIDs[DICT_LOG_RckBlck_MessageReceived - 1] = 0;
            this->m_filteredIDs[DICT_LOG_RckBlck_NoMessageReceived - 1] = 0;
            // this->m_filteredIDs[DICT_LOG_RckBlck_UnableToSend - 1] = 0;
            this->m_filteredIDs[DICT_LOG_RckBlck_MailboxCheckFail - 1] = 0;
            this->m_filteredIDs[DICT_LOG_RckBlck_RingAlert - 1] = 0;
            // this->m_filteredIDs[DICT_LOG_RckBlck_CommandSent - 1] = 0;
            // this->m_filteredIDs[DICT_LOG_RckBlck_Response - 1] = 0;
        }
    }

    ActiveLoggerImpl::~ActiveLoggerImpl() {
    }

    void ActiveLoggerImpl::init(
            NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
            NATIVE_INT_TYPE instance /*!< The instance number*/
            ) {
        ActiveLoggerComponentBase::init(queueDepth,instance);
    }

    void ActiveLoggerImpl::LogRecv_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::LogSeverity severity, Fw::LogBuffer &args) {

        // make sure ID is not zero. Zero is reserved for ID filter.
        FW_ASSERT(id != 0);

        switch (severity) {
            case Fw::LOG_FATAL: // always pass FATAL
                break;
            case Fw::LOG_WARNING_HI:
                if (this->m_filterState[FILTER_WARNING_HI].enabled == FILTER_DISABLED) {
                   return;
                }
                break;
            case Fw::LOG_WARNING_LO:
                if (this->m_filterState[FILTER_WARNING_LO].enabled == FILTER_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_COMMAND:
                if (this->m_filterState[FILTER_COMMAND].enabled == FILTER_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_ACTIVITY_HI:
                if (this->m_filterState[FILTER_ACTIVITY_HI].enabled == FILTER_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_ACTIVITY_LO:
                if (this->m_filterState[FILTER_ACTIVITY_LO].enabled == FILTER_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_DIAGNOSTIC:
                if (this->m_filterState[FILTER_DIAGNOSTIC].enabled == FILTER_DISABLED) {
                    return;
                }
                break;
            default:
                FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(severity));
                return;
        }

        // check ID filters
        for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
            if (
              (m_filteredIDs[entry] == id) &&
              (severity != Fw::LOG_FATAL)
              ) {
                return;
            }
        }

        // send event to the logger thread
        this->loqQueue_internalInterfaceInvoke(id,timeTag,static_cast<QueueLogSeverity>(severity),args);

        // if connected, announce the FATAL
        if (Fw::LOG_FATAL == severity) {
            if (this->isConnected_FatalAnnounce_OutputPort(0)) {
                this->FatalAnnounce_out(0,id);
            }
        }
    }

    void ActiveLoggerImpl::loqQueue_internalInterfaceHandler(FwEventIdType id, Fw::Time &timeTag, QueueLogSeverity severity, Fw::LogBuffer &args) {
        // Serialize event
        this->m_logPacket.setId(id);
        this->m_logPacket.setTimeTag(timeTag);
        this->m_logPacket.setLogBuffer(args);
        this->m_comBuffer.resetSer();
        Fw::SerializeStatus stat = this->m_logPacket.serialize(this->m_comBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

        /*/ Deserialization example for fun
        Fw::LogPacket logPacketDeser;
        stat = logPacketDeser.deserialize(this->m_comBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        FwEventIdType logIdDeser = logPacketDeser.getId();
        printf("Log id: 0x%.2X\n", logIdDeser);
        if(logIdDeser == 0x4D) {    // PR_PingReceived
            U32 code;
            Fw::LogBuffer logBufferDeser(logPacketDeser.getLogBuffer());
            logBufferDeser.resetDeser();
            stat = logBufferDeser.deserialize(code);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
            printf("PR_PingReceived core : %u\n", code);
        }
        //*/

        if (this->isConnected_PktSend_OutputPort(0)) {
            this->PktSend_out(0, this->m_comBuffer,0);
        }
    }

    void ActiveLoggerImpl::SET_EVENT_FILTER_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, EventLevel FilterLevel, FilterEnabled FilterEnable) {
        if (  (FilterLevel > FILTER_DIAGNOSTIC) or
              (FilterLevel < FILTER_WARNING_HI) or
              (FilterEnable < FILTER_ENABLED) or
              (FilterEnable > FILTER_DISABLED)) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
            return;
        }
        this->m_filterState[FilterLevel].enabled = FilterEnable;
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::SET_ID_FILTER_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq, //!< The command sequence number
            U32 ID,
            IdFilterEnabled IdFilterEnable //!< ID filter state
        ) {
        // check parameter
        switch (IdFilterEnable) {
            case ID_ENABLED:
            case ID_DISABLED:
                break;
            default:
                printf("Invalid enum\n");
                this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
                return;
        }

        if (ID_ENABLED == IdFilterEnable) { // add ID
            // search list for existing entry
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == ID) {
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ID_FILTER_ENABLED(ID);
                    return;
                }
            }
            // if not already a match, search for an open slot
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == 0) {
                    this->m_filteredIDs[entry] = ID;
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ID_FILTER_ENABLED(ID);
                    return;
                }
            }
            // if an empty slot was not found, send an error event
            this->log_WARNING_LO_ID_FILTER_LIST_FULL(ID);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        } else { // remove ID
            // search list for existing entry
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == ID) {
                    this->m_filteredIDs[entry] = 0; // zero entry
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ID_FILTER_REMOVED(ID);
                    return;
                }
            }
            // if it gets here, wasn't found
            this->log_WARNING_LO_ID_FILTER_NOT_FOUND(ID);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        }

    }

    void ActiveLoggerImpl::DUMP_FILTER_STATE_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq //!< The command sequence number
        ) {

        // first, iterate through severity filters
        for (NATIVE_INT_TYPE filter = 0; filter < EventLevel_MAX; filter++) {
            this->log_ACTIVITY_LO_SEVERITY_FILTER_STATE(
                    static_cast<EventFilterState>(filter),
                    FILTER_ENABLED == this->m_filterState[filter].enabled
                    );
        }

        // iterate through ID filter
        for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
            if (this->m_filteredIDs[entry] != 0) {
                this->log_ACTIVITY_HI_ID_FILTER_ENABLED(this->m_filteredIDs[entry]);
            }
        }

        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::pingIn_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
        // return key
        this->pingOut_out(0,key);
    }

} // namespace Svc
