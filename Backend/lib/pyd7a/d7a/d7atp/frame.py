
from d7a.support.schema import Validatable, Types
from d7a.d7atp.control import Control
from d7a.types.ct import CT
from d7a.alp.command import Command
from d7a.alp.parser import Parser as AlpParser

class Frame(Validatable):

  SCHEMA = [{
    "control": Types.OBJECT(Control),
    "dialog_id": Types.INTEGER(min=0, max=255),
    "transaction_id": Types.INTEGER(min=0, max=255),
    "agc_rx_level_i": Types.INTEGER(min=0, max=31),
    "tl": Types.OBJECT(CT, nullable=True),
    "te": Types.OBJECT(CT, nullable=True),
    "tc": Types.OBJECT(CT, nullable=True),
    "ack_template": Types.OBJECT(nullable=True), # TODO
    "alp_command": Types.OBJECT(Command)
  }]

  def __init__(self, control, dialog_id, transaction_id, alp_command, agc_rx_level_i=10, tl=None, te=None, tc=None, ack_template=None):
    if agc_rx_level_i == None:
      agc_rx_level_i = 10

    self.control = control
    self.dialog_id = dialog_id
    self.transaction_id = transaction_id
    self.agc_rx_level_i = agc_rx_level_i
    self.tl = tl
    self.te = te
    self.tc = tc
    self.ack_template = ack_template
    self.alp_command = alp_command
    super(Frame, self).__init__()

  @staticmethod
  def parse(bitstream, payload_length):
    control = Control.parse(bitstream)
    payload_length = payload_length - 1 # subtract control byte

    dialog_id = bitstream.read("uint:8")
    payload_length = payload_length - 1

    transaction_id = bitstream.read("uint:8")
    payload_length = payload_length - 1

    target_rx_level_i = None
    if control.has_agc:
      target_rx_level_i = bitstream.read("uint:8")
      payload_length -= 1

    tl = None
    if control.has_tl:
      tl = CT.parse(bitstream)
      payload_length -= 1

    te = None
    if control.has_te:
      te = CT.parse(bitstream)
      payload_length -= 1

    tc = None
    # TODO currently we have no way to know if Tc is present or not
    # Tc is present when control.is_ack_requested AND when we are requester,
    # while responders copy this flag but do NOT provide a Tc.
    # When parsing single frames without knowledge of dialogs we cannot determine this.
    # We use control.is_dialog_start for now but this will break when we start supporting multiple transactions per dialog
    if control.is_ack_requested and control.is_dialog_start:
      tc = CT.parse(bitstream)
      payload_length -= 1

    ack_template = None
    if control.is_ack_not_void:
      transaction_id_start = bitstream.read("uint:8")
      payload_length = payload_length - 1
      transaction_id_stop = bitstream.read("uint:8")
      payload_length = payload_length - 1
      assert transaction_id_start == transaction_id, "Other case not implemented yet"
      assert transaction_id_stop == transaction_id, "Other case not implemented yet"
      # TODO ack bitmap (for when transaction_id_start != transaction_id)
      ack_template = [ transaction_id_start, transaction_id_stop ]

    assert control.is_ack_record_requested == False, "Not implemented yet"
    assert control.is_ack_not_void == False, "Not implemented yet"

    alp_command = AlpParser().parse(bitstream, payload_length)

    return Frame(
      control=control,
      dialog_id=dialog_id,
      transaction_id=transaction_id,
      agc_rx_level_i=target_rx_level_i,
      tl=tl,
      te=te,
      tc=tc,
      ack_template=ack_template,
      alp_command=alp_command
    )

  def __iter__(self):
    for byte in self.control: yield byte
    yield self.dialog_id
    yield self.transaction_id
    if self.control.has_agc:
      yield self.agc_rx_level_i

    if self.control.has_tl:
      yield self.tl

    if self.control.has_te:
      yield self.te

    if self.control.is_ack_not_void:
      for byte in self.ack_template: yield byte

    for byte in self.alp_command: yield byte