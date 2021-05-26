/*
 * PositionSerializableCustom.cpp
 * Dirty hack - Copied from App/FlexTrak/Position/PositionSerializableAc.cpp
 * Required by TlmReportPacket before FlexTrak is compiled and unable
 * to fix it otherwise ... :(
 */
#include <Fw/Tlm/PositionSerializable/PositionSerializableCustom.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/EightyCharString.hpp>
#endif
#include <cstring>
namespace App {
// public methods

PositionSerCustom::PositionSerCustom(void): Serializable() {

}

PositionSerCustom::PositionSerCustom(const PositionSerCustom& src) : Serializable() {
    this->set(src.m_latitude, src.m_longitude, src.m_altitude, src.m_satellite);
}

PositionSerCustom::PositionSerCustom(const PositionSerCustom* src) : Serializable() {
    FW_ASSERT(src);
    this->set(src->m_latitude, src->m_longitude, src->m_altitude, src->m_satellite);
}

PositionSerCustom::PositionSerCustom(F64 latitude, F64 longitude, U16 altitude, U8 satellite) : Serializable() {
    this->set(latitude, longitude, altitude, satellite);
}

const PositionSerCustom& PositionSerCustom::operator=(const PositionSerCustom& src) {
    this->set(src.m_latitude, src.m_longitude, src.m_altitude, src.m_satellite);
    return src;
}

bool PositionSerCustom::operator==(const PositionSerCustom& src) const {
    return (
        (src.m_latitude == this->m_latitude) &&
        (src.m_longitude == this->m_longitude) &&
        (src.m_altitude == this->m_altitude) &&
        (src.m_satellite == this->m_satellite) &&
        true);
}

void PositionSerCustom::set(F64 latitude, F64 longitude, U16 altitude, U8 satellite) {
    this->m_latitude = latitude;
    this->m_longitude = longitude;
    this->m_altitude = altitude;
    this->m_satellite = satellite;
}

F64 PositionSerCustom::getlatitude(void) {
    return this->m_latitude;
}

F64 PositionSerCustom::getlongitude(void) {
    return this->m_longitude;
}

U16 PositionSerCustom::getaltitude(void) {
    return this->m_altitude;
}

U8 PositionSerCustom::getsatellite(void) {
    return this->m_satellite;
}

void PositionSerCustom::setlatitude(F64 val) {
    this->m_latitude = val;
}
void PositionSerCustom::setlongitude(F64 val) {
    this->m_longitude = val;
}
void PositionSerCustom::setaltitude(U16 val) {
    this->m_altitude = val;
}
void PositionSerCustom::setsatellite(U8 val) {
    this->m_satellite = val;
}
Fw::SerializeStatus PositionSerCustom::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    // serialize type ID
    stat = buffer.serialize((U32)PositionSerCustom::TYPE_ID);
#endif

    stat = buffer.serialize(this->m_latitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_longitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_altitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_satellite);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

Fw::SerializeStatus PositionSerCustom::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    U32 typeId;

    stat = buffer.deserialize(typeId);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (typeId != PositionSerCustom::TYPE_ID) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
#endif

    stat = buffer.deserialize(this->m_latitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_longitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_altitude);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_satellite);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING  || BUILD_UT

void PositionSerCustom::toString(Fw::StringBase& text) const {

    static const char * formatString =
       "("
       "latitude = %lf, "
       "longitude = %lf, "
       "altitude = %u, "
       "satellite = %u"
       ")";

    // declare strings to hold any serializable toString() arguments


    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    (void)snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,formatString
       ,this->m_latitude
       ,this->m_longitude
       ,this->m_altitude
       ,this->m_satellite
    );
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

    text = outputString;
}

#endif

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const PositionSerCustom& obj) {
        Fw::EightyCharString str;
        obj.toString(str);
        os << str.toChar();
        return os;
    }
#endif
} // end namespace App
