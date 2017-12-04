from d7a.alp.action import Action
from d7a.alp.operands.interface_configuration import InterfaceConfiguration
from d7a.alp.operations.forward import Forward
from d7a.alp.operations.nop import NoOperation
from d7a.alp.operations.operation import Operation
from d7a.support.schema import Types


class ForwardAction(Action):
  SCHEMA = [{
    "resp"     : Types.BOOLEAN(),
    "op"       : Types.BITS(6),
    "operation": Types.OBJECT(Operation),
    "operand"  : Types.OBJECT(InterfaceConfiguration)  # TODO for now only D7 interface is supported
  }]

  def __init__(self, resp=False, operation=NoOperation()):
    self.resp      = resp
    super(ForwardAction, self).__init__(operation)

  def __iter__(self):
    byte = 0
    if self.group: byte |= 1 << 7
    if self.resp:  byte |= 1 << 6
    byte += self.op
    yield byte

    for byte in self.operation: yield byte
