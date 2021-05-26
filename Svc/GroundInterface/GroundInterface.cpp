// ====================================================================== 
// \title  GroundInterface.cpp
// \author lestarch
// \brief  cpp file for GroundInterface component implementation class
// ====================================================================== 

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Log/LogPacket.hpp>
#include <Svc/GroundInterface/GroundInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Tlm/PicturePacket.hpp"
#include <string.h>

#include "App/Config/PiCameraConfig.hpp"
#include "Os/FileSystem.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

namespace Svc {

  const U32 GroundInterfaceComponentImpl::MAX_DATA_SIZE = 2048;
  const TOKEN_TYPE GroundInterfaceComponentImpl::START_WORD = static_cast<TOKEN_TYPE>(0xdeadbeef);
  const TOKEN_TYPE GroundInterfaceComponentImpl::END_WORD = static_cast<TOKEN_TYPE>(0xcafecafe);

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  GroundInterfaceComponentImpl ::
    GroundInterfaceComponentImpl(
        const char *const compName
    ) : GroundInterfaceComponentBase(compName),
        m_ext_buffer(m_buffer, GND_BUFFER_SIZE),
        m_data_size(0),
        m_in_ring(m_in_buffer, GND_BUFFER_SIZE)
  {

  }

  void GroundInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    GroundInterfaceComponentBase::init(instance);
  }

  GroundInterfaceComponentImpl ::
    ~GroundInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------
  void GroundInterfaceComponentImpl::pictureIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 path 
      )
      {
        std::ostringstream osPictureSsdv;
        osPictureSsdv << App::BIN_DIRECTORY << path <<".bin";
        printf("%s\n",osPictureSsdv.str().c_str());
        std::ifstream in(osPictureSsdv.str().c_str(),std::ios::binary | std::ios::ate);
        U32 nbPacket = in.tellg()/256;
        printf("nb image packet : %d\n",nbPacket);
        in.seekg (0, std::ios::end);
        Fw::PicturePacket picturePacket;
        Fw::Buffer* pictureBuffer = picturePacket.getPacketBuffer();
        Fw::ComBuffer buffer;
        U8 data[256];
        pictureBuffer->setSize(256);
        pictureBuffer->setData(data);
        char* dataChar = reinterpret_cast<char*>(data);
        U16 index = 0;
        for(U16 i = 0; i<nbPacket ;i++){
            picturePacket.setId(i);
            in.read((char*)dataChar,256);
            index += 256;
            in.seekg(index);
            buffer.resetSer();
            picturePacket.serialize(buffer);
            frame_send(buffer.getBuffAddr(),buffer.getBuffLength(),Fw::ComPacket::FW_PACKET_PICTURE_PACKET);
        }

        in.close();
      }
  void GroundInterfaceComponentImpl ::
    downlinkPort_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
      FW_ASSERT(data.getBuffLength() <= MAX_DATA_SIZE);
      frame_send(data.getBuffAddr(), data.getBuffLength());
  }

  void GroundInterfaceComponentImpl ::
    fileDownlinkBufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
      FW_ASSERT(fwBuffer.getSize() <= MAX_DATA_SIZE);
      frame_send(fwBuffer.getData(), fwBuffer.getSize(), Fw::ComPacket::FW_PACKET_FILE);
      fileDownlinkBufferSendOut_out(0, fwBuffer);
  }

  void GroundInterfaceComponentImpl ::
    readCallback_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &buffer
    )
  {
      processBuffer(buffer);
  }

  void GroundInterfaceComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    )
  {
      // TODO: replace with a call to a buffer manager
      Fw::Buffer buffer = m_ext_buffer;
      // Call read poll if it is hooked up
      if (isConnected_readPoll_OutputPort(0)) {
          readPoll_out(0, buffer);
          processBuffer(buffer);
      }
  }

  void GroundInterfaceComponentImpl::frame_send(U8 *data, TOKEN_TYPE size, FwPacketDescriptorType packet_type) {
      // TODO: replace with a call to a buffer manager
      Fw::Buffer buffer = m_ext_buffer;
      Fw::SerializeBufferBase& buffer_wrapper = buffer.getSerializeRepr();
      buffer_wrapper.resetSer();
      // True size is supplied size plus sizeof(FwPacketDescriptorType) if a packet_type other than "UNKNOWN" was supplied.
      // This is because if not UNKNOWN, the packet_type is serialized too.  Otherwise it is assumed the PACKET_TYPE is
      // already the first token in the UNKNOWN typed buffer.
      U32 true_size = (packet_type != Fw::ComPacket::FW_PACKET_UNKNOWN) ? size + sizeof(FwPacketDescriptorType) : size;
      // START_WORD + true size + data + END_WORD
      U32 total_size = sizeof(TOKEN_TYPE) + sizeof(U32) + true_size + sizeof(TOKEN_TYPE);
      // Serialize data
      FW_ASSERT(GND_BUFFER_SIZE >= total_size, GND_BUFFER_SIZE, total_size);
      buffer_wrapper.serialize(START_WORD);
      buffer_wrapper.serialize(static_cast<TOKEN_TYPE>(true_size));
      // Explicitly set the packet type, if it didn't come with the data already
      if (packet_type != Fw::ComPacket::FW_PACKET_UNKNOWN) {
          buffer_wrapper.serialize(packet_type);
      }
      buffer_wrapper.serialize(data, size, true);
      buffer_wrapper.serialize(static_cast<TOKEN_TYPE>(END_WORD));
      // Setup for sending by truncating unused data
      buffer.setSize(buffer_wrapper.getBuffLength());
      FW_ASSERT(buffer.getSize() == total_size, buffer.getSize(), total_size);

        /*/ Deserialization example for fun
        Fw::SerializeBufferBase& deserBufferWrapper = buffer.getSerializeRepr();
        deserBufferWrapper.resetDeser();
        deserBufferWrapper.setBuffLen(buffer.getSize());

        TOKEN_TYPE token;
        Fw::SerializeStatus stat = deserBufferWrapper.deserialize(token);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        FW_ASSERT(token == START_WORD);

        TOKEN_TYPE dataSize;
        FwPacketDescriptorType packetType;

        stat = deserBufferWrapper.deserialize(dataSize);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

        stat = deserBufferWrapper.deserialize(packetType);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

        if(packetType == Fw::ComPacket::FW_PACKET_LOG && dataSize == 17) {
            Fw::ComBuffer comBuffer(deserBufferWrapper.getBuffAddrLeft(), dataSize-1);
            comBuffer.resetDeser();

            Fw::LogPacket logPacket;
            
            stat = logPacket.deserialize(comBuffer);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
            
            // Skip Com packet
            stat = deserBufferWrapper.deserializeSkip(dataSize - 1);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

            // Deserialize END_WORD
            stat = deserBufferWrapper.deserialize(token);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
            FW_ASSERT(token == END_WORD);

            // ?? logPacket.getId() does not work :(
            printf("Received LogPacket of size %u for event 0x%.2x\n", dataSize, logPacket.getId());
            return;
        } else {
            printf("PacketType %u of size %u\n", packetType, dataSize);
        }
        //*/

      write_out(0, buffer);
  }

  void GroundInterfaceComponentImpl ::
    routeComData()
  {
      // Read the packet type from the data buffer
      FwPacketDescriptorType packet_type = Fw::ComPacket::FW_PACKET_UNKNOWN;

      //read packet descriptor in size agnostic way
      U8 packet_descriptor_size = sizeof(FwPacketDescriptorType);
      U8 packet_type_bytes[packet_descriptor_size];
      Fw::SerializeStatus stat = m_in_ring.peek(packet_type_bytes, packet_descriptor_size, HEADER_SIZE);
      //m_in_ring.peek(packet_type, HEADER_SIZE); // this way is only valid for 4byte packet descriptors
      if(stat == Fw::FW_SERIALIZE_OK)
      {
        // unpack Big Endian encoded bytes
        packet_type = 0;
        for(int i = 0; i < packet_descriptor_size; i++)
        {
          packet_type <<= 8;
          packet_type |= packet_type_bytes[i];
        }
      }

      // Process variable type
      switch (packet_type) {
          case Fw::ComPacket::FW_PACKET_COMMAND: {
              Fw::ComBuffer com;
              m_in_ring.peek(com.getBuffAddr(), m_data_size, HEADER_SIZE);
              // Reset com buffer for sending out data
              com.setBuffLen(m_data_size);
              uplinkPort_out(0, com, 0);
              break;
          }
          case Fw::ComPacket::FW_PACKET_FILE: {
              // If file uplink is possible, handle files.  Otherwise ignore.
              if (isConnected_fileUplinkBufferGet_OutputPort(0) &&
                  isConnected_fileDownlinkBufferSendOut_OutputPort(0)) {
                  Fw::Buffer buffer = fileUplinkBufferGet_out(0, m_data_size);
                  m_in_ring.peek(buffer.getData(), m_data_size - sizeof(packet_type), HEADER_SIZE + sizeof(packet_type));
                  buffer.setSize(m_data_size - sizeof(packet_type));
                  fileUplinkBufferSendOut_out(0, buffer);
              }
              break;
          }
          default:
              return;
      }
  }

  void GroundInterfaceComponentImpl ::
    processRing()
  {
      // Header items for the packet
      TOKEN_TYPE start;
      U32 checksum; //TODO: make this run a CRC32
      // Inner-loop, process ring buffer looking for at least the header
      while (m_in_ring.get_remaining_size() >= HEADER_SIZE) {
          m_data_size = 0;
          // Peek into the header and read out values
          Fw::SerializeStatus status = m_in_ring.peek(start, 0);
          FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
          status = m_in_ring.peek(m_data_size, sizeof(TOKEN_TYPE));
          FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
          // Check the header for correctness
          if (start != START_WORD || m_data_size >= MAX_DATA_SIZE) {
              m_in_ring.rotate(1);
              continue;
          }
          // Check for enough data to deserialize everything otherwise break and wait for more.
          else if (m_in_ring.get_remaining_size() < (HEADER_SIZE + m_data_size + sizeof(END_WORD))) {
              break;
          }
          // Continue with the data portion and checksum
          m_in_ring.peek(checksum, HEADER_SIZE + m_data_size);
          // Check checksum
          if (checksum == END_WORD) {
              routeComData();
              m_in_ring.rotate(HEADER_SIZE + m_data_size + sizeof(U32));
          }
          // Failed checksum, keep looking for valid message
          else {
              m_in_ring.rotate(1);
          }
      }
  }

  void GroundInterfaceComponentImpl ::
    processBuffer(Fw::Buffer& buffer)
  {
      NATIVE_UINT_TYPE buffer_offset = 0;
      while (buffer_offset < buffer.getSize()) {
          NATIVE_UINT_TYPE ser_size = (buffer.getSize() >= m_in_ring.get_remaining_size(true)) ?
              m_in_ring.get_remaining_size(true) : static_cast<NATIVE_UINT_TYPE>(buffer.getSize());
          m_in_ring.serialize(buffer.getData() + buffer_offset, ser_size);
          buffer_offset = buffer_offset + ser_size;
          processRing();
      }
  }
} // end namespace Svc
