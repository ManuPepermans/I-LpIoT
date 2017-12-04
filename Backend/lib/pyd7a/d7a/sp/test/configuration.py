# configuration.py
# author: Christophe VG <contact@christophe.vg>

# unit tests for the D7A SP (FIFO) Configuration

import unittest

from bitstring import ConstBitStream

from d7a.d7anp.addressee import IdType, NlsMethod
from d7a.sp.session import States

from d7a.types.ct         import CT
from d7a.sp.qos           import QoS, ResponseMode, RetryMode
from d7a.sp.configuration import Configuration

class TestConfiguration(unittest.TestCase):
  def test_default_constructor(self):
    c = Configuration()

  def test_invalid_configuration_construction(self):
    def bad(args, kwargs): Configuration(**kwargs)
    self.assertRaises(ValueError, bad, [], { "qos"       : None  })
    self.assertRaises(ValueError, bad, [], { "addressee" : None  })
    self.assertRaises(ValueError, bad, [], { "dorm_to"   : None  })

  def test_configuration_bad_composed_objects(self):
    def bad(args, kwargs): Configuration(**kwargs)
    self.assertRaises(ValueError, bad, [], { "qos":       CT()  })
    self.assertRaises(ValueError, bad, [], { "dorm_to":   QoS() })
    self.assertRaises(ValueError, bad, [], { "addressee": QoS() })

  def test_byte_generation(self):
    # TODO: use mocking framework to mock sub-objects
    bytes = bytearray(Configuration())
    self.assertEqual(len(bytes), 4)
    self.assertEquals(bytes[0], int( '00000000', 2)) # qos
    self.assertEquals(bytes[1], int( '00000000', 2)) # dorm_to (CT)
    self.assertEquals(bytes[2], int( '00010000', 2)) # addressee control NOID
    self.assertEquals(bytes[3], 0)  # access class

  def test_parse(self):
    bytes = [
      0b00000000,
      0,
      0b00010000,
      0
    ]

    config = Configuration.parse(ConstBitStream(bytes=bytes))

    self.assertEqual(config.qos.resp_mod, ResponseMode.RESP_MODE_NO)
    self.assertEqual(config.qos.retry_mod, RetryMode.RETRY_MODE_NO)
    self.assertEqual(config.qos.stop_on_err, False)
    self.assertEqual(config.qos.record, False)
    self.assertEqual(config.addressee.id_type, IdType.NOID)
    self.assertEqual(config.addressee.nls_method, NlsMethod.NONE)
    self.assertEqual(config.addressee.access_class, 0)

if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestConfiguration)
  unittest.TextTestRunner(verbosity=1).run(suite)
