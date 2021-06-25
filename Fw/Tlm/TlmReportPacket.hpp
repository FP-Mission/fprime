/*
 * TlmReportPacket.hpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#ifndef TLMREPORTPACKET_HPP_
#define TLMREPORTPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Time/Time.hpp>

#include <Fw/Tlm/PositionSerializable/PositionSerializableCustom.hpp>

namespace Fw {

    class TlmReportPacket : public ComPacket {
        public:

            enum {
                DEBUG_REPORT = 0x69,
                FP1_MISSION_REPORT = 0xAB
            };

            TlmReportPacket();
            virtual ~TlmReportPacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            // Buffer containing value must be remainder of buffer
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            // setters
            void setId(FwChanIdType id);
            void setTimeTag(Time& timeTag);

            // getters
            FwChanIdType getId(void);
            Time& getTimeTag(void);
            struct {
                U8 CommandDispatched;
                U8 CommandErrors;
                U8 Ping_lateWarning;
                U8 RckBlck_RSSI;
                U16 BD_Cycles;
                U16 PR_NumPings;
                U16 Eps_BatteryVoltage;
                I16 TempProb_InternalTemperature;
                I16 TempProb_ExternalTemperature;
                F32 THERMOMETER_TEMP;
                F32 THERMOMETER_HUMI;
                F32 BAROMETER_TEMP;
                F32 BAROMETER_PRESS;
                U16 BAROMETER_ALT;
                App::PositionSerCustom Gps_Position;
                U8 Gps_LockState;
                U8 PiCam_PictureCnt;
            } data;
            
        PROTECTED:
            FwChanIdType m_id; // !< Telemtetry report id (unused for now)
            Fw::Time m_timeTag; // !< time tag
    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
