import unittest
import binascii
from PyCRC.CRCCCITT import CRCCCITT
from d7a.alp.operands.file import DataRequest, Data
from d7a.alp.operations.requests import ReadFileData
from d7a.alp.operations.responses import ReturnFileData
from d7a.d7anp.addressee import IdType, NlsMethod

from d7a.dll.parser import Parser, FrameType
from d7a.support.Crc import calculate_crc
from d7a.types.ct import CT


class TestForegroundFrameParser(unittest.TestCase):
  def setUp(self):
    self.parser = Parser(FrameType.FOREGROUND)

  def test_read_id_command_frame(self):
    read_id_command = [
      0x15, # length
      0x00, # subnet
      0x6a, # DLL control
      0x20, # D7ANP control
      0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, # Origin Access ID
      0xa8, # D7ATP control
      0xe9, # dialog ID
      0x00, # transaction ID
      0x05, # Tl
      0x05, # Tc
      0x01, # ALP control (read file data operation)
      0x00, 0x00, 0x08, # file data request operand (file ID 0x00)
      0x25, 0xDA # CRC
    ]

    (frames, info) = self.parser.parse(read_id_command)
    self.assertEqual(len(frames), 1)
    frame = frames[0]
    self.assertEqual(frame.length, 21)
    self.assertEqual(frame.subnet, 0)
    self.assertEqual(frame.control.id_type, IdType.NOID)
    self.assertEqual(frame.control.eirp_index, 42)
    self.assertEqual(len(frame.target_address), 0)
    self.assertEqual(frame.d7anp_frame.control.nls_method, NlsMethod.NONE)
    self.assertFalse(frame.d7anp_frame.control.has_hopping)
    self.assertFalse(frame.d7anp_frame.control.has_no_origin_access_id)
    self.assertEqual(frame.d7anp_frame.control.origin_id_type, IdType.UID)
    self.assertEqual(frame.d7anp_frame.origin_access_class, 0x01)
    self.assertEqual(frame.d7anp_frame.origin_access_id, [0, 0, 0, 0, 0, 0, 0, 1])
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.is_dialog_start)
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.has_tl)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.has_te)
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.is_ack_requested)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.is_ack_not_void)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.is_ack_record_requested)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.has_agc)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.dialog_id, 0xe9)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.transaction_id, 0)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tl.exp, CT(exp=0, mant=5).exp)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tl.mant, CT(exp=0, mant=5).mant)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tc.exp, CT(exp=0, mant=5).exp)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tc.mant, CT(exp=0, mant=5).mant)
    self.assertEqual(len(frame.d7anp_frame.d7atp_frame.alp_command.actions), 1)
    alp_action = frame.d7anp_frame.d7atp_frame.alp_command.actions[0]
    self.assertEqual(type(alp_action.operation), ReadFileData)
    self.assertEqual(type(alp_action.operand), DataRequest)
    self.assertEqual(alp_action.operand.offset.id, 0)
    self.assertEqual(alp_action.operand.offset.offset, 0)
    self.assertEqual(alp_action.operand.length, 8)
    # TODO self.assertEqual(len(frame.payload), 16)
    hexstring = binascii.hexlify(bytearray(read_id_command[:-2])).decode('hex') # TODO there must be an easier way...
    self.assertEqual(frame.crc16, CRCCCITT(version='FFFF').calculate(hexstring))

  # TODO tmp
  def test_read_id_response_frame(self):
    frame_data = [ 0x25,  # length
                   0x00,  # subnet
                   0x80,  # dll control
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, # target_address
                   0x20,  # D7ANP control
                   0x01, # origin access class
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, # origin access ID
                   0xa8,  # D7ATP control
                   0xe9,  # dialog ID
                   0x00,  # transaction ID
                   0x05,  # Tl
                   0x05,  # Tc
                   0x20,  # ALP control (return file data operation)
                   0x00, 0x00, 0x08, # file data operand
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, # UID
                   ]

    frame_data = frame_data + calculate_crc(frame_data)

    (frames, info) = self.parser.parse(frame_data)
    self.assertEqual(len(frames), 1)
    frame = frames[0]
    self.assertEqual(frame.length, 37)
    self.assertEqual(frame.subnet, 0)
    self.assertEqual(frame.control.id_type, IdType.UID)
    self.assertEqual(frame.control.eirp_index, 0)
    self.assertEqual(len(frame.target_address), 8)
    self.assertEqual(frame.target_address, [0, 0, 0, 0, 0, 0, 0, 1])
    self.assertEqual(frame.d7anp_frame.control.nls_method, NlsMethod.NONE)
    self.assertFalse(frame.d7anp_frame.control.has_hopping)
    self.assertFalse(frame.d7anp_frame.control.has_no_origin_access_id)
    self.assertEqual(frame.d7anp_frame.control.origin_id_type, IdType.UID)
    self.assertEqual(frame.d7anp_frame.origin_access_class, 0x01)
    self.assertEqual(frame.d7anp_frame.origin_access_id, [0, 0, 0, 0, 0, 0, 0, 2])
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.is_dialog_start)
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.has_tl)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.has_te)
    self.assertTrue(frame.d7anp_frame.d7atp_frame.control.is_ack_requested)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.is_ack_not_void)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.is_ack_record_requested)
    self.assertFalse(frame.d7anp_frame.d7atp_frame.control.has_agc)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.dialog_id, 0xe9)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.transaction_id, 0)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tl.exp, CT(exp=0, mant=5).exp)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tl.mant, CT(exp=0, mant=5).mant)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tc.exp, CT(exp=0, mant=5).exp)
    self.assertEqual(frame.d7anp_frame.d7atp_frame.tc.mant, CT(exp=0, mant=5).mant)
    self.assertEqual(len(frame.d7anp_frame.d7atp_frame.alp_command.actions), 1)
    alp_action = frame.d7anp_frame.d7atp_frame.alp_command.actions[0]
    self.assertEqual(type(alp_action.operation), ReturnFileData)
    self.assertEqual(type(alp_action.operand), Data)
    self.assertEqual(alp_action.operand.offset.id, 0)
    self.assertEqual(alp_action.operand.offset.offset, 0)
    self.assertEqual(alp_action.operand.length, 8)

class TestBackgroundFrameParser(unittest.TestCase):
  def setUp(self):
    self.parser = Parser(FrameType.BACKGROUND)

  def test_background_frame(self):
    frame = [
      0x01, # subnet
      0x80, # control
      0x00, # payload
      0x01, # payload
      0x25, 0xDA # CRC
    ]

    (frames, info) = self.parser.parse(frame)
    self.assertEqual(len(frames), 1)
    self.assertEqual(frames[0].control.id_type, IdType.UID)
    self.assertEqual(frames[0].control.tag, 0)
    self.assertEqual(frames[0].payload, 1)
    self.assertEqual(frames[0].crc16, 9690)