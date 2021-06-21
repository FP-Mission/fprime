"""
framing.py:

This module defines all the framing constructs used when adapting the F prime ground system for use with communications
or elsewhere in the system. There is an abstract base class called FramerDeframe, which defines the standard pattern of
'frame', 'deframe', and the helper 'deframe_all'. This allows users to frame and deframe packets using base classes
that implement this pattern. The current list of implementation classes are:

1. FpFramerDeframer: a class used to write the now-standard F prime ground packet format
2. TcpServerFramerDeframer: a non-symmetric framer/deframer for use interacting with the Tcp Server packet format

@author lestarch
"""
import abc
import copy
import struct
import re
import binascii


def CHECKSUM_CALC(_):
    """ Initial checksum implementation for FpFramerDeframer. """
    return 0xCAFECAFE


class FramerDeframer(abc.ABC):
    """
    Abstract base class of the Framer/Deframer variety. Framers and Deframers have to define two methods, one for
    framing a set of bytes and one for deframing a set of bytes into packets.
    """

    @abc.abstractmethod
    def frame(self, data):
        """
        Frames outgoing data in the specified format. Expects incoming raw bytes to frame, and adds on the needed header
        and footer bytes. This new array of bytes is returned from the method.

        :param data: bytes to frame
        :return: array of raw bytes representing a framed packet. Should be ready for uplink.
        """

    @abc.abstractmethod
    def deframe(self, data, no_copy=False):
        """
        Deframes the incoming data from the specified format. Produces exactly one packet, and leftover bytes. Users
        wanting all packets to be deframed should call "deframe_all". If no full packet is available, this method
        returns None. Expects incoming raw bytes to deframe, and returns a deframed packet or None, and the leftover
        bytes that were unused. Will search and discard data up until a start token is found. Note: data will be
        consumed up to the first start token found.

        :param data: framed data bytes
        :param no_copy: (optional) will prevent extra copy if True, but "data" input will be destroyed.
        :return: (packet as array of bytes or None, leftover bytes)
        """

    def deframe_all(self, data, no_copy):
        """
        Deframes all available packets found in a single set of bytes by calling deframe until a None packet is
        retrieved. This list of packets, and the remaining bytes are returned

        :param data: framed data bytes
        :param no_copy: (optional) will prevent extra copy if True, but "data" input will be destroyed.
        :return:
        """
        packets = []
        #print(data)
        if not no_copy:
            data = copy.copy(data)
        while True:
            # print("\t{}".format(data))
            # Deframe and return only on None
            (packet, data) = self.deframe(data, no_copy=True)
            # print("\t\tFound {}".format(len(packets)))
            # print("\t\tpackets {}".format(packets))
            # print("\t\tleftOver {}".format(data))
            # print("\n")
            if packet is None:
                # print("Return {} {}".format(packets, data))
                return packets, data
            packets.append(packet)


