# addressee
# author: Christophe VG <contact@christophe.vg>

# class implementation of addressee parameters


import struct
from enum import Enum

from d7a.support.schema import Validatable, Types
from d7a.types.ct import CT


class IdType(Enum):
  NBID  = 0
  NOID  = 1
  UID   = 2
  VID   = 3

class NlsMethod(Enum):
  NONE = 0
  AES_CTR = 1
  AES_CBC_MAC_128 = 2
  AES_CBC_MAC_64 = 3
  AES_CBC_MAC_32 = 4
  AES_CCM_128 = 5
  AES_CCM_64 = 6
  AES_CCM_32 = 7

class Addressee(Validatable):
  
  # addressee ID length
  ID_LENGTH_NBID = 1
  ID_LENGTH_NOID = 0
  ID_LENGTH_VID = 2
  ID_LENGTH_UID = 8

  SCHEMA = [
    {
      # void identifier with reached devices estimation
      "id_type"   : Types.ENUM(IdType, allowedvalues=[IdType.NBID]),
      "nls_method": Types.ENUM(NlsMethod),
      "access_class": Types.BYTE(),
      "id"        : Types.OBJECT(CT)
    },
    {
      # void identifier without reached devices estimation
      "id_type": Types.ENUM(IdType, allowedvalues=[IdType.NOID]),
      "nls_method": Types.ENUM(NlsMethod),
      "access_class": Types.BYTE(),
      "id": Types.INTEGER([None])
    },
    {
       # virtual
      "id_type"   : Types.ENUM(IdType, allowedvalues=[IdType.VID]),
      "nls_method": Types.ENUM(NlsMethod),
      "access_class": Types.BYTE(),
      "id"        : Types.INTEGER(min=0, max=0xFFFF)
    },
    {
      # unicast
      "id_type"   : Types.ENUM(IdType, allowedvalues=[IdType.UID]),
      "nls_method": Types.ENUM(NlsMethod),
      "access_class": Types.BYTE(),
      "id"        : Types.INTEGER(min=0, max=0xFFFFFFFFFFFFFFFF)
     }
   ]


#  SCHEMA = [
#    {
#      "id_type"   : Types.INTEGER(IdType.ALL),
#      "cl"        : Types.BITS(4),
#      "id_length" : Types.INTEGER([0, 2, 8]),
#      "id"        : Types.INTEGER(min=0, max=0xFFFFFFFFFFFFFFFF)
#    }
#  ]

  def __init__(self, access_class=0, id_type=IdType.NOID, id=None, nls_method=NlsMethod.NONE):
    self.id_type = id_type
    self.access_class = access_class
    self.id = id
    self.nls_method = nls_method
    super(Addressee, self).__init__()

  @property
  def id_length(self):
    return Addressee.length_for(id_type=self.id_type)

  @classmethod
  def length_for(self, id_type):
    if id_type == IdType.NBID: return Addressee.ID_LENGTH_NBID
    if id_type == IdType.NOID: return Addressee.ID_LENGTH_NOID
    if id_type == IdType.VID: return Addressee.ID_LENGTH_VID
    if id_type == IdType.UID: return Addressee.ID_LENGTH_UID

  @staticmethod
  def parse(s):
    _     = s.read("pad:2")
    id_type = IdType(s.read("uint:2"))
    nls_method = NlsMethod(s.read("uint:4"))
    cl    = s.read("uint:8")
    l     = Addressee.length_for(id_type)
    id    = s.read("uint:"+str(l*8)) if l > 0 else None
    return Addressee(id_type=id_type, access_class=cl, id=id, nls_method=nls_method)

  def __iter__(self):
    byte = 0
    # pad 2 << 7 << 6
    byte |= self.id_type.value << 4
    byte += self.nls_method.value
    yield byte
    yield self.access_class
    if self.id_length > 0:
      id = bytearray(struct.pack(">Q", self.id))[8-self.id_length:]
      for byte in id: yield byte

  def __str__(self):
    return "ac={}, id_type={}, id={}".format(self.access_class, self.id_type, hex(self.id))