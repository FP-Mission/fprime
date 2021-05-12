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

from enum import IntEnum
from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.decoders.decoder import Decoder
from fprime_gds.common.utils import config_manager

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

        if config is None:
            # Retrieve singleton for the configs
            config = config_manager.ConfigManager().get_instance()

        self.__dict = ch_dict
        self.id_obj = config.get_type("ch_id")

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
        # print("===== TLM REPORT id: 0x{:02X} =====".format(report_id))
        if report_id == ReportIds.DEBUG_REPORT: 

            (ptr, PR_NumPings) = self.decode_ch(0x4e, data, ptr, report_time)
            (ptr, BD_Cycles) = self.decode_ch(0x4c, data, ptr, report_time)

            return [PR_NumPings, BD_Cycles]
        elif report_id == ReportIds.FP1_MISSION_REPORT: 
            print("Decode FP1_MISSION TlmReport")
            # @todo
        else:
            print("TlmReport id 0x{:02X} does not exist".format(report_id))
            return None

    def decode_ch(self, chan_id, data, ptr, report_time):
        ch_temp = self.__dict[chan_id]                      # Get template
        val_obj = self.decode_ch_val(data, ptr, ch_temp)    # Decode channel value
        ptr += val_obj.getSize()                            # Increment pointer
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
