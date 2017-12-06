import struct

from d7a.support.schema import Validatable, Types
from d7a.system_files.file import File
from d7a.system_files.system_file_ids import SystemFileIds


class DllConfigFile(File, Validatable):
  SCHEMA = [{
    "active_access_class": Types.INTEGER(min=0, max=0xFF),
    "vid": Types.INTEGER(min=0, max=0xFFFF)
    # TODO others
  }]

  def __init__(self, active_access_class=0, vid=0xFFFF):
    self.active_access_class = active_access_class
    self.vid = vid
    Validatable.__init__(self)
    File.__init__(self, SystemFileIds.DLL_CONFIG.value, 6)

  @staticmethod
  def parse(s):
    ac = s.read("uint:8")
    vid = s.read("uint:16")
    return DllConfigFile(active_access_class=ac, vid=vid)

  def __iter__(self):
    yield self.active_access_class
    for byte in bytearray(struct.pack(">H", self.vid)):
      yield byte


  def __str__(self):
    return "active_access_class={}, vid={}".format(self.active_access_class, self.vid)