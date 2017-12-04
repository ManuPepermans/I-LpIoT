from enum import Enum

from d7a.support.schema import Validatable, Types


class ChannelCoding(Enum):
  PN9 = 0x00
  FEC_PN9 = 0x02


class ChannelClass(Enum):
  LO_RATE = 0x00
  LORA = 0x01 # TODO not part of spec
  NORMAL_RATE = 0x02
  HI_RATE = 0x03

class ChannelBand(Enum):
  BAND_433 = 0x02
  BAND_868 = 0x03
  BAND_915 = 0x04

class ChannelHeader(Validatable):
  # TODO
  SCHEMA = [{
    "channel_coding": Types.ENUM(ChannelCoding),
    "channel_class": Types.ENUM(ChannelClass),
    "channel_band": Types.ENUM(ChannelBand)
  }]

  def __init__(self, channel_coding, channel_class, channel_band):
    self.channel_coding = channel_coding
    self.channel_class = channel_class
    self.channel_band = channel_band
    super(ChannelHeader, self).__init__()

  def __iter__(self):
    byte = self.channel_band.value << 4
    byte += self.channel_class.value << 2
    byte += self.channel_coding.value
    yield byte

  @staticmethod
  def parse(s):
    s.read("uint:1") # RFU
    channel_band = ChannelBand(s.read("uint:3"))
    channel_class = ChannelClass(s.read("uint:2"))
    channel_coding = ChannelCoding(s.read("uint:2"))
    return ChannelHeader(channel_coding=channel_coding, channel_class=channel_class, channel_band=channel_band)

  def __str__(self):
    return "coding={}, class={}, band={}".format(
      self.channel_coding,
      self.channel_class,
      self.channel_band
    )

  def __eq__(self, other):
    if type(other) is type(self):
      return self.__dict__ == other.__dict__
    return False

  def __ne__(self, other):
    if isinstance(other, self.__class__):
      return not self.__eq__(other)
    return False