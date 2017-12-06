import struct

from d7a.phy.channel_header import ChannelHeader
from d7a.support.schema import Validatable, Types


class SubBand(Validatable):
  # TODO update to D7AP v1.1

  SCHEMA = [{
    "channel_index_start": Types.INTEGER(min=0, max=0xFFFF),
    "channel_index_end": Types.INTEGER(min=0, max=0xFFFF),
    "eirp": Types.INTEGER(min=-128, max=127),
    "cca": Types.INTEGER(min=0, max=255),
    "duty": Types.INTEGER(min=0, max=255),
  }]

  def __init__(self, channel_index_start=0, channel_index_end=0, eirp=0, cca=86, duty=255):
    self.channel_index_start = channel_index_start
    self.channel_index_end = channel_index_end
    self.eirp = eirp
    self.cca = cca
    self.duty = duty
    super(SubBand, self).__init__()

  def __iter__(self):
    for byte in bytearray(struct.pack("<h", self.channel_index_start)): yield byte
    for byte in bytearray(struct.pack("<h", self.channel_index_end)): yield byte
    yield self.eirp
    yield self.cca
    yield self.duty

  @staticmethod
  def parse(s):
    channel_index_start = struct.unpack("<h", s.read("bytes:2"))[0]
    channel_index_end = struct.unpack("<h", s.read("bytes:2"))[0]
    eirp = s.read("uint:8")
    cca = s.read("uint:8")
    duty = s.read("uint:8")

    return SubBand(channel_index_start=channel_index_start,
                   channel_index_end=channel_index_end,
                   eirp=eirp,
                   cca=cca,
                   duty=duty)

  def __str__(self):
    return "channel_index_start={}, channel_index_end={}, eirp={}, cca={}, duty={}".format(
      self.channel_index_start,
      self.channel_index_end,
      self.eirp,
      self.cca,
      self.duty
    )