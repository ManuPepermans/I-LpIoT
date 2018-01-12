import struct

from d7a.support.schema import Validatable, Types
from d7a.system_files.file import File
from d7a.system_files.system_file_ids import SystemFileIds


class FirmwareVersionFile(File, Validatable):

  SCHEMA = [{
    "d7a_protocol_version_major": Types.INTEGER(min=0, max=255),
    "d7a_protocol_version_minor": Types.INTEGER(min=0, max=255),
    # custom fields, specific to oss7
    "application_name": Types.STRING(maxlength=6),
    "git_sha1": Types.STRING(maxlength=7)
  }]

  def __init__(self, d7a_protocol_version_major=0, d7a_protocol_version_minor=0, application_name="", git_sha1=""):
    self.d7a_protocol_version_major = d7a_protocol_version_major
    self.d7a_protocol_version_minor = d7a_protocol_version_minor
    self.application_name = application_name
    self.git_sha1 = git_sha1
    Validatable.__init__(self)
    File.__init__(self, SystemFileIds.FIRMWARE_VERSION.value, 15)

  @property
  def d7ap_version(self):
    return str(self.d7a_protocol_version_major) + '.' + str(self.d7a_protocol_version_minor)

  @staticmethod
  def parse(s):
    major = s.read("uint:8")
    minor = s.read("uint:8")
    application_name = s.read("bytes:6").decode("ascii")
    git_sha1 = s.read("bytes:7").decode("ascii")
    return FirmwareVersionFile(d7a_protocol_version_major=major, d7a_protocol_version_minor=minor,
                               application_name=application_name, git_sha1=git_sha1)

  def __iter__(self):
    yield self.d7a_protocol_version_major
    yield self.d7a_protocol_version_minor

    for byte in bytearray(self.application_name.encode("ASCII")):
      yield byte

    for byte in bytearray(self.git_sha1.encode("ASCII")):
      yield byte

  def __str__(self):
    return "d7ap v{}, application_name={}, git_sha1={}".format(self.d7ap_version, self.application_name, self.git_sha1)
    return "d7ap v{}, application_name={}, git_sha1={}".format(self.d7ap_version, self.application_name, self.git_sha1)