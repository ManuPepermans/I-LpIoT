from d7a.alp.interface import InterfaceType
from d7a.sp.configuration import Configuration
from d7a.support.schema import Validatable, Types


class InterfaceConfiguration(Validatable):

  SCHEMA = [{
    "interface_id"        : Types.ENUM(InterfaceType),
    "interface_configuration"    : Types.OBJECT(Configuration)
  }]

  def __init__(self, interface_id, interface_configuration):
    self.interface_id = interface_id
    self.interface_configuration   = interface_configuration
    super(InterfaceConfiguration, self).__init__()

  def __iter__(self):
    yield self.interface_id.value
    for byte in self.interface_configuration: yield byte

  def __str__(self):
    return "interface-id={}, configuration={}".format(self.interface_id, self.interface_configuration)
