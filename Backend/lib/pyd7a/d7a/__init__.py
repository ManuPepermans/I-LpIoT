from enum import Enum

import jsonpickle

from d7a.support.serialization.enum_handler import EnumHandler

jsonpickle.handlers.register(Enum, EnumHandler, base=True)