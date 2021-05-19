/**
 * \file
 * \author T. Canham
 * \brief Implementation file for channelized telemetry storage component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include "Fw/Logger/Logger.hpp"

#include <stdio.h>

//#define DEBUG_PRINT(x, ...)  printf(x, ##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Svc {

    TlmChanImpl::TlmChanImpl(const char* name) : TlmChanComponentBase(name)
    {
        // clear data
        this->m_activeBuffer = 0;
        // clear slot pointers
        for (NATIVE_UINT_TYPE entry = 0; entry < TLMCHAN_NUM_TLM_HASH_SLOTS; entry++) {
            this->m_tlmEntries[0].slots[entry] = 0;
            this->m_tlmEntries[1].slots[entry] = 0;
        }
        // clear buckets
        for (NATIVE_UINT_TYPE entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            this->m_tlmEntries[0].buckets[entry].used = false;
            this->m_tlmEntries[0].buckets[entry].updated = false;
            this->m_tlmEntries[0].buckets[entry].bucketNo = entry;
            this->m_tlmEntries[0].buckets[entry].next = 0;
            this->m_tlmEntries[0].buckets[entry].id = 0;
            this->m_tlmEntries[1].buckets[entry].used = false;
            this->m_tlmEntries[1].buckets[entry].updated = false;
            this->m_tlmEntries[1].buckets[entry].bucketNo = entry;
            this->m_tlmEntries[1].buckets[entry].next = 0;
            this->m_tlmEntries[1].buckets[entry].id = 0;
        }
        // clear free index
        this->m_tlmEntries[0].free = 0;
        this->m_tlmEntries[1].free = 0;

#if TLMCHAN_MODE == 1
        printf("TlmChan is configured to downlink data through TlmPackets\n");
#elif TLMCHAN_MODE == 2
        printf("TlmChan is configured to downlink data through TlmReportsPackets\n");
#else
        printf("!! Tlm downlink is disabled %u. Please set TLMCHAN_MODE in TlmChanImplCfg.hpp to configure Tlm mode\n", TLMCHAN_MODE);
#endif
    }

    TlmChanImpl::~TlmChanImpl() {
    }

    void TlmChanImpl::init(
            NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
            NATIVE_INT_TYPE instance /*!< The instance number*/
            ) {
        TlmChanComponentBase::init(queueDepth,instance);
    }

    NATIVE_UINT_TYPE TlmChanImpl::doHash(FwChanIdType id) {
        return (id % TLMCHAN_HASH_MOD_VALUE)%TLMCHAN_NUM_TLM_HASH_SLOTS;
    }

    void* TlmChanImpl::findEntry(TlmSet* tlmSet, FwChanIdType id) {
        NATIVE_UINT_TYPE index = this->doHash(id);
        TlmEntry* entry = 0;
        
        if (tlmSet->slots[index]) {
            entry = tlmSet->slots[index];
            for (NATIVE_UINT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
                if (entry) {
                    if (entry->id == id) { // found the matching entry
                        return entry;
                    } else { // try next entry
                        entry = entry->next;
                    }
                } else {
                    // According to TlmRecv_handler, at this step the algorithm add a new bucket
                    // In our case (reading), it means that the id was not found
                    DEBUG_PRINT("No bucket for id 0x%.2X in TlmSet %p\n", id, tlmSet);
                    return 0;
                }
            }
        } else {
            // According to TlmRecv_handler, at this step the algorithm create a new entry at slot head
            // In our case (reading), it means that the id was not found
            DEBUG_PRINT("No slot for id 0x%.2X in TlmSet %p\n", id, tlmSet);
            return 0;
        }
    }

    void TlmChanImpl::pingIn_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
        // return key
        this->pingOut_out(0,key);
    }

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    void TlmChanImpl ::
    DUMP_CHANNEL_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 id
    ) {
        TlmEntry* entryBuffer0 = 0;
        TlmEntry* entryBuffer1 = 0;
        TlmEntry* entryToUse = 0;

        // Only write packets if connected
        if (not this->isConnected_PktSend_OutputPort(0)) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }      
        
        // @todo Kinf of dirty was to handle mutual exclusion
        // The idea is to keep FPrime internal logic with double buffer
        // and minimise the effect of the dumpSearchMutex during standard
        // Run_handler and TlmRecv_handler
        this->lock();               // avoid TlmRecv_handler call
        dumpSearchMutex.lock();    // avoid Run_handler

        // Search in both buffers
        entryBuffer0 = (TlmEntry*)findEntry(&this->m_tlmEntries[0], id);
        entryBuffer1 = (TlmEntry*)findEntry(&this->m_tlmEntries[1], id);

        // Use entry depending on results
        if(entryBuffer0 != 0 && entryBuffer1 == 0) {
            DEBUG_PRINT("Channel 0x%.2X found in buffer 0\n");
            entryToUse = entryBuffer0;
        } else if (entryBuffer0 == 0 && entryBuffer1 != 0) {
            DEBUG_PRINT("Channel 0x%.2X found in buffer 1\n");
            entryToUse = entryBuffer1;
        } else if (entryBuffer0 != 0 && entryBuffer1 != 0) {
            // If channel is in both buffer, chose most up to date
            if(entryBuffer0->lastUpdate > entryBuffer1->lastUpdate) {
                DEBUG_PRINT("Channel 0x%.2X found in buffer 0 (up to date)\n");
                entryToUse = entryBuffer0;
            } else {
                DEBUG_PRINT("Channel 0x%.2X found in buffer 1 (up to date)\n");
                entryToUse = entryBuffer1;
            }
        }

        if(entryToUse != 0) {
            // Downlink channel
            FW_ASSERT(entryToUse);
            this->m_tlmPacket.setId(entryToUse->id);
            this->m_tlmPacket.setTimeTag(entryToUse->lastUpdate);
            this->m_tlmPacket.setTlmBuffer(entryToUse->buffer);
            this->m_comBuffer.resetSer();
            Fw::SerializeStatus stat = this->m_tlmPacket.serialize(this->m_comBuffer);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
            
            this->PktSend_out(0,this->m_comBuffer,0);

            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
        } else {
            this->log_WARNING_LO_TLM_NOT_UPDATED(id);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
        }

        dumpSearchMutex.unLock();  // permit Run_handler call
        this->unLock();             // permit TlmRecv_handler call
    }
}
