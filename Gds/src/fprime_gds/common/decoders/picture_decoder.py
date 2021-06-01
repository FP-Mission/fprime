
import copy
import logging
import time

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
        picture_id = self.id_obj.val


        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        frame_id = self.id_obj.val

        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        filesize = self.id_obj.val


        with open('/mnt/c/dev/HE-ARC/github/test.bin', 'ab') as file:  

            file.write(data[6:])


