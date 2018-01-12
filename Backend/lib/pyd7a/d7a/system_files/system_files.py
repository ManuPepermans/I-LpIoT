from d7a.system_files.access_profile import AccessProfileFile
from d7a.system_files.dll_config import DllConfigFile
from d7a.system_files.firmware_version import FirmwareVersionFile
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.uid import UidFile


class SystemFiles:
  files = {
    SystemFileIds.UID: UidFile(),
    SystemFileIds.FIRMWARE_VERSION: FirmwareVersionFile(),
    SystemFileIds.DLL_CONFIG: DllConfigFile(),
    SystemFileIds.ACCESS_PROFILE_0: AccessProfileFile(access_specifier=0),
    SystemFileIds.ACCESS_PROFILE_1: AccessProfileFile(access_specifier=1),
    SystemFileIds.ACCESS_PROFILE_2: AccessProfileFile(access_specifier=2),
    SystemFileIds.ACCESS_PROFILE_3: AccessProfileFile(access_specifier=3),
    SystemFileIds.ACCESS_PROFILE_4: AccessProfileFile(access_specifier=4),
    SystemFileIds.ACCESS_PROFILE_5: AccessProfileFile(access_specifier=5),
    SystemFileIds.ACCESS_PROFILE_6: AccessProfileFile(access_specifier=6),
    SystemFileIds.ACCESS_PROFILE_7: AccessProfileFile(access_specifier=7),
    SystemFileIds.ACCESS_PROFILE_8: AccessProfileFile(access_specifier=8),
    SystemFileIds.ACCESS_PROFILE_9: AccessProfileFile(access_specifier=9),
    SystemFileIds.ACCESS_PROFILE_10: AccessProfileFile(access_specifier=10),
    SystemFileIds.ACCESS_PROFILE_11: AccessProfileFile(access_specifier=11),
    SystemFileIds.ACCESS_PROFILE_12: AccessProfileFile(access_specifier=12),
    SystemFileIds.ACCESS_PROFILE_13: AccessProfileFile(access_specifier=13),
    SystemFileIds.ACCESS_PROFILE_14: AccessProfileFile(access_specifier=14),

  }

  def get_all_system_files(self):
    return sorted(self.files, key=lambda t: t.value)