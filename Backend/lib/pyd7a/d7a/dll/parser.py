from enum import Enum

from bitstring import ConstBitStream, ReadError

from d7a.dll.background_frame import BackgroundFrame
from d7a.dll.foreground_frame import ForegroundFrame

class ParseError(Exception): pass

class FrameType(Enum):
  FOREGROUND = 0
  BACKGROUND = 1

class Parser(object):

  def __init__(self, frame_type):
    self.buffer = []
    self.frame_type = frame_type

  def parse(self, frame):
    self.buffer.extend(frame)
    return self.parse_buffer()

  def parse_buffer(self):
    parsed = 0
    frames = []

    (frame, info) = self.parse_one_frame_from_buffer()
    parsed += info["parsed"]
    frames.append(frame)
    # TODO loop
    # while True:
    #   (frame, info) = self.parse_one_frame_from_buffer()
    #   if frame is None: break
    #   parsed += info["parsed"]
    #   frames.append(frame)

    info["parsed"] = parsed
    return (frames, info)

  def shift_buffer(self, start):
    self.buffer = self.buffer[start:]
    return self

  def parse_one_frame_from_buffer(self):
    retry       = True    # until we have one or don't have enough
    errors      = []
    frame       = None
    bits_parsed = 0
    while retry and len(self.buffer) > 0:
      try:
        self.s      = ConstBitStream(bytes=self.buffer)
        if self.frame_type == FrameType.FOREGROUND:
          frame = ForegroundFrame.parse(self.s)
        else:
          frame = BackgroundFrame.parse(self.s)
        bits_parsed = self.s.pos
        self.shift_buffer(bits_parsed/8)
        retry = False         # got one, carry on
      except ReadError as e:       # not enough to read, carry on and wait for more
        retry = False
      except ParseError as e: # actual problem with current buffer, need to skip
        errors.append({
          "error"   : e.args[0],
          "buffer"  : list(self.buffer),
          "pos"     : self.s.pos,
          "skipped" : self.skip_bad_buffer_content()
        })

    info = {
      "parsed" : bits_parsed,
      "buffer" : len(self.buffer) * 8,
      "errors" : errors
    }
    return (frame, info)




