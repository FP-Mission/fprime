"""
rockblock.py:

Module containing a comm-layer adapter for the RockBlock API

@author jonathan
"""
import logging
import queue
import time
from typing import Text
import requests
import yaml
import os

from yaml.error import YAMLError

import fprime_gds.common.communication.adapters.base
import fprime_gds.common.logger

LOGGER = logging.getLogger("rockblock_adapter")

class RockBlockAdapter(fprime_gds.common.communication.adapters.base.BaseAdapter):
    """
    Send uplink packets to RockBlock
    """

    # Interval to send a KEEPALIVE packet. None will turn off KEEPALIVE.
    KEEPALIVE_INTERVAL = None
    # Data to send out as part of the KEEPALIVE packet. Should not be null nor empty.
    KEEPALIVE_DATA = b"sitting well"

    def __init__(self):
        """
        Initialize this adapter by creating a handler for UDP and TCP. A thread for the KEEPALIVE application packets
        will be created, if the interval is not none.
        """
        self.stop = False
        self.keepalive = None
        self.data_chunks = queue.Queue()
        self.blob = b""

        # @todo define path with command argument
        creds = None
        try:
            creds = yaml.safe_load(open(os.getcwd() + "/rockblock_creds.yaml"))
            self.imei = creds['credentials']['imei']
            self.username = creds['credentials']['username']
            self.password = creds['credentials']['password']
        except (FileNotFoundError, KeyError) as e:
            print(e)
            print("In order to use RockBlock adater to send command to the RockBlock API, first create 'rockblock_creds.yaml' at current location ({}) with the following content:".format(os.getcwd()))
            print("=============")
            print("credentials:")
            print("\timei : '<RockBlock IMEI>'")
            print("\tusername : '<Rock Seven Core username>'")
            print("\tpassword : '<Rock Seven Core Password>'")
            print("=============")
            exit()

    def open(self):
        pass      

    def close(self):
        """Close the adapter, by setting the stop flag"""
        self.stop = True

    def th_handler(self, handler):
        """Adapter thread function"""
        handler.open()
        while not self.stop:
            self.data_chunks.put(handler.read())
        handler.close()

    def write(self, frame):
        """
        Send a given framed bit of data by sending it out the RockBlock API

        :param frame: framed data packet to send out
        :return: True, when data was sent. False otherwise.
        """

        # Test
        #url = 'https://webhook.site/8583cfed-9f92-4840-bde4-7090449f38c3'
        #myobj = {
        #    'imei': '288168435816856',
        #    'username': 'myemail@ici.com',
        #    'password': rockblock_creds.creds['imei'],
        #    'data': frame.hex()
        #}

        # RockBlock
        url = 'https://rockblock.rock7.com/rockblock/MT'
        headers = {"Accept": "text/plain"}
        myobj = {
            'imei': self.imei,
            'username': self.username,
            'password': self.password,
            'data': frame.hex()
        }
        
        x = requests.post(url, data = myobj, headers=headers)

        if x.status_code == requests.codes.ok:
            response = x.text.split(",")
            if response[0] == "ERROR":
                print("Error sending command to RockBock: ({}) {}", response[1], response[2])
            elif response[1] == "OK":
                print("Command sent to RockBlock")
                return True
        else:
            print("Unable to reach RockBlock API")
            print(x)
            print(x.text)
        
        return False
        
        
    def read(self, timeout=0.500):
        """
        RockBlock do not receive messages
        """
        
        print("This function should not be called")

        return b""

    def th_alive(self, interval):
        """
        Run this thread in order to accept incoming connections and spiral them off into another thread for handling the
        given client.
        """
        while not self.stop:
            self.write(RockBlockAdapter.KEEPALIVE_DATA)
            time.sleep(interval)

    @classmethod
    def get_arguments(cls):
        """
        Returns a dictionary of flag to argparse-argument dictionaries for use with argparse to setup arguments.

        :return: dictionary of flag to argparse arguments for use with argparse
        """
        return {
            
        }

    @classmethod
    def check_arguments(cls, args):
        """
        Code that should check arguments of this adapter. If there is a problem with this code, then a "ValueError"
        should be raised describing the problem with these arguments.

        :param args: arguments as dictionary
        """
        pass

