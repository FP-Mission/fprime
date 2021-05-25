/*
 * TlmReportPacket.cpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#include <Fw/Tlm/PicturePacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    PicturePacket::PicturePacket() : m_id(0) {
        
    }

    PicturePacket::~PicturePacket() {
    }

    SerializeStatus PicturePacket::serialize(SerializeBufferBase& buffer) const {
        SerializeStatus stat;
        stat = serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

     return stat = buffer.serialize(this->packetBuffer);

    }

    SerializeStatus PicturePacket::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat;
        stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        stat = buffer.deserialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

       return stat = buffer.deserialize(this->packetBuffer);
    }

    void PicturePacket::setId(U16 id) {
        this->m_id = id;
    }

    U16 PicturePacket::getId(void) {
        return this->m_id;
    }

    Buffer* PicturePacket::getPacketBuffer(){
        return &packetBuffer;
    }

} /* namespace Fw */
