import unittest

from bitstring import ConstBitStream

from d7a.phy.channel_header import ChannelHeader, ChannelCoding, ChannelClass, ChannelBand
from d7a.system_files.access_profile import AccessProfileFile


class TestAccessProfileFile(unittest.TestCase):

  def test_default_constructor(self):
    f = AccessProfileFile()
    self.assertEqual(f.access_specifier, 0)
    self.assertEqual(f.access_profile, None)

  def test_invalid_access_specifier(self):
     def bad(): AccessProfileFile(access_specifier=21) # can be max 14
     self.assertRaises(ValueError, bad)


  # parsing and byte generation is tested in AccessProfile unit tests