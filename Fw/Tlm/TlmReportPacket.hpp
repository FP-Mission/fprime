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
            U32 value;  // test variable to store random u32 in report
            
        PROTECTED:
            FwChanIdType m_id; // !< Channel id
            Fw::Time m_timeTag; // !< time tag
    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
