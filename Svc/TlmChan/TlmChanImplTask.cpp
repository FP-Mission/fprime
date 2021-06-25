/**
 * \file
 * \author T. Canham
 * \brief Implementation for telemetry channel component thread.
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Tlm/PositionSerializable/PositionSerializableCustom.hpp>
#include "Fw/Logger/Logger.hpp"

#include <App/Top/Dictionnary.hpp>

#include <stdio.h>
#include <time.h>

//#define DEBUG_PRINT(x,...) Fw::Logger::logMsg(x,##__VA_ARGS__);
#define DEBUG_PRINT(x,...)

namespace Svc {

    void TlmChanImpl::Run_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
        // Only write packets if connected
        if (not this->isConnected_PktSend_OutputPort(0)) {
            return;
        }

        // lock mutex long enough to modify active telemetry buffer
        // so the data can be read without worrying about updates
        this->lock();
        this->m_activeBuffer = 1 - this->m_activeBuffer;
        // set activeBuffer to not updated
        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            this->m_tlmEntries[this->m_activeBuffer].buckets[entry].updated = false;
        }
        this->unLock();

        dumpSearchMutex.lock();    // avoid DUMP_CHANNEL_cmdHandler

#if TLMCHAN_MODE == 1   // TlmPackets mode
        // Go through each entry and send a TlmPacket if it has been updated
        DEBUG_PRINT("Downlink activeBuffer %u\n", 1-this->m_activeBuffer);
        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            TlmEntry* p_entry = &this->m_tlmEntries[1-this->m_activeBuffer].buckets[entry];
            if ((p_entry->updated) && (p_entry->used)) {
                DEBUG_PRINT("\tDownlink 0x%.2X\n", p_entry->id);
                this->m_tlmPacket.setId(p_entry->id);
                this->m_tlmPacket.setTimeTag(p_entry->lastUpdate);
                this->m_tlmPacket.setTlmBuffer(p_entry->buffer);
                this->m_comBuffer.resetSer();
                Fw::SerializeStatus stat = this->m_tlmPacket.serialize(this->m_comBuffer);
                FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
                p_entry->updated = false;
                this->PktSend_out(0,this->m_comBuffer,0);
            }
        }
#elif TLMCHAN_MODE == 2 // TlmReportsPackets mode
        // Update data for TlmReportPacket
        // @todo Find a cleaner way/better place to do this
        U8 u8Val;
        U16 u16Val;
        U32 u32Val;
        F32 f32Val;
        I16 i16Val;
        App::PositionSerCustom position;
        // Channels that are in the other active buffer will be updated 
        // on the next call to Run_handler
        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            TlmEntry* p_entry = &this->m_tlmEntries[1-this->m_activeBuffer].buckets[entry];
            if (p_entry->used) {
                p_entry->buffer.resetDeser();
                switch (p_entry->id) {
                case DICT_TLM_CommandsDispatched:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.CommandDispatched = u8Val;
                    break;
                case DICT_TLM_CommandErrors:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.CommandErrors = u8Val;
                    break;
                case DICT_TLM_PR_NumPings:
                    p_entry->buffer.deserialize(u16Val);
                    m_tlmReportPacket.data.PR_NumPings = u16Val;
                    break;
                case DICT_TLM_BD_Cycles:
                    p_entry->buffer.deserialize(u16Val);
                    m_tlmReportPacket.data.BD_Cycles = u16Val;
                    break;
                case DICT_TLM_Eps_BatteryVoltage:
                    p_entry->buffer.deserialize(u16Val);
                    m_tlmReportPacket.data.Eps_BatteryVoltage = u16Val;
                    break;
                case DICT_TLM_PingLateWarnings:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.Ping_lateWarning = u8Val;
                    break;
                case DICT_TLM_Gps_Position:
                    p_entry->buffer.deserialize(position);
                    m_tlmReportPacket.data.Gps_Position = position;
                    break;
                case DICT_TLM_Gps_LockState:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.Gps_LockState = u8Val;
                    break;
                case DICT_TLM_RckBlck_RSSI:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.RckBlck_RSSI = u8Val;
                    break;
                case DICT_TLM_TempProb_InternalTemperature:
                    p_entry->buffer.deserialize(i16Val);
                    m_tlmReportPacket.data.TempProb_InternalTemperature = i16Val;
                    break;
                case DICT_TLM_TempProb_ExternalTemperature:
                    p_entry->buffer.deserialize(i16Val);
                    m_tlmReportPacket.data.TempProb_ExternalTemperature = i16Val;
                    break;
                case DICT_TLM_THERMOMETER_TEMP:
                    p_entry->buffer.deserialize(f32Val);
                    m_tlmReportPacket.data.THERMOMETER_TEMP = f32Val;
                    break;
                case DICT_TLM_THERMOMETER_HUMI:
                    p_entry->buffer.deserialize(f32Val);
                    m_tlmReportPacket.data.THERMOMETER_HUMI = f32Val;
                    break;
                case DICT_TLM_BAROMETER_TEMP:
                    p_entry->buffer.deserialize(f32Val);
                    m_tlmReportPacket.data.BAROMETER_TEMP = f32Val;
                    break;
                case DICT_TLM_BAROMETER_PRESS:
                    p_entry->buffer.deserialize(f32Val);
                    m_tlmReportPacket.data.BAROMETER_PRESS = f32Val;
                    break;
                case DICT_TLM_BAROMETER_ALT:
                    p_entry->buffer.deserialize(u16Val);
                    m_tlmReportPacket.data.BAROMETER_ALT = u16Val;
                    break;
                case DICT_TLM_PiCam_PictureCnt:
                    p_entry->buffer.deserialize(u8Val);
                    m_tlmReportPacket.data.PiCam_PictureCnt = u8Val;
                    break;
                default:
                    break;
                }
            }
        }

        // Generate TlmReportPacket
        Fw::Time time = getTime();
        manageTelemetry(time);
        this->m_tlmReportPacket.setId(Fw::TlmReportPacket::FP1_MISSION_REPORT); // DEBUG_REPORT
        this->m_tlmReportPacket.setTimeTag(time);
        this->m_comBuffer.resetSer();
        Fw::SerializeStatus stat = this->m_tlmReportPacket.serialize(this->m_comBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        FW_ASSERT(this->isConnected_PktSend_OutputPort(0));
        DEBUG_PRINT("Downlink TlmReport\n");
        this->PktSend_out(0,this->m_comBuffer,0);
        //*/
