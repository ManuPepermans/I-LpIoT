import unittest

from bitstring import ConstBitStream

from d7a.dll.sub_profile import SubProfile

from d7a.types.ct import CT


class TestSubProfile(unittest.TestCase):

  def test_default_constructor(self):
    sp = SubProfile()
    self.assertEqual(sp.subband_bitmap, 0x00)
    ct = CT()
    self.assertEqual(sp.scan_automation_period.mant, ct.mant)
    self.assertEqual(sp.scan_automation_period.exp, ct.exp)

  def test_byte_generation(self):
    expected = [
      0b10000001, # subband bitmap
      0, # scan automation period
    ]

    sp = SubProfile(subband_bitmap=0b10000001, scan_automation_period=CT(0))
    bytes = bytearray(sp)
    for i in xrange(len(bytes)):
      self.assertEqual(expected[i], bytes[i])

    self.assertEqual(len(expected), len(bytes))

  def test_parse(self):
    bytes = [
       0b10000001, # subband bitmap
       0,  # scan automation period
     ]

    sp = SubProfile.parse(ConstBitStream(bytes=bytes))
    self.assertEqual(sp.subband_bitmap, 0b10000001)
    self.assertEqual(sp.scan_automation_period.mant, CT(0).mant)
    self.assertEqual(sp.scan_automation_period.exp, CT(0).exp)
