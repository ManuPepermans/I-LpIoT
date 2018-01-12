# file.py
# author: Christophe VG <contact@christophe.vg>

# unit tests for the D7 File {*} Operands

import unittest

from d7a.alp.operands.file import Offset, Data

class TestOffset(unittest.TestCase):
  def test_default_offset_constructor(self):
    Offset()

  def test_offset_valid_boundaries(self):
    Offset(id=0x00)             and Offset(id=0xFF)
    Offset(size=1)              and Offset(size=4)
    Offset(offset=0x00)         and Offset(offset=0xFF)
    Offset(size=2, offset=0x00) and Offset(size=2, offset=0xFFFF)
    Offset(size=3, offset=0x00) and Offset(size=3, offset=0xFFFFFF)
    Offset(size=4, offset=0x00) and Offset(size=4, offset=0xFFFFFFFF)

  def test_offset_invalid_boundaries(self):
    def bad(args, kwargs): Offset(**kwargs)
    self.assertRaises(ValueError, bad, [], {"id"    : -1                      })
    self.assertRaises(ValueError, bad, [], {"id"    : 0x1FF                   })
    self.assertRaises(ValueError, bad, [], {"size"  : 0                       })
    self.assertRaises(ValueError, bad, [], {"size"  : 5                       })
    self.assertRaises(ValueError, bad, [], {"offset": -1                      })
    self.assertRaises(ValueError, bad, [], {"offset": 0x1FF                   })
    self.assertRaises(ValueError, bad, [], {"size"  : 2, "offset": -1         })
    self.assertRaises(ValueError, bad, [], {"size"  : 2, "offset": 0x1FFFF    })
    self.assertRaises(ValueError, bad, [], {"size"  : 3, "offset": -1         })
    self.assertRaises(ValueError, bad, [], {"size"  : 3, "offset": 0x1FFFFFF  })
    self.assertRaises(ValueError, bad, [], {"size"  : 4, "offset": -1         })
    self.assertRaises(ValueError, bad, [], {"size"  : 4, "offset": 0x1FFFFFFFF})

  def test_byte_generation(self):
    bytes = bytearray(Offset())
    self.assertEqual(len(bytes), 2)
    self.assertEqual(bytes[0], int('00000000', 2))
    self.assertEqual(bytes[1], int('00000000', 2))

    bytes = bytearray(Offset(id=0xFF))
    self.assertEqual(len(bytes), 2)
    self.assertEqual(bytes[0], int('11111111', 2))
    self.assertEqual(bytes[1], int('00000000', 2))

    bytes = bytearray(Offset(size=4))
    self.assertEqual(len(bytes), 5)
    self.assertEqual(bytes[0], int('00000000', 2))
    self.assertEqual(bytes[1], int('11000000', 2))
    self.assertEqual(bytes[2], int('00000000', 2))
    self.assertEqual(bytes[3], int('00000000', 2))
    self.assertEqual(bytes[4], int('00000000', 2))

class TestData(unittest.TestCase):
  def test_default_data_constructor(self):
    Data()

  def test_data_bad_offset(self):
    def bad(): Data(Data())
    self.assertRaises(ValueError, bad)

  def test_data_length(self):
    d = Data(data=[0xd7, 0x04, 0x00])
    self.assertEqual(d.length, 3)
    self.assertEqual(len(d),   3)

  def test_byte_generation(self):
    bytes = bytearray(Data())
    self.assertEqual(len(bytes), 3)
    self.assertEqual(bytes[0], int('00000000', 2)) # offset
    self.assertEqual(bytes[1], int('00000000', 2)) # offset
    self.assertEqual(bytes[2], int('00000000', 2))

    bytes = bytearray(Data(data=[0x01,0x02,0x03,0x04]))
    self.assertEqual(len(bytes), 7)
    self.assertEqual(bytes[0], int('00000000', 2)) # offset
    self.assertEqual(bytes[1], int('00000000', 2)) # offset
    self.assertEqual(bytes[2], int('00000100', 2)) # length = 4
    self.assertEqual(bytes[3], int('00000001', 2))
    self.assertEqual(bytes[4], int('00000010', 2))
    self.assertEqual(bytes[5], int('00000011', 2))
    self.assertEqual(bytes[6], int('00000100', 2))
    
if __name__ == '__main__':
  for case in [TestOffset, TestData]:
    suite = unittest.TestLoader().loadTestsFromTestCase(case)
    unittest.TextTestRunner(verbosity=2).run(suite)
