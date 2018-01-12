from d7a.support.schema import Validatable, Types

class Control(Validatable):

  SCHEMA = [{
    "is_dialog_start": Types.BOOLEAN(),
    "has_tl": Types.BOOLEAN(),
    "has_te": Types.BOOLEAN(),
    "is_ack_requested": Types.BOOLEAN(),
    "is_ack_not_void": Types.BOOLEAN(),
    "is_ack_record_requested": Types.BOOLEAN(),
    "has_agc": Types.BOOLEAN()
  }]

  def __init__(self, is_dialog_start, has_tl, has_te, is_ack_requested, is_ack_not_void, is_ack_record_requested, has_agc):
    self.is_dialog_start = is_dialog_start
    self.has_tl = has_tl
    self.has_te = has_te
    self.is_ack_requested = is_ack_requested
    self.is_ack_not_void = is_ack_not_void
    self.is_ack_record_requested = is_ack_record_requested
    self.has_agc = has_agc
    super(Control, self).__init__()

  @staticmethod
  def parse(s):
    is_dialog_start = s.read("bool")
    _ = s.read("bool") # RFU
    has_tl = s.read("bool")
    has_te = s.read("bool")
    is_ack_requested = s.read("bool")
    is_ack_not_void = s.read("bool")
    is_ack_record_requested = s.read("bool")
    has_agc = s.read("bool")

    return Control(
      is_dialog_start=is_dialog_start,
      has_tl=has_tl,
      has_te=has_te,
      is_ack_requested=is_ack_requested,
      is_ack_not_void=is_ack_not_void,
      is_ack_record_requested=is_ack_record_requested,
      has_agc=has_agc
    )

  def __iter__(self):
    byte = 0
    if self.is_dialog_start: byte |= 1 << 7
    if self.has_tl:  byte |= 1 << 5
    if self.has_te:  byte |= 1 << 4
    if self.is_ack_requested:  byte |= 1 << 3
    if self.is_ack_not_void:  byte |= 1 << 2
    if self.is_ack_record_requested: byte |= 1 << 1
    byte += self.has_agc
    yield byte