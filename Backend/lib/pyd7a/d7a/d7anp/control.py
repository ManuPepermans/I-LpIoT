from d7a.d7anp.addressee import IdType, NlsMethod
from d7a.support.schema import Validatable, Types

class Control(Validatable):

  SCHEMA = [{
    "has_no_origin_access_id": Types.BOOLEAN(),
    "has_hopping": Types.BOOLEAN(),
    "origin_id_type": Types.ENUM(IdType, allowedvalues=[IdType.UID, IdType.VID]),
    "nls_method": Types.ENUM(NlsMethod),
  }]

  def __init__(self, has_no_origin_access_id, has_hopping, nls_method, origin_id_type):
    self.has_no_origin_access_id = has_no_origin_access_id
    self.nls_method = nls_method
    self.has_hopping = has_hopping
    self.origin_id_type = origin_id_type
    super(Control, self).__init__()

  @staticmethod
  def parse(bitstream):
    return Control(
      has_no_origin_access_id=bitstream.read("bool"),
      has_hopping=bitstream.read("bool"),
      origin_id_type=IdType(bitstream.read("uint:2")),
      nls_method=NlsMethod(bitstream.read("uint:4")),
    )

  def __iter__(self):
    byte = 0
    if self.has_no_origin_access_id: byte |= 1 << 7
    if self.has_multi_hop:  byte |= 1 << 6
    if self.origin_id_type:  byte |= 1 << 4
    byte += self.nls_method
    yield byte