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
            void setPictureId(U16 id);
            void setFrameId(U16 id);
            void setData(U8* data, U32 index, U32 lenght, U32 fileSize);

            // getters
            U16 getPictrueId(void);
            U16 getFrameId(void);
            U8* getData(void);
            U16 getNbPacket(void);
            
        PROTECTED:
            U16 m_pictrueId; 
            U16 m_frameId;
            U16 m_nbPacket;
            U8 m_data[256];
            U32 m_dataLenght; 

    };

} /* namespace Fw */

#endif /* TLMREPORTPACKET_HPP_ */