#endif
       dumpSearchMutex.unLock();    // avoid DUMP_CHANNEL_cmdHandler
    }
    
    void TlmChanImpl::manageTelemetry(Fw::Time& time){
        time_t t = time.getSeconds();
        tm tm;
        if (localtime_r(&t, &tm) == NULL) {
            return;
        }
        std::ostringstream stringTime;
        stringTime << tm.tm_mday<<"/"<<tm.tm_mon + 1<<"/"<<tm.tm_year + 1900<< " "<< tm.tm_hour
        << ":"<< tm.tm_min<<":"<<tm.tm_sec;
        std::ostringstream osTelemetry;
        osTelemetry << path;
        std::ofstream outFileTelemetry (osTelemetry.str(),std::ios::app );

        outFileTelemetry << stringTime.str() <<","<<m_tlmReportPacket.data.BD_Cycles <<"," <<m_tlmReportPacket.data.Ping_lateWarning <<","<< m_tlmReportPacket.data.Gps_Position.getlatitude() << 
        ","<<m_tlmReportPacket.data.Gps_Position.getlongitude()<<","<< m_tlmReportPacket.data.BAROMETER_TEMP <<","<< m_tlmReportPacket.data.BAROMETER_PRESS <<  
            ","<< m_tlmReportPacket.data.BAROMETER_ALT << ","<< m_tlmReportPacket.data.TempProb_InternalTemperature << ","<< m_tlmReportPacket.data.TempProb_ExternalTemperature << ","<< m_tlmReportPacket.data.Eps_BatteryVoltage <<
            ","<< m_tlmReportPacket.data.PiCam_PictureCnt <<","<<m_tlmReportPacket.data.THERMOMETER_HUMI <<"\n";

        outFileTelemetry.close();
    }
}