from d7a.d7anp.addressee import IdType
from d7a.support.schema import Validatable, Types

class ForegroundFrameControl(Validatable):

  SCHEMA = [{
    "id_type": Types.ENUM(IdType),
    "eirp_index": Types.INTEGER(None, 0, 63)
  }]

  def __init__(self, id_type, eirp_index=0):
    self.id_type = id_type
    self.eirp_index = eirp_index
    super(ForegroundFrameControl, self).__init__()

  @staticmethod
  def parse(s):
    id_type = IdType(s.read("uint:2"))
    eirp_index = s.read("uint:6")
    return ForegroundFrameControl(
      id_type=id_type,
      eirp_index=eirp_index
    )

  def __iter__(self):
    byte = 0
    byte |= self.id_type.value << 6
    byte += self.eirp_index
    yield byte