class FpFramerDeframer(FramerDeframer):
    """
    Framing object used to read/write in the standard F prime format. This format is compatible with the standard ground
    system. It contains the following format for data:

    | token: start word |
    | token: data length |
    | bytes: F prime packet bytes (Fw::Comm, Fw::FilePacket, etc.) |
    | token: checksum |

    Where a token is a big-endian integer of TOKEN_SIZE bytes in length (see below)
    """

    # Size of an F prime framing token, and the type based on that size
    TOKEN_SIZE = 4
    # Total size of header data based on token size
    HEADER_SIZE = TOKEN_SIZE * 2
    # Size of checksum value, and the hardcoded value before CRC32 is available
    CHECKSUM_SIZE = 4
    MAXIMUM_DATA_SIZE = 4096

    # Filled by set_constants()
    TOKEN_TYPE = None
    HEADER_FORMAT = None
    START_TOKEN = None

    def __init__(self):
        """ Sets constants on construction. """
        # Setup the constants as soon as possible.
        FpFramerDeframer.set_constants()

    @classmethod
    def set_constants(clazz):
        """
        Setup the constants for the various token sizes. This will ensure that the system can read the tokens properly.
        This can be changed to make the framing more efficient.
        """
        if FpFramerDeframer.TOKEN_SIZE == 4:
            FpFramerDeframer.TOKEN_TYPE = "I"
            FpFramerDeframer.START_TOKEN = 0xDEADBEEF
        elif FpFramerDeframer.TOKEN_SIZE == 2:
            FpFramerDeframer.TOKEN_TYPE = "H"
            FpFramerDeframer.START_TOKEN = 0xBEEF
        elif FpFramerDeframer.TOKEN_SIZE == 1:
            FpFramerDeframer.TOKEN_TYPE = "B"
            FpFramerDeframer.START_TOKEN = 0xEF
        else:
            raise ValueError(
                "Invalid TOKEN_SIZE of {}".format(FpFramerDeframer.TOKEN_SIZE)
            )
        FpFramerDeframer.HEADER_FORMAT = ">" + (FpFramerDeframer.TOKEN_TYPE * 2)

    def frame(self, data):
        """
        Frames outgoing data in the F prime standard format. Expects incoming raw bytes to frame, and adds on the
        needed framing tokens to the front and end of the bytes.

        :param data: bytes to frame
        :return: array of raw bytes representing a framed packet. Should be ready for uplink.
        """
        framed = struct.pack(
            FpFramerDeframer.HEADER_FORMAT, FpFramerDeframer.START_TOKEN, len(data)
        )
        framed += data
        framed += struct.pack(">I", CHECKSUM_CALC(framed))
        return framed

    def deframe(self, data, no_copy=False):
        """
        Deframes the incoming data from the F prime standard format. Produces exactly one packet, and leftover bytes.
        Users wanting all packets to be deframed should call "deframe_all". If no full packet is available, this method
        returns None. Expects incoming raw bytes to deframe, and returns a deframed packet or None, and the leftover
        bytes that were unused. Will search and discard data up until a start token is found. Note: data will be
        consumed up to the first start token found.

        :param data: framed data bytes
        :param no_copy: (optional) will prevent extra copy if True, but "data" input will be destroyed.
        :return: (packet as array of bytes or None, leftover bytes)
        """ 
        if not no_copy:
            data = copy.copy(data)
        # Continue until there is not enough data for the header, or until a packet is found (return)
        while len(data) >= FpFramerDeframer.HEADER_SIZE:
            # Read header information including start token and size and check if we have enough for the total size
            start, data_size = struct.unpack_from(FpFramerDeframer.HEADER_FORMAT, data)
            total_size = (
                FpFramerDeframer.HEADER_SIZE
                + data_size
                + FpFramerDeframer.CHECKSUM_SIZE
            )
            # Invalid frame, rotate away a Byte and keep processing
            if (
                start != FpFramerDeframer.START_TOKEN
                or data_size >= FpFramerDeframer.MAXIMUM_DATA_SIZE
            ):
                data = data[1:]
                continue
            # If the pool is large enough to read the whole frame, then read it
            elif len(data) >= total_size:
                deframed, check = struct.unpack_from(
                    ">{}sI".format(data_size), data, FpFramerDeframer.HEADER_SIZE
                )
                # If the checksum is valid, return the packet. Otherwise continue to rotate
                if check == CHECKSUM_CALC(
                    data[: data_size + FpFramerDeframer.HEADER_SIZE]
                ):
                    data = data[total_size:]
                    return deframed, data
                # Bad checksum, rotate 1 and keep looking for non-garbage
                data = data[1:]
                continue
            # Case of not enough data for a full packet, return hoping for more later
            return None, data
        return None, data


class TcpServerFramerDeframer(FramerDeframer):
    """
    Framing object used to read/write in the Tcp Server format. This format is compatible with the older ground
    system. It contains the following formats for data:

    Uplink (command) data:
    | string: 'ZZZZ' |
    | bytes: F prime packet bytes (Fw::Comm, Fw::FilePacket, etc.) |

    Downlink (ground-system) data:
    | string: 'A5A5 GUI ' |
    | U32: length of data |
    | bytes: F prime packet bytes (Fw::Comm, Fw::FilePacket, etc.) |

    WARNING: this is a non-symmetric framer/deframer as the Tcp Server's protocol isn't symmetric. Therefore, it should
    never be used to deframe its own framed data.
    """

    START_STRING = "ZZZZ"

    def frame(self, data):
        """
        Frames outgoing data in the Tcp server outgoing format. Expects incoming raw bytes to frame, and adds on the
        needed framing tokens to the front and end of the bytes.

        :param data: bytes to frame
        :return: array of raw bytes representing a framed packet. Should be ready for uplink.
        """
        return b"A5A5 GUI %s%s" % (struct.pack(">I", len(data)), data)

    def deframe(self, data, no_copy=False):
        """
        Deframes the incoming data from the F prime standard format. Produces exactly one packet, and leftover bytes.
        Users wanting all packets to be deframed should call "deframe_all". If no full packet is available, this method
        returns None. Expects incoming raw bytes to deframe, and returns a deframed packet or None, and the leftover
        bytes that were unused. Will search and discard data up until a start token is found. Note: data will be
        consumed up to the first start token found.

        :param data: framed data bytes
        :param no_copy: (optional) will prevent extra copy if True, but "data" input will be destroyed.
        :return: (packet as array of bytes or None, leftover bytes)
        """
        if not no_copy:
            data = copy.copy(data)
        # Shift over to ZZZZ
        while len(data) > 4 and data[0:4] != b"ZZZZ":
            data = data[1:]
        # Break out of data when not enough
        if len(data) < 8:
            return None, data
        # Read the length and break if not enough data
        (data_len,) = struct.unpack_from(">I", data, 4)
        if len(data) < data_len + 8:
            return None, data
        packet = data[8 : data_len + 8]
        data = data[data_len + 8 :]
        return packet, data

