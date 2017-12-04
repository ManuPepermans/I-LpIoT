from enum import Enum

from d7a.dll.sub_profile import SubProfile
from d7a.phy.channel_header import ChannelHeader
from d7a.phy.subband import SubBand
from d7a.support.schema           import Validatable, Types
from d7a.types.ct import CT


class CsmaCaMode(Enum):
  UNC = 0
  AIND = 1
  RAIND = 2
  RIGD = 3


class AccessProfile(Validatable):
  NUMBER_OF_SUB_PROFILES = 4
  MAX_NUMBER_OF_SUB_BANDS = 8

  # TODO update to D7AP v1.1
  SCHEMA = [{
    "channel_header": Types.OBJECT(ChannelHeader),
    "sub_profiles": Types.LIST(SubProfile, minlength=4, maxlength=4),
    "sub_bands": Types.LIST(SubBand, minlength=0, maxlength=8)
  }]

  def __init__(self, channel_header, sub_profiles, sub_bands):
    self.channel_header = channel_header
    self.sub_profiles = sub_profiles
    self.sub_bands = sub_bands
    super(AccessProfile, self).__init__()

  @staticmethod
  def parse(s):
    channel_header = ChannelHeader.parse(s)
    sub_profiles = []
    for _ in range(AccessProfile.NUMBER_OF_SUB_PROFILES):
      sub_profiles.append(SubProfile.parse(s))

    sub_bands = []
    for _ in range(AccessProfile.MAX_NUMBER_OF_SUB_BANDS):
      sub_bands.append(SubBand.parse(s))

    return AccessProfile(channel_header=channel_header,
                         sub_bands=sub_bands,
                         sub_profiles=sub_profiles
                         )

  def __iter__(self):
    for byte in self.channel_header: yield byte
    for sp in self.sub_profiles:
      for byte in sp: yield byte

    for sb in self.sub_bands:
      for byte in sb: yield byte

  def __str__(self):
    subprofiles_string = ""
    for subprofile in self.sub_profiles:
      subprofiles_string = subprofiles_string + str(subprofile)

    subbands_string = ""
    for subband in self.sub_bands:
      subbands_string = subbands_string + str(subband)

    return "channel_header={}, sub_profiles={}, sub_bands={}".format(
      self.channel_header,
      subprofiles_string,
      subbands_string
    )