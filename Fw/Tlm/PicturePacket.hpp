/*
 * PicturePacket.hpp
 *
 *  Created on: May 11, 2021
 *      Author: Jonathan Michel
 */

#ifndef PICTUREPACKET_HPP_
#define PICTUREPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Time/Time.hpp>


namespace Fw {

    class PicturePacket : public ComPacket {
        public:

            PicturePacket();
            virtual ~PicturePacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            // Buffer containing value must be remainder of buffer
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            // setters
            void setPictureId(U32 id);
            void setFrameId(U32 id);
            void setData(U8* data, U32 lenght);

            // getters
            U32 getPictrueId(void);
            U32 getFrameId(void);
            U8* getData(void);
            
        PROTECTED:
            U32 m_pictrueId; 
            U32 m_frameId;
            U8* m_data;
            U32 m_dataLenght; 
    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
