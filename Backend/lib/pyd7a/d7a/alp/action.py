# action
# author: Christophe VG <contact@christophe.vg>

# class implementation of action parameters

# D7A ALP Action 
from d7a.alp.operations.requests import ReadFileData
from d7a.alp.operations.responses import ReturnFileData
from d7a.alp.operations.write_operations import WriteFileData
from d7a.support.schema           import Validatable, Types

from d7a.alp.operations.operation import Operation
from d7a.alp.operations.nop       import NoOperation

class Action(Validatable):

  SCHEMA = [{
    "op"       : Types.BITS(6),
    "operation": Types.OBJECT(Operation),
    "operand"  : Types.OBJECT(nullable=True)  # there is no Operand base-class
  }]

  def __init__(self, operation=NoOperation()):
    self.operation = operation
    super(Action, self).__init__()

  @property
  def op(self):
    return self.operation.op

  @property
  def operand(self):
    return self.operation.operand

  def __str__(self):
    if isinstance(self.operation, ReturnFileData):
      # when reading a known system files we output the parsed data
      if self.operation.systemfile_type != None and self.operation.file_data_parsed != None:
        return "Received {} content: {}".format(self.operation.systemfile_type.__class__.__name__,
                                                self.operation.file_data_parsed)

    return str(self.operation)
