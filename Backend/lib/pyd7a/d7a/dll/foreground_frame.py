import pprint

from d7a.d7anp.addressee import IdType
from d7a.support.schema           import Validatable, Types
from d7a.dll.foreground_frame_control import ForegroundFrameControl
from d7a.d7anp.frame import Frame as D7anpFrame

from PyCRC.CRCCCITT import CRCCCITT

class ForegroundFrame(Validatable):

  SCHEMA = [{
    "length": Types.BYTE(),
    "subnet": Types.BYTE(),
    "control": Types.OBJECT(ForegroundFrameControl),
    "target_address": Types.BYTES(), # TODO max size?
    "d7anp_frame": Types.OBJECT(D7anpFrame), # TODO assuming foreground frames for now
    "crc16"  : Types.BITS(16) # TODO does not work, look into this later {'validator': validate_crc }
  }]

  def __init__(self, length, subnet, control, target_address, d7anp_frame, crc16):
    self.length = length
    self.subnet = subnet
    self.control = control
    self.target_address = target_address
    self.d7anp_frame = d7anp_frame
    self.crc16 = crc16
    # TODO validate CRC

    super(ForegroundFrame, self).__init__()

  # def validate_crc(self, value, error):
  #   raw_data = []
  #   raw_data.append(self.length)
  #   raw_data.append(self.subnet)
  #   raw_data.append(self.control)
  #   raw_data.append(self.target_address)
  #   raw_data.append(self.payload)
  #   crc = CRCCCITT().calculate(raw_data)


  @staticmethod
  def parse(s):
    length = s.read("int:8")
    subnet = s.read("int:8")
    control = ForegroundFrameControl.parse(s)
    payload_length = length - 4 # substract subnet, control, crc
    if control.id_type == IdType.VID:
      target_address = map(ord, s.read("bytes:2"))
      payload_length = payload_length - 2
    elif control.id_type == IdType.UID:
        target_address = map(ord, s.read("bytes:8"))
        payload_length = payload_length - 8
    else:
      target_address = []

    return ForegroundFrame(
      length=length,
      subnet=subnet,
      control=control,
      target_address=target_address,
      d7anp_frame=D7anpFrame.parse(s, payload_length),
      crc16=s.read("uint:16")
    )


  def __iter__(self):
    yield self.length
    yield self.subnet
    for byte in self.control: yield byte
    for byte in self.target_address: yield byte
    for byte in self.d7anp_frame: yield byte
    yield self.crc16

  def __str__(self):
    return pprint.PrettyPrinter().pformat(self.as_dict())
