# responses
# author: Christophe VG <contact@christophe.vg>

# class implementation of responses
from bitstring import ConstBitStream

from d7a.alp.operations.operation import Operation

from d7a.alp.operands.file        import Data, Offset
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles


class ReturnFileData(Operation):
  def __init__(self, *args, **kwargs):
    self.systemfile_type = None
    self.file_data_parsed = None
    self.op     = 32
    self.operand_class = Data
    super(ReturnFileData, self).__init__(*args, **kwargs)
    self.try_parse_system_file()

  def try_parse_system_file(self):
    # when reading a known system files we store the parsed data and filename
    try:
      systemfile_type = SystemFiles().files[SystemFileIds(self.operand.offset.id)]
    except:
      return

    if systemfile_type is not None and systemfile_type.length == self.operand.length:
      self.systemfile_type = systemfile_type
      self.file_data_parsed = systemfile_type.parse(ConstBitStream(bytearray(self.operand.data)))

