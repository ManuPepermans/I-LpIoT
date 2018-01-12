import struct

from d7a.support.schema import Validatable, Types
from d7a.system_files.file import File
from d7a.system_files.system_file_ids import SystemFileIds


class UidFile(File, Validatable):
  SCHEMA = [{
    "uid": Types.INTEGER(min=0, max=0xFFFFFFFFFFFFFFFF)
  }]


  def __init__(self, uid=0):
    self.uid = uid
    File.__init__(self, SystemFileIds.UID.value, 8)
    Validatable.__init__(self)

  @staticmethod
  def parse(s):
    uid = s.read("uint:64")
    return UidFile(uid=uid)

  def __iter__(self):
    for byte in bytearray(struct.pack(">Q", self.uid)):
      yield byte


  def __str__(self):
    return "uid={}".format(hex(self.uid))