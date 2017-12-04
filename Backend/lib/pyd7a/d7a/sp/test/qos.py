# qos.py
# author: Christophe VG <contact@christophe.vg>

# unit tests for the D7A SP QoS Paramters 

import unittest

from bitstring import ConstBitStream

from d7a.sp.qos import QoS, ResponseMode, RetryMode


class TestQoS(unittest.TestCase):
  def test_default_constructor(self):
    qos = QoS()

  def test_byte_generation(self):
    bytes = bytearray(QoS())
    self.assertEqual(len(bytes), 1)
    self.assertEqual(bytes[0], int('00000000', 2))

    bytes = bytearray(QoS(
      retry_mod = RetryMode.RETRY_MODE_NO,
      resp_mod    = ResponseMode.RESP_MODE_ANY,
      record=True,
      stop_on_err=True,
    ))
    self.assertEqual(len(bytes), 1)
    self.assertEqual(bytes[0], int('11000010', 2))

  def test_parse(self):
    bytes = [
      0b11000010
    ]

    qos = QoS.parse(ConstBitStream(bytes=bytes))

    self.assertEqual(qos.retry_mod, RetryMode.RETRY_MODE_NO)
    self.assertEqual(qos.resp_mod, ResponseMode.RESP_MODE_ANY)
    self.assertEqual(qos.record, True)
    self.assertEqual(qos.stop_on_err, True)

if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestQoS)
  unittest.TextTestRunner(verbosity=2).run(suite)
