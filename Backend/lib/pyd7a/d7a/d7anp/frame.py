from d7a.d7anp.addressee import NlsMethod, IdType
from d7a.support.schema import Validatable, Types
from d7a.d7anp.control import Control
from d7a.d7atp.frame import Frame as D7atpFrame

class Frame(Validatable):

  SCHEMA = [{
    "control": Types.OBJECT(Control),
    "origin_access_class": Types.BYTE(),
    "origin_access_id": Types.BYTES(), # TODO refactor to use OriginAddressee (subclass of addressee containing control and access_id)
    "d7atp_frame": Types.OBJECT(D7atpFrame)
  }]

  def __init__(self, control, origin_access_class, origin_access_id, d7atp_frame):
    self.control = control
    self.origin_access_class = origin_access_class
    self.origin_access_id = origin_access_id
    self.d7atp_frame = d7atp_frame # TODO
    super(Frame, self).__init__()

  @staticmethod
  def parse(bitstream, payload_length):
    control = Control.parse(bitstream)
    payload_length -= 1 # substract control

    origin_access_class = bitstream.read("uint:8")
    payload_length -= 1

    assert control.has_hopping == False, "Not implemented yet"
    assert control.nls_method == NlsMethod.NONE, "Not implemented yet"

    if not control.has_no_origin_access_id:
      if control.origin_id_type == IdType.VID:
        origin_access_id = map(ord, bitstream.read("bytes:2"))
        payload_length = payload_length - 2
      elif control.origin_id_type == IdType.UID:
        origin_access_id = map(ord, bitstream.read("bytes:8"))
        payload_length = payload_length - 8
      else:
        assert False
    else:
      origin_access_id = []

    #payload=map(ord,bitstream.read("bytes:" + str(payload_length)))
    d7atp_frame = D7atpFrame.parse(bitstream, payload_length)
    return Frame(control=control, origin_access_class=origin_access_class, origin_access_id=origin_access_id, d7atp_frame=d7atp_frame)

  def __iter__(self):
    for byte in self.control: yield byte
    yield self.origin_access_class
    for byte in self.origin_access_id: yield byte
    for byte in self.d7atp_frame: yield byte