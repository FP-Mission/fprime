/*
 * TlmReportPacket.cpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#include <Fw/Tlm/PicturePacket.hpp>
#include <Fw/Types/Assert.hpp>

#include <cstring>

namespace Fw {

    PicturePacket::PicturePacket(){
        this->m_type = FW_PACKET_PICTURE;
    }

    PicturePacket::~PicturePacket() {
    }

    SerializeStatus PicturePacket::serialize(SerializeBufferBase& buffer) const {
        SerializeStatus stat;

        stat = serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_pictrueId);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_frameId);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        
        stat = buffer.serialize(this->m_dataLenght);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        for(U32 i = 0; i< m_dataLenght; i++){
            stat = buffer.serialize(this->m_data[i]);
            if (stat != FW_SERIALIZE_OK) {
                return stat;
            }
        }

       return stat;
    }

    SerializeStatus PicturePacket::deserialize(SerializeBufferBase& buffer) {
        SerializeStatus stat;

        stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        
        stat = buffer.deserialize(this->m_pictrueId);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_frameId);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        
        stat = buffer.deserialize(this->m_dataLenght);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        for(U32 i = 0; i< m_dataLenght; i++){
            stat = buffer.deserialize(this->m_data[i]);
            if (stat != FW_SERIALIZE_OK) {
                return stat;
            }
        }

       return stat;
    
    }

        void PicturePacket::setPictureId(U32 id){
            this->m_pictrueId = id;
        }
        void PicturePacket::setFrameId(U32 id){
            this->m_frameId = id;
        }
        void PicturePacket::setData(U8* data, U32 index,U32 lenght){
            std::memcpy(m_data,data+index*lenght,lenght);
            this->m_dataLenght = lenght;
        }

        // getters
        U32 PicturePacket::getPictrueId(void){
            return this->m_pictrueId;
        }
        U32 PicturePacket::getFrameId(void){
            return this->m_frameId;
        }
        U8* PicturePacket::getData(void){
            return this->m_data;
        }

} /* namespace Fw */
