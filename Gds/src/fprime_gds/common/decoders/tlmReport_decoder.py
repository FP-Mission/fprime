"""
@brief Channel Decoder class used to parse binary telemetry report data

Decoders are responsible for taking in serialized data and parsing it into
objects. Decoders receive serialized data (that had a specific descriptor) from
a distributer that it has been registered to. The distributer will send the
binary data after removing any length and descriptor headers.

Example data that would be sent to a decoder that parses channels:
    +-------------------+---------------------+------------ - - -
    | ID (4 bytes) | Time Tag (x bytes) | Data....
    +-------------------+---------------------+------------ - - -
                        depends on time context and base enabling

@date Created May 11, 2021
@author Jonathan Michel
"""

import copy
import logging
import json
import time
import sys, os
import requests
import socket
import base64

from enum import IntEnum
from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.decoders.decoder import Decoder
from fprime_gds.common.utils import config_manager

LOGGER = logging.getLogger("decoder")

class ReportIds(IntEnum):
    DEBUG_REPORT = 0x69
    FP1_MISSION_REPORT = 0xAB

class TlmReportDecoder(Decoder):
    """Decoder class for Channel data"""

    def __init__(self, ch_dict, config):
        """
        TlmReportDecoder class constructor

        Args:
            ch_dict: Channel telemetry dictionary. Channel IDs should be keys
                     and ChTemplate objects should be values

        Returns:
            An initialized channel decoder object.
        """
        super().__init__()
        self.name = socket.gethostname()

        if config is None:
            # Retrieve singleton for the configs
            config = config_manager.ConfigManager().get_instance()

        self.__dict = ch_dict
        self.id_obj = config.get_type("ch_id")

    def debug_report(self, value):
        # print(value)
        pass

    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary telemetry channel data to decode

        Returns:
            Parsed version of the channel telemetry data in the form of a
            ChData object or None if the data is not decodable
        """
        ptr = 0


        # Decode Report ID here...
        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        report_id = self.id_obj.val

        # Decode time...
        report_time = TimeType()
        report_time.deserialize(data, ptr)
        ptr += report_time.getSize()

        # @todo Get ids from dictionnary depending on channel name
        # @todo Dynamic reading of TlmReport file format ??
        self.debug_report("===== TLM REPORT id: 0x{:02X} =====".format(report_id))
        try:
            if report_id == ReportIds.DEBUG_REPORT: 

                (ptr, PR_NumPings) = self.decode_ch(0x4e, data, ptr, report_time)
                (ptr, BD_Cycles) = self.decode_ch(0x4c, data, ptr, report_time)

                return [PR_NumPings, BD_Cycles]
            elif report_id == ReportIds.FP1_MISSION_REPORT:                 
                (ptr, BD_Cycles) = self.decode_ch(0x4e, data, ptr, report_time)
                self.debug_report(BD_Cycles)
                (ptr, CommandDispatched) = self.decode_ch(0x3, data, ptr, report_time)
                self.debug_report(BD_Cycles)
                (ptr, CommandErrors) = self.decode_ch(0x4, data, ptr, report_time)
                self.debug_report(CommandErrors)
                (ptr, RckBlck_RSSI) = self.decode_ch(0x96, data, ptr, report_time)
                self.debug_report(RckBlck_RSSI)
                (ptr, TempProb_InternalTemperature) = self.decode_ch(0xa0, data, ptr, report_time)
                self.debug_report(TempProb_InternalTemperature)
                (ptr, TempProb_ExternalTemperature) = self.decode_ch(0xa1, data, ptr, report_time)
                self.debug_report(TempProb_ExternalTemperature)
                (ptr, BAROMETER_TEMP) = self.decode_ch(0xc8, data, ptr, report_time)
                self.debug_report(BAROMETER_TEMP)
                (ptr, BAROMETER_PRESS) = self.decode_ch(0xc9, data, ptr, report_time)
                self.debug_report(BAROMETER_PRESS)
                (ptr, BAROMETER_ALT) = self.decode_ch(0xca, data, ptr, report_time)
                self.debug_report(BAROMETER_ALT)
                (ptr, Gps_Position) = self.decode_ch(0x78, data, ptr, report_time)
                self.debug_report(Gps_Position)
                (ptr, Gps_LockState) = self.decode_ch(0x79, data, ptr, report_time)
                self.debug_report(Gps_LockState)
                (ptr, PiCam_PictureCnt) = self.decode_ch(0x82, data, ptr, report_time)
                self.debug_report(PiCam_PictureCnt)

                self.save_data(BAROMETER_PRESS, report_time,"../data/pressure.txt" )
                self.save_data(BAROMETER_ALT, report_time,"../data/altitude.txt" )
                self.save_data(Gps_Position, report_time,"../data/gps.txt" )
                self.send_gps(Gps_Position, report_time)

                return [BD_Cycles, CommandDispatched, CommandErrors, RckBlck_RSSI, TempProb_InternalTemperature, TempProb_ExternalTemperature, BAROMETER_TEMP, BAROMETER_PRESS, BAROMETER_ALT, Gps_Position, Gps_LockState, PiCam_PictureCnt]
                
            else:
                LOGGER.warning("TlmReport id 0x{:02X} does not exist".format(report_id))
                return None
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            LOGGER.warning("Unable to parse TlmReport 0x{:02X}. Check {}:{}".format(report_id, fname, exc_tb.tb_lineno))
            print(e)
            return None


    def decode_ch(self, chan_id, data, ptr, report_time):
        ch_temp = self.__dict[chan_id]                      # Get template
        try:
            val_obj = self.decode_ch_val(data, ptr, ch_temp)    # Decode channel value
            ptr += val_obj.getSize()                            # Increment pointer
        except:
            LOGGER.warning("Unable to decode channel 0x{:02X} from TlmReport".format(chan_id))
            raise
            
        return (ptr, ChData(val_obj, report_time, ch_temp))

    def decode_ch_val(self, val_data, offset, template):
        """
        Decodes the given channel's value from the given data

        Args:
            val_data: Data to parse the value out of
            offset: Location in val_data to start the parsing
            template: Channel Template object for the channel

        Returns:
            The channel's value as an instance of a class derived from
            the BaseType class. The val_data has been deserialized using this
            object, and so the channel value can be retrieved from the obj's
            val field.
        """
        # This line creates a new object of the same type as the template's
        # type_obj. This allows us to use the new object to deserialize and
        # store the data value. If we did not do this, the template's object
        # would be used to deserialize multiple times and channel objects
        # referencing the template's type object would seem to have their value
        # changed randomly
        val_obj = copy.deepcopy(template.get_type_obj())

        val_obj.deserialize(val_data, offset)

        return val_obj

    def save_data(self, data, epoch, path):
        val = data.get_val()
        epoch = epoch.seconds
        current_time = time.localtime(epoch)
        format_date = f"{current_time.tm_hour}:{current_time.tm_min}:{current_time.tm_sec}"
        json_data = []
        tmp = {"x":format_date,"y":val}
        try:
            with open(path) as json_file:
                json_data = json.load(json_file)
        except:
            pass
        json_data.append(tmp)
        with open(path, 'w') as json_file: 
            json.dump(json_data, json_file)

    def send_gps(self,data, epoch):
        url = "http://panama.internet-box.ch/gps"
        epoch = epoch.seconds
        lat = data.get_val()['latitude']
        long = data.get_val()['longitude']
        if lat !=0 and long != 0:
            data = {"date": epoch, "lat":lat, "long":long, "name":self.name}
            try:
                response = requests.post(url, data)
            except:
                print("error sending gps to server")

        


