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

        // Serialize depending on report id
        switch(this->m_id) {
            case DEBUG_REPORT:
                stat = buffer.serialize(this->data.BD_Cycles);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }

                stat = buffer.serialize(this->data.PR_NumPings);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                break;
            case FP1_MISSION_REPORT:
                /*stat = buffer.serialize(this->data.CommandErrors);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }*/
                stat = buffer.serialize(this->data.BD_Cycles);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.Ping_lateWarning);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.Eps_BatteryVoltage);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.TempProb_InternalTemperature);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.TempProb_ExternalTemperature);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }/*
                stat = buffer.serialize(this->data.THERMOMETER_TEMP);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.THERMOMETER_HUMI);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }*/
                stat = buffer.serialize(this->data.BAROMETER_TEMP);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.BAROMETER_PRESS);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.BAROMETER_ALT);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.gpsPosition);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.serialize(this->data.PiCam_PictureCnt);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }

                break;
            case 0:
                // default id
                break;
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

        // Serialize depending on report id
        switch(this->m_id) {
            case DEBUG_REPORT:
                stat = buffer.deserialize(this->data.BD_Cycles);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.PR_NumPings);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                break;
            case FP1_MISSION_REPORT:
                stat = buffer.deserialize(this->data.BD_Cycles);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }/*
                stat = buffer.deserialize(this->data.CommandErrors);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }*/
                stat = buffer.deserialize(this->data.Ping_lateWarning);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.Eps_BatteryVoltage);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.TempProb_InternalTemperature);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.TempProb_ExternalTemperature);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }/*
                stat = buffer.deserialize(this->data.THERMOMETER_TEMP);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.THERMOMETER_HUMI);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }*/
                stat = buffer.deserialize(this->data.BAROMETER_TEMP);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.BAROMETER_PRESS);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.BAROMETER_ALT);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.gpsPosition);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                stat = buffer.deserialize(this->data.PiCam_PictureCnt);
                if (stat != FW_SERIALIZE_OK) {
                    return stat;
                }
                break;
            case 0:
                // default id
                break;
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
