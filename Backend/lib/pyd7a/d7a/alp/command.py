# command
# author: Christophe VG <contact@christophe.vg>

# class implementation of ALP commands

# a D7A ALP Command consists of 1 or more ALP Actions
import random

from d7a.alp.action import Action
from d7a.alp.interface import InterfaceType
from d7a.alp.operands.file import Offset, DataRequest, Data
from d7a.alp.operands.interface_configuration import InterfaceConfiguration
from d7a.alp.operands.tag_id import TagId
from d7a.alp.operations.forward import Forward
from d7a.alp.operations.requests import ReadFileData
from d7a.alp.operations.responses import ReturnFileData
from d7a.alp.operations.tag_request import TagRequest
from d7a.alp.operations.write_operations import WriteFileData
from d7a.alp.status_action import StatusAction, StatusActionOperandExtensions
from d7a.alp.tag_response_action import TagResponseAction
from d7a.parse_error import ParseError
from d7a.sp.configuration import Configuration

from d7a.support.schema           import Validatable, Types
from d7a.alp.regular_action import RegularAction
from d7a.alp.tag_request_action import TagRequestAction


class Command(Validatable):
  
  SCHEMA = [{
    "actions": Types.LIST(Action),
    "interface_status": Types.OBJECT(StatusAction, nullable=True) # can be null for example when parsing DLL frames
  }]

  def __init__(self, actions=[], generate_tag_request_action=True, tag_id=None, send_tag_response_when_completed=True):
    self.actions = []
    self.interface_status = None
    self.generate_tag_request_action = generate_tag_request_action
    self.tag_id = tag_id
    self.send_tag_response_when_completed = send_tag_response_when_completed
    self.execution_completed = False

    for action in actions:
      if type(action) == StatusAction and action.status_operand_extension == StatusActionOperandExtensions.INTERFACE_STATUS:
        if self.interface_status != None: raise ParseError("An ALP command can contain one and only one Interface Status action")
        self.interface_status = action
      elif type(action) == TagRequestAction:
        if self.tag_id != None: raise ParseError("An ALP command can contain one and only one Tag Request Action")
        self.tag_id = action.operand.tag_id
        self.send_tag_response_when_completed = action.respond_when_completed
        # we don't add this to self.actions but prepend it on serializing
      elif type(action) == TagResponseAction:
        if self.tag_id != None: raise ParseError("An ALP command can contain one and only one Tag Response Action")
        self.tag_id = action.operand.tag_id
        self.completed_with_error = action.error # TODO distinguish between commands and responses?
        self.execution_completed = action.eop
      else:
        self.actions.append(action)

    if self.generate_tag_request_action and self.tag_id == None:
      self.tag_id = random.randint(0, 255)

    super(Command, self).__init__()

  def add_action(self, action):
    self.actions.append(action)

  def add_forward_action(self, interface_type=InterfaceType.HOST, interface_configuration=None):
    if interface_configuration is not None and interface_type == InterfaceType.HOST:
      raise ValueError("interface_configuration is not supported for interface_type HOST")

    if interface_type == InterfaceType.D7ASP:
      if interface_configuration is None:
        interface_configuration = Configuration()

      self.actions.append(
        RegularAction(
          operation=Forward(
            operand=InterfaceConfiguration(
              interface_id=InterfaceType.D7ASP,
              interface_configuration=interface_configuration
            )
          )
        )
      )

  @staticmethod
  def create_with_read_file_action_system_file(file, interface_type=InterfaceType.HOST, interface_configuration=None):
    # default to host interface, when D7ASP interface is used prepend with Forward action
    cmd = Command()
    cmd.add_forward_action(interface_type, interface_configuration)
    cmd.add_action(
      RegularAction(
        operation=ReadFileData(
          operand=DataRequest(
            offset=Offset(id=file.id, offset=0), # TODO offset size
            length=file.length
          )
        )
      )
    )

    return cmd

  @staticmethod
  def create_with_read_file_action(file_id, length, offset=0, interface_type=InterfaceType.HOST, interface_configuration=None):
    # default to host interface, when D7ASP interface is used prepend with Forward action
    cmd = Command()
    cmd.add_forward_action(interface_type, interface_configuration)
    cmd.add_action(
      RegularAction(
        operation=ReadFileData(
          operand=DataRequest(
            offset=Offset(id=file_id, offset=offset), # TODO offset size
            length=length
          )
        )
      )
    )

    return cmd

  @staticmethod
  def create_with_write_file_action(file_id, data, offset=0, interface_type=InterfaceType.HOST, interface_configuration=None):
    # default to host interface, when D7ASP interface is used prepend with Forward action
    cmd = Command()
    cmd.add_forward_action(interface_type, interface_configuration)
    cmd.add_action(
      RegularAction(
        operation=WriteFileData(
          operand=Data(
            offset=Offset(id=file_id, offset=offset), # TODO offset size
            data=data
          )
        )
      )
    )

    return cmd

  @staticmethod
  def create_with_write_file_action_system_file(file, interface_type=InterfaceType.HOST, interface_configuration=None):
    # default to host interface, when D7ASP interface is used prepend with Forward action
    cmd = Command()
    cmd.add_forward_action(interface_type, interface_configuration)
    cmd.add_action(
      RegularAction(
        operation=WriteFileData(
          operand=Data(
            offset=Offset(id=file.id, offset=0), # TODO offset size
            data=list(file)
          )
        )
      )
    )

    return cmd

  @staticmethod
  def create_with_return_file_data_action(file_id, data, interface_type=InterfaceType.HOST, interface_configuration=None):
    # default to host interface, when D7ASP interface is used prepend with Forward action
    cmd = Command()
    cmd.add_forward_action(interface_type, interface_configuration)
    cmd.add_action(
      RegularAction(
        operation=ReturnFileData(
          operand=Data(
            data=data,
            offset=Offset(id=file_id)
          )
        )
      )
    )

    return cmd

  def __iter__(self):
    if self.generate_tag_request_action:
      tag_request_action = TagRequestAction(
        respond_when_completed=self.send_tag_response_when_completed,
        operation=TagRequest(
          operand=TagId(tag_id=self.tag_id)
        )
      )
      for byte in tag_request_action:
        yield byte

    if self.interface_status is not None:
      for byte in self.interface_status:
        yield byte

    for action in self.actions:
      for byte in action:
        yield byte

  def describe_actions(self):
    description = ""
    for action in self.actions:
      description = description + "{}, ".format(action)

    return description.strip(", ")

  def get_d7asp_interface_status(self):
    if self.interface_status is None or self.interface_status.operand.interface_id != 0xD7:
      return None

    return self.interface_status.operation.operand.interface_status

  def __str__(self):
    output = "Command with tag {} ".format(self.tag_id)
    if(self.execution_completed):
      status = "completed"
      if(self.completed_with_error):
        status += ", with error"
      else:
        status += ", without error"
    else:
      status = "executing"

    output += "({})".format(status)

    if(len(self.actions) > 0):
      output += "\n\tactions:\n"
      for action in self.actions:
        output += "\t\taction: {}\n".format(action)

    if self.interface_status is not None:
      output += "\tinterface status: {}\n".format(self.interface_status)
    return output