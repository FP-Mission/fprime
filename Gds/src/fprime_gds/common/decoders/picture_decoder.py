
import copy
import logging

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.decoders.decoder import Decoder
from fprime_gds.common.utils import config_manager
from fprime.common.models.serialize.numerical_types import  U16Type



class PictureDecoder(Decoder):
    """Decoder class for Channel data"""

    def __init__(self):

        super().__init__()

        self.config = config_manager.ConfigManager().get_instance()

        self.id_obj = U16Type()

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
        print(f"picture id : {report_id}")
        print(type(data))

        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        report_id = self.id_obj.val
        print(f"frame id : {report_id}")

        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        report_id = self.id_obj.val
        print(f"taille : {report_id}")

        with open('/mnt/c/dev/HE-ARC/github/test.bin', 'ab') as file:  

            file.write(data[6:])


