from d7a.d7anp.addressee import IdType
from d7a.support.schema import Validatable, Types

class BackgroundFrameControl(Validatable):

  SCHEMA = [{
    "id_type": Types.ENUM(IdType),
    "tag": Types.INTEGER(None, 0, 63)
  }]

  def __init__(self, id_type, tag):
    self.id_type = id_type
    self.tag = tag
    super(BackgroundFrameControl, self).__init__()

  @staticmethod
  def parse(s):
    id_type = IdType(s.read("uint:2"))
    tag = s.read("uint:6")
    return BackgroundFrameControl(
      id_type=id_type,
      tag=tag
    )

  def __iter__(self):
    byte = 0
    byte |= self.id_type.value << 6
    byte += self.tag
    yield byte