class LoRaGoFramerDeframer(FramerDeframer):
    """

    """

    def frame(self, data):
        """
        Frames outgoing data

        :param data: bytes to frame
        :return: array of raw bytes representing a framed packet. Should be ready for uplink.
        """
        print("LoRaGo can not uplink data for now - Data are send through IP")
        df = FpFramerDeframer()
        df.set_constants()
        return df.frame(data)

    def deframe(self, data, no_copy=False):
        """
        Deframes the incoming data from the F prime standard format. Produces exactly one packet, and leftover bytes.
        Users wanting all packets to be deframed should call "deframe_all". If no full packet is available, this method
        returns None. Expects incoming raw bytes to deframe, and returns a deframed packet or None, and the leftover
        bytes that were unused. Will search and discard data up until a start token is found. Note: data will be
        consumed up to the first start token found.

        :param data: framed data bytes
        :param no_copy: (optional) will prevent extra copy if True, but "data" input will be destroyed.
        :return: (packet as array of bytes or None, leftover bytes)
        """
        if not no_copy:
            data = copy.copy(data)

        # Detect frame comming from LoRaGo 
        # LoRaGo frame format: <label> = <value>
        # Existing label: CurrentRSSI, Hex, Message, CRC, ...
        # In our case, we are looking for 'Hex' that corresponds to binary data representing a FPrime frame

        # Example: BEEF\r\nCurrentRSSI=-107\r\nHex=CAFECAFE04537DEADBEEF\r\nCurre
        # frames = [(b'CurrentRSSI=-107\r\n', b'CurrentRSSI', b'-107'), (b'Hex=CAFECAFE04537DEADBEEF\r\n', b'Hex', b'CAFECAFE04537DEADBEEF']
        loRaGoFrames = re.findall(b"((\w*)=(.*?)\r\n)", data, re.DOTALL)

        # print("\tLoRaGoFrames {}".format(len(loRaGoFrames)))

        # Replace found frames to keep only leftover data
        # Result: BEEF\r\nCurre
        for f in loRaGoFrames:
            data = data.replace(f[0], b"", )

        # Remove potential incomplete frame at the beginning
        # Result: Curre 
        data = re.sub(b"^.*?\r\n", b"", data, 1)

        leftOver = b''
        fpFrameFound = False
        fpFrameContent = None

        if len(loRaGoFrames) > 0 :

            df = FpFramerDeframer()
            df.set_constants()

            for f in loRaGoFrames:
                
                # Faking received message
                #f = (b'Hex=DEADBEEF00011247020608AD93200835D00119CAFECAFE', b'Hex', b'DEADBEEF00011247020608AD93200835D00119CAFECAFE')

                # Process 'Hex' frame for LoRaGo with FpFramerDeframer to obtain FP frame
                if f[1] == b'Hex' and fpFrameFound is False:
                    # Deframe packet
                    # Is formated as hex chain but deframe() requires a binary format
                    # b'DEADBEEF' -> b'\xde\xad\xbe\xef'
                    (packet, data) = df.deframe(bytearray.fromhex(f[2].decode()))

                    # df.deframe() returns bytearray data b'\xde\xad\xbe\xef' that have
                    # to be reconverted to binary -> b'DEADBEEF' for next analysis
                    data = binascii.hexlify(data)

                    # If FP frame is found
                    if packet is not None:
                        fpFrameContent = packet
                        fpFrameFound = True

                elif f[1] == b'Hex':    # Append potential other Hex frame from LoRaGo for future processing
                    leftOver += (f[0])
                elif f[1] == b'Device' or f[1] == b'Version':
                    print("[LoRaGo] {}={}".format(f[1].decode(), f[2].decode()))
                    pass
                else:
                    # print("[LoRaGo] Received {}".format(f[0]))
                    pass

        # Append potential imcomplete LoRaGo frame for future processing 
        leftOver += data

        # Return frames and extra leftover data
        return fpFrameContent, leftOver