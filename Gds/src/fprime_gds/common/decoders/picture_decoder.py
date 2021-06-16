
import copy
import logging
import time
import os

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

        self.data_size = 64

        self.counter_frame = -1

        self.packet_received = []

        self.current_picture_id = -1

        self.data_stored = []

        self.pictureFull = False

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
        nb_packet = self.id_obj.val

        print("Frame received {} {}".format(picture_id, frame_id))

        if picture_id != self.current_picture_id:
            self.packet_received = [False] * nb_packet
            self.data_stored = [None] * nb_packet
            self.current_picture_id = picture_id
            self.counter_frame = -1
            self.pictureFull= False
 
        self.counter_frame = frame_id
        self.packet_received[frame_id] = True
        self.data_stored[frame_id] = data[-self.data_size:]

        
        with open(f'../data/frame/{picture_id}.txt', 'a') as file: 
                file.write(f"{picture_id}, {frame_id}, {nb_packet}\n")

        if all(self.packet_received) and self.pictureFull == False:
            self.pictureFull = True
            print(f"all picture {picture_id} received")
            
            with open(f'../fprime/Gds/src/fprime_gds/flask/static/img/pictures/bin/{picture_id}.bin', 'wb') as file:  
                for d in self.data_stored:   
                    file.write(d)
            
            os.system(f"ssdv -d ../fprime/Gds/src/fprime_gds/flask/static/img/pictures/bin/{picture_id}.bin ../fprime/Gds/src/fprime_gds/flask/static/img/pictures/{picture_id}.jpeg")

            with open(f'../data/picture.txt', 'a') as file: 
                file.write(f"{picture_id}\n")

        return 1


