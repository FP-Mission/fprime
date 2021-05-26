/*
 * PositionSerializableCustom.hpp
 * Dirty hack - Copied from App/FlexTrak/Position/PositionSerializableAc.hpp
 * Required by TlmReportPacket before FlexTrak is compiled and unable
 * to fix it otherwise ... :(
 */
#ifndef POSITIONSERCUSTOM_HPP_
#define POSITIONSERCUSTOM_HPP_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/StringType.hpp>
#include <stdio.h> // snprintf
#ifdef BUILD_UT
#include <iostream>
#include <Fw/Types/EightyCharString.hpp>
#endif
#endif

namespace App {

class PositionSerCustom : public Fw::Serializable {


public:

    enum {
        SERIALIZED_SIZE =
        sizeof(F64) +
        sizeof(F64) +
        sizeof(U16) +
        sizeof(U8)
    }; //!< serializable size of PositionSerCustom

    PositionSerCustom(void); //!< Default constructor
    PositionSerCustom(const PositionSerCustom* src); //!< pointer copy constructor
    PositionSerCustom(const PositionSerCustom& src); //!< reference copy constructor
    PositionSerCustom(F64 latitude, F64 longitude, U16 altitude, U8 satellite); //!< constructor with arguments
    const PositionSerCustom& operator=(const PositionSerCustom& src); //!< equal operator
    bool operator==(const PositionSerCustom& src) const; //!< equality operator
#ifdef BUILD_UT
    // to support GoogleTest framework in unit tests
    friend std::ostream& operator<<(std::ostream& os, const PositionSerCustom& obj);
#endif

    void set(F64 latitude, F64 longitude, U16 altitude, U8 satellite); //!< set values

    F64 getlatitude(void); //!< get member latitude
    F64 getlongitude(void); //!< get member longitude
    U16 getaltitude(void); //!< get member altitude
    U8 getsatellite(void); //!< get member satellite

    void setlatitude(F64 val); //!< set member latitude
    void setlongitude(F64 val); //!< set member longitude
    void setaltitude(U16 val); //!< set member altitude
    void setsatellite(U8 val); //!< set member satellite


    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const; //!< serialization function
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer); //!< deserialization function
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(Fw::StringBase& text) const; //!< generate text from serializable
#endif
protected:

    enum {
        TYPE_ID = 0x970781E1 //!< type id
    };

    F64 m_latitude; //<! latitude - 
    F64 m_longitude; //<! longitude - 
    U16 m_altitude; //<! altitude - 
    U8 m_satellite; //<! satellite - 
private:

};
} // end namespace App
#endif /* POSITIONSERCUSTOM_HPP_ */

