/*
 * TlmReportPacket.cpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#include <Fw/Tlm/TlmReportPacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TlmReportPacket::TlmReportPacket() : m_id(0) {
        this->m_type = FW_PACKET_TLM_REPORT;
    }

    TlmReportPacket::~TlmReportPacket() {
    }

    SerializeStatus TlmReportPacket::serialize(SerializeBufferBase& buffer) const {
        SerializeStatus stat;
#if !FW_AMPCS_COMPATIBLE
        stat = serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif
        stat = buffer.serialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

#if !FW_AMPCS_COMPATIBLE
        stat = buffer.serialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif

        stat = buffer.serialize(this->value);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        return stat;

    }

    SerializeStatus TlmReportPacket::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat;
#if !FW_AMPCS_COMPATIBLE
        stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif
        stat = buffer.deserialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

#if !FW_AMPCS_COMPATIBLE
        stat = buffer.deserialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif

        stat = buffer.deserialize(this->value);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        return stat;
    }

    void TlmReportPacket::setId(FwChanIdType id) {
        this->m_id = id;
    }

    void TlmReportPacket::setTimeTag(Time& timeTag) {
        this->m_timeTag = timeTag;
    }

    FwChanIdType TlmReportPacket::getId(void) {
        return this->m_id;
    }

    Time& TlmReportPacket::getTimeTag(void) {
        return this->m_timeTag;
    }
} /* namespace Fw */
