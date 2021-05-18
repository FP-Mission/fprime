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

#include <stdio.h>

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

    void TlmChanImpl::pingIn_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
        // return key
        this->pingOut_out(0,key);
    }


}
