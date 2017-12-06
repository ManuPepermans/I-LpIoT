import unittest

from bitstring import ConstBitStream

from d7a.dll.access_profile import AccessProfile, CsmaCaMode, SubBand
from d7a.dll.sub_profile import SubProfile
from d7a.phy.channel_header import ChannelHeader, ChannelBand, ChannelCoding, ChannelClass
from d7a.types.ct import CT


class TestAccessProfile(unittest.TestCase):
  valid_channel_header = ChannelHeader(
    channel_class=ChannelClass.NORMAL_RATE,
    channel_coding=ChannelCoding.PN9,
    channel_band=ChannelBand.BAND_433
  )

  valid_sub_bands = [
    SubBand(),
    SubBand(),
    SubBand(),
    SubBand(),
    SubBand(),
    SubBand(),
    SubBand(),
    SubBand()
  ]

  valid_sub_profiles = [
    SubProfile(),
    SubProfile(),
    SubProfile(),
    SubProfile()
  ]

  def test_validation_ok(self):
    ap = AccessProfile(channel_header=self.valid_channel_header,
                       sub_profiles=self.valid_sub_profiles,
                       sub_bands=self.valid_sub_bands)


  def test_validation_sub_profiles(self):
    def bad():
      ap = AccessProfile(channel_header=self.valid_channel_header,
                         sub_profiles=[],
                         sub_bands=self.valid_sub_bands)

    self.assertRaises(ValueError, bad)

  def test_validation_sub_profiles_count(self):
    def bad():
      sub_profiles = [SubProfile() for _ in range(10)] # too many ...

      ap = AccessProfile(channel_header=self.valid_channel_header,
                         sub_profiles=sub_profiles,
                         sub_bands=self.valid_sub_bands)

    self.assertRaises(ValueError, bad)

  def test_validation_sub_bands_type(self):
    def bad():
      ap = AccessProfile(channel_header=self.valid_channel_header,
                         sub_profiles=self.valid_sub_profiles,
                         sub_bands=[None])

    self.assertRaises(ValueError, bad)

  def test_validation_sub_bands_count(self):
    def bad():
      sub_bands = [SubBand() for _ in range(10)] # too many ...

      ap = AccessProfile(channel_header=self.valid_channel_header,
                         sub_profiles=self.valid_sub_profiles,
                         sub_bands=sub_bands)

    self.assertRaises(ValueError, bad)

  def test_byte_generation(self):
    expected = [
      0b00101000,  # channel header
    ]

    for _ in xrange(AccessProfile.NUMBER_OF_SUB_PROFILES):
      expected.extend(list(bytearray(SubProfile())))

    expected.extend(list(bytearray(SubBand()))) # only one sub_band

    ap = AccessProfile(channel_header=self.valid_channel_header,
                       sub_bands=[SubBand()],
                       sub_profiles=self.valid_sub_profiles)

    bytes = bytearray(ap)
    for i in xrange(len(bytes)):
      self.assertEqual(expected[i], bytes[i])

    self.assertEqual(len(expected), len(bytes))

  def test_parse(self):
    bytes = list(bytearray(self.valid_channel_header))

    for _ in xrange(AccessProfile.NUMBER_OF_SUB_PROFILES):
      bytes.extend(list(bytearray(SubProfile())))

    for _ in range(AccessProfile.MAX_NUMBER_OF_SUB_BANDS):
      bytes.extend(list(bytearray(SubBand())))

    ap = AccessProfile.parse(ConstBitStream(bytes=bytes))
    self.assertEqual(ap.channel_header.channel_band, self.valid_channel_header.channel_band)
    self.assertEqual(ap.channel_header.channel_coding, self.valid_channel_header.channel_coding)
    self.assertEqual(ap.channel_header.channel_class, self.valid_channel_header.channel_class)
    self.assertEqual(len(ap.sub_bands), AccessProfile.MAX_NUMBER_OF_SUB_BANDS)
    for sb in ap.sub_bands:
      self.assertEqual(sb.channel_index_start, SubBand().channel_index_start)
      self.assertEqual(sb.channel_index_end, SubBand().channel_index_end)
      self.assertEqual(sb.cca, SubBand().cca)
      self.assertEqual(sb.duty, SubBand().duty)
      self.assertEqual(sb.eirp, SubBand().eirp)

    for sp in ap.sub_profiles:
      self.assertEqual(sp.subband_bitmap, SubProfile().subband_bitmap)
      self.assertEqual(sp.scan_automation_period.exp, SubProfile().scan_automation_period.exp)
      self.assertEqual(sp.scan_automation_period.mant, SubProfile().scan_automation_period.mant)

    self.assertEqual(len(ap.sub_profiles), AccessProfile.NUMBER_OF_SUB_PROFILES)

