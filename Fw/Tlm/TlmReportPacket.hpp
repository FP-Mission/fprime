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

namespace Fw {

    class TlmReportPacket : public ComPacket {
        public:

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
                U32 PR_NumPings;
                U32 BD_Cycles;
            } data;

            
        PROTECTED:
            FwChanIdType m_id; // !< Telemtetry report id (unused for now)
            Fw::Time m_timeTag; // !< time tag
    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
