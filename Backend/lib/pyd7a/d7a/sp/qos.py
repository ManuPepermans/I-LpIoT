# qos
# author: Christophe VG <contact@christophe.vg>

# class implementation of qos parameters
from enum import Enum

from d7a.support.schema import Validatable, Types

class ResponseMode(Enum):
  RESP_MODE_NO = 0
  RESP_MODE_ALL = 1
  RESP_MODE_ANY = 2
  RESP_MODE_NO_RPT = 4
  RESP_MODE_ON_ERROR = 5
  RESP_MODE_PREFERRED = 6

class RetryMode(Enum):
  RETRY_MODE_NO = 0

class QoS(Validatable):
  SCHEMA = [{
    "stop_on_err": Types.BOOLEAN(),
    "record" : Types.BOOLEAN(),
    "resp_mod"     : Types.ENUM(ResponseMode),
    "retry_mod": Types.ENUM(RetryMode)
  }]
  
  def __init__(self, resp_mod=ResponseMode.RESP_MODE_NO, retry_mod=RetryMode.RETRY_MODE_NO, stop_on_err=False, record=False):
    self.resp_mod     = resp_mod
    self.retry_mod    = retry_mod
    self.stop_on_err  = stop_on_err
    self.record       = record
    super(QoS, self).__init__()

  def __iter__(self):
    byte = 0
    if self.stop_on_err: byte |= 1 << 7
    if self.record: byte |= 1 << 6
    byte |= self.retry_mod.value << 3
    byte += self.resp_mod.value
    yield byte

  @staticmethod
  def parse(s):
    stop_on_error = s.read("bool")
    record = s.read("bool")
    retry_mode = RetryMode(int(s.read("uint:3")))
    resp_mode = ResponseMode(int(s.read("uint:3")))
    return QoS(stop_on_err=stop_on_error, record=record, resp_mod=resp_mode, retry_mod=retry_mode)

  def __str__(self):
    return str(self.as_dict())