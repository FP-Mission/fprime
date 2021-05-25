/*
 * TlmReportPacket.hpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#ifndef TLMREPORTPACKET_HPP_
#define TLMREPORTPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Buffer/Buffer.hpp>

namespace Fw {

    class PicturePacket : public ComPacket {
        public:

            PicturePacket();
            virtual ~PicturePacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            // Buffer containing value must be remainder of buffer
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            // setters
            void setId(U16 id);

            // getters
            U16 getId(void);
            Buffer* getPacketBuffer();
            
        PROTECTED:
            U16 m_id; // !< Telemtetry report id (unused for now)
            Buffer packetBuffer;
    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
