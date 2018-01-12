from d7a.dll.access_profile import AccessProfile
from d7a.support.schema import Validatable, Types
from d7a.system_files.file import File
from d7a.system_files.system_file_ids import SystemFileIds


class AccessProfileFile(File, Validatable):
  SCHEMA = [{
    "access_specifier": Types.INTEGER(min=0, max=14),
    "access_profile": Types.OBJECT(AccessProfile, nullable=True)
  }]

  def __init__(self, access_specifier=0, access_profile=None):
    self.access_specifier = access_specifier
    self.access_profile = access_profile
    Validatable.__init__(self)
    File.__init__(self, SystemFileIds.ACCESS_PROFILE_0.value + access_specifier, 65)

  def __iter__(self):
    for byte in self.access_profile:
      yield byte

  @staticmethod
  def parse(s):
    return AccessProfileFile(access_specifier=0, access_profile=AccessProfile.parse(s)) # TODO access_specifier?

  def __str__(self):
    return "active_specifier={}, access_profile={}".format(self.access_specifier, self.access_profile)