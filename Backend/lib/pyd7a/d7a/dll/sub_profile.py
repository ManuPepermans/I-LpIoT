import pprint

from d7a.support.schema import Validatable, Types
from d7a.types.ct import CT


class SubProfile(Validatable):

  SCHEMA = [{
    "subband_bitmap": Types.BYTE(),
    "scan_automation_period": Types.OBJECT(CT)
  }]

  def __init__(self, subband_bitmap=0, scan_automation_period=CT()):
    self.subband_bitmap = subband_bitmap
    self.scan_automation_period = scan_automation_period
    super(SubProfile, self).__init__()

  @staticmethod
  def parse(s):
    subband_bitmap = s.read("uint:8")
    scan_automation_period = CT.parse(s)
    return SubProfile(subband_bitmap=subband_bitmap, scan_automation_period=scan_automation_period)

  def __iter__(self):
    yield self.subband_bitmap
    for byte in self.scan_automation_period: yield byte

  def __str__(self):
    return pprint.PrettyPrinter().pformat(self.as_dict())