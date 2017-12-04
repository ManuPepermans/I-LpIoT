# command.py
# author: Christophe VG <contact@christophe.vg>

# unit tests for the D7 ALP command byte generation

import unittest
from d7a.alp.operations.status            import InterfaceStatus
from d7a.alp.parser                       import Parser
from d7a.alp.command                      import Command
from d7a.alp.action                       import Action
from d7a.alp.operations.responses         import ReturnFileData
from d7a.alp.operands.file                import Data, Offset
from d7a.alp.regular_action import RegularAction
from d7a.alp.status_action import StatusAction, StatusActionOperandExtensions
from d7a.d7anp.addressee import Addressee
from d7a.phy.channel_header import ChannelHeader, ChannelBand, ChannelClass, ChannelCoding
from d7a.sp.status                        import Status as D7ASpStatus
from d7a.alp.operands.interface_status    import InterfaceStatusOperand
from d7a.types.ct import CT


class TestCommand(unittest.TestCase):
  def setUp(self):
    self.parser = Parser()

  def test_simple_received_return_file_data_command(self):
    cmd = Command(
        generate_tag_request_action=False,
        actions=[
          RegularAction(
            operation=ReturnFileData(
              operand=Data(
                data=list(bytearray("Hello world")),
                offset=Offset(id=0x51)
              )
            )
          ),
          StatusAction(
            status_operand_extension=StatusActionOperandExtensions.INTERFACE_STATUS,
            operation=InterfaceStatus(
              operand=InterfaceStatusOperand(
                interface_id=0xD7,
                interface_status=D7ASpStatus(
                  channel_header=ChannelHeader(channel_band=ChannelBand.BAND_433,
                                               channel_class=ChannelClass.LO_RATE,
                                               channel_coding=ChannelCoding.PN9),
                  channel_index=16,
                  rx_level=70,
                  link_budget=80,
                  target_rx_level=80,
                  nls=False,
                  missed=False,
                  retry=False,
                  unicast=False,
                  fifo_token=200,
                  seq_nr=0,
                  response_to=CT(mant=20),
                  addressee=Addressee()
                )
              )
            )
          )
        ]
    )
    expected = [
      0x62,                                           # Interface Status action
      0xD7,                                           # D7ASP interface
      32,                                              # channel header
      16, 0,                                          # channel_id
      70,                                             # rxlevel (- dBm)
      80,                                             # link budget
      80,                                             # target rx level
      0,                                              # status
      200,                                            # fifo token
      0,                                              # seq
      20,                                             # response timeout
      0x10,                                           # addressee ctrl (NOID)
      0,                                              # access class
      0x20,                                           # action=32/ReturnFileData
      0x51,                                           # File ID
      0x00,                                           # offset
      0x0b,                                           # length
      0x48, 0x65, 0x6c, 0x6c, 0x6f,                   # Hello
      0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64              # World
    ]
    bytes = bytearray(cmd)
    self.assertEqual(len(bytes), len(expected))
    for i in xrange(len(expected)):
      self.assertEqual(bytes[i], expected[i])


  def test_simple_received_return_file_data_command_with_tag_request(self):
    cmd = Command(
      tag_id=25,
      actions=[
        RegularAction(
          operation=ReturnFileData(
            operand=Data(
              data=list(bytearray("Hello world")),
              offset=Offset(id=0x51)
            )
          )
        ),
        StatusAction(
          status_operand_extension=StatusActionOperandExtensions.INTERFACE_STATUS,
          operation=InterfaceStatus(
            operand=InterfaceStatusOperand(
              interface_id=0xD7,
              interface_status=D7ASpStatus(
                channel_header=ChannelHeader(channel_band=ChannelBand.BAND_433,
                                             channel_class=ChannelClass.LO_RATE,
                                             channel_coding=ChannelCoding.PN9),
                channel_index=16,
                rx_level=70,
                link_budget=80,
                target_rx_level=80,
                nls=False,
                missed=False,
                retry=False,
                unicast=False,
                fifo_token=200,
                seq_nr=0,
                response_to=CT(mant=20),
                addressee=Addressee()
              )
            )
          )
        )
      ]
    )
    expected = [
      0xB4,   # tag request with send response bit set
      25,     # tag ID
      0x62,  # Interface Status action
      0xD7,  # D7ASP interface
      32,  # channel header
      16, 0,  # channel_id
      70,  # rxlevel (- dBm)
      80,  # link budget
      80,  # target rx level
      0,  # status
      200,  # fifo token
      0,  # seq
      20,  # response timeout
      0x10,  # addressee ctrl (NOID)
      0,     # access class
      0x20,  # action=32/ReturnFileData
      0x51,  # File ID
      0x00,  # offset
      0x0b,  # length
      0x48, 0x65, 0x6c, 0x6c, 0x6f,  # Hello
      0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64  # World
    ]
    bytes = bytearray(cmd)
    self.assertEqual(len(bytes), len(expected))
    for i in xrange(len(expected)):
      self.assertEqual(bytes[i], expected[i])


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestCommand)
  unittest.TextTestRunner(verbosity=2).run(suite)
