/*
 * TelemChanImpl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include "Fw/Logger/Logger.hpp"

#include <stdio.h>

//#define DEBUG_PRINT(x,...) Fw::Logger::logMsg(x,##__VA_ARGS__);
#define DEBUG_PRINT(x,...)

namespace Svc {

    void TlmChanImpl::TlmRecv_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {

        // Compute index for entry
        NATIVE_UINT_TYPE index = this->doHash(id);
        TlmEntry* entryToUse = 0;
        TlmEntry* prevEntry = 0;

        // Search to see if channel has already been stored or a bucket needs to be added
        if (this->m_tlmEntries[this->m_activeBuffer].slots[index]) {
            entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
            for (NATIVE_UINT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
                if (entryToUse) {
                    if (entryToUse->id == id) { // found the matching entry
                        DEBUG_PRINT("Update channel 0x%.2X in buffer %d\n", id, this->m_activeBuffer);
                        break;
                    } else { // try next entry
                        prevEntry = entryToUse;
                        entryToUse = entryToUse->next;
                    }
                } else {
                    // Make sure that we haven't run out of buckets
                    FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
                    // add new bucket from free list
                    entryToUse = &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
                    prevEntry->next = entryToUse;
                    // clear next pointer
                    entryToUse->next = 0;
                    DEBUG_PRINT("Create bucket for channel 0x%.2X in buffer %d\n", id, this->m_activeBuffer);
                    break;
                }
            }
        } else {
            // Make sure that we haven't run out of buckets
            FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
            // create new entry at slot head
            this->m_tlmEntries[this->m_activeBuffer].slots[index] = &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
            entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
            DEBUG_PRINT("Create entry for channel 0x%.2X in buffer %d\n", id, this->m_activeBuffer);
            entryToUse->next = 0;
        }

        // copy into entry
        FW_ASSERT(entryToUse);
        entryToUse->used = true;
        entryToUse->id = id;
        entryToUse->updated = true;
        entryToUse->lastUpdate = timeTag;
        entryToUse->buffer = val;

    }
}
