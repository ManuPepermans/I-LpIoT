# parser
# author: Christophe VG <contact@christophe.vg>

# a parser for ALP commands
import struct

from d7a.alp.command              import Command
from d7a.alp.forward_action import ForwardAction
from d7a.alp.interface import InterfaceType
from d7a.alp.operands.interface_configuration import InterfaceConfiguration
from d7a.alp.operands.interface_status import InterfaceStatusOperand
from d7a.alp.operations.forward import Forward
from d7a.alp.operations.status import InterfaceStatus
from d7a.alp.operations.tag_response import TagResponse
from d7a.alp.operations.write_operations import WriteFileData
from d7a.alp.status_action import StatusAction, StatusActionOperandExtensions
from d7a.alp.regular_action import RegularAction
from d7a.alp.operations.responses import ReturnFileData
from d7a.alp.operations.requests  import ReadFileData
from d7a.alp.operands.file        import Offset, Data, DataRequest
from d7a.alp.tag_response_action import TagResponseAction
from d7a.parse_error              import ParseError
from d7a.sp.configuration import Configuration
from d7a.sp.status import Status
from d7a.d7anp.addressee import Addressee
from d7a.types.ct import CT
from d7a.alp.operands.tag_id import TagId
from d7a.alp.operations.tag_request import TagRequest
from d7a.alp.tag_request_action import TagRequestAction
from d7a.phy.channel_header import ChannelHeader


class Parser(object):

  def parse(self, s, cmd_length):
    actions = []
    if cmd_length != 0:
      alp_bytes_parsed = 0
      while alp_bytes_parsed < cmd_length:
        startpos = s.bytepos
        action = self.parse_alp_action(s)
        actions.append(action)
        alp_bytes_parsed = alp_bytes_parsed + (s.bytepos - startpos)

    cmd = Command(actions = actions, generate_tag_request_action=True)
    return cmd

  def parse_alp_action(self, s):
    # meaning of first 2 bits depend on action opcode
    b7 = s.read("bool")
    b6 = s.read("bool")
    op = s.read("uint:6")
    try:
      return{
        1  :  self.parse_alp_read_file_data_action,
        4  :  self.parse_alp_write_file_data_action,
        32 :  self.parse_alp_return_file_data_action,
        34 :  self.parse_alp_return_status_action,
        35 :  self.parse_tag_response_action,
        50 :  self.parse_forward_action,
        52 :  self.parse_tag_request_action
      }[op](b7, b6, s)
    except KeyError:
      raise ParseError("alp_action " + str(op) + " is not implemented")

  def parse_alp_read_file_data_action(self, b7, b6, s):
    operand = self.parse_alp_file_data_request_operand(s)
    return RegularAction(group=b7,
                  resp=b6,
                  operation=ReadFileData(operand=operand))

  def parse_alp_write_file_data_action(self, b7, b6, s):
    operand = self.parse_alp_return_file_data_operand(s)
    return RegularAction(group=b7,
                  resp=b6,
                  operation=WriteFileData(operand=operand))

  def parse_alp_file_data_request_operand(self, s):
    offset = self.parse_offset(s)
    length = s.read("uint:8")
    return DataRequest(length=length, offset=offset)

  def parse_alp_return_file_data_action(self, b7, b6, s):
    operand = self.parse_alp_return_file_data_operand(s)
    return RegularAction(group=b7,
                        resp=b6,
                        operation=ReturnFileData(operand=operand))

  def parse_alp_return_file_data_operand(self, s):
    offset = self.parse_offset(s)
    length = s.read("uint:8") # TODO assuming 1 bute for now but can be 4 bytes
    data   = s.read("bytes:" + str(length))
    return Data(offset=offset, data=map(ord,data))

  def parse_alp_return_status_action(self, b7, b6, s):
    if b7:
      raise ParseError("Status Operand extension 2 and 3 is RFU")

    if b6: # interface status
      interface_id = s.read("uint:8")
      try:
        interface_status_operation = {
          0x00 :  self.parse_alp_interface_status_host,
          0xd7 :  self.parse_alp_interface_status_d7asp,
        }[interface_id](s)
        return StatusAction(operation=interface_status_operation,
                            status_operand_extension=StatusActionOperandExtensions.INTERFACE_STATUS)
      except KeyError:
        raise ParseError("Received ALP Interface status for interface " + str(interface_id) + " which is not implemented")
    else: # action status
      pass # TODO

  def parse_tag_request_action(self, b7, b6, s):
    if b6:
      raise ParseError("bit 6 is RFU")

    tag_id = s.read("uint:8")
    return TagRequestAction(respond_when_completed=b7, operation=TagRequest(operand=TagId(tag_id=tag_id)))

  def parse_tag_response_action(self, b7, b6, s):
    tag_id = s.read("uint:8")
    return TagResponseAction(eop=b7, error=b6, operation=TagResponse(operand=TagId(tag_id=tag_id)))

  def parse_forward_action(self, b7, b6, s):
    if b7:
      raise ParseError("bit 7 is RFU")

    interface_id = InterfaceType(int(s.read("uint:8")))
    assert(interface_id == InterfaceType.D7ASP)
    interface_config = Configuration.parse(s)
    return ForwardAction(resp=b6, operation=Forward(operand=InterfaceConfiguration(interface_id=interface_id,
                                                                                   interface_configuration=interface_config)))

  def parse_alp_interface_status_host(self, s):
    pass # no interface status defined for host interface

  def parse_alp_interface_status_d7asp(self, s):
    status = Status.parse(s)

    return InterfaceStatus(
      operand=InterfaceStatusOperand(interface_id=0xd7, interface_status=status)
    )

  def parse_offset(self, s):
    id     = s.read("uint:8")
    size   = s.read("uint:2") # + 1 = already read

    offset = s.read("uint:" + str(6+(size * 8)))
    return Offset(id=id, size=size+1, offset=offset)
