import unittest

from bitstring import ConstBitStream

from d7a.d7atp.control import Control


class TestControl(unittest.TestCase):
  def test_parsing(self):
    ctrl = Control.parse(ConstBitStream(bytes=[0x93]))
    self.assertEqual(ctrl.is_dialog_start, True)
    self.assertEqual(ctrl.has_tl, False)
    self.assertEqual(ctrl.has_te, True)
    self.assertEqual(ctrl.is_ack_requested, False)
    self.assertEqual(ctrl.is_ack_not_void, False)
    self.assertEqual(ctrl.is_ack_record_requested, True)
    self.assertEqual(ctrl.has_agc, True)

  def test_byte_generation(self):
    ctrl = Control(
      is_dialog_start=True,
      has_tl=True,
      has_te=False,
      is_ack_requested=True,
      is_ack_not_void=True,
      is_ack_record_requested=False,
      has_agc=False
    )

    data = bytearray(ctrl)
    self.assertEqual(len(data), 1)
    self.assertEqual(data[0], 0xAC)
