import importlib

import jsonpickle


class EnumHandler(jsonpickle.handlers.BaseHandler):
  def flatten(self, obj, data):
    data['value'] = obj.name
    return data

  def restore(self, obj):
    module_name, class_name = obj['py/object'].rsplit(".", 1)
    MyEnum = getattr(importlib.import_module(module_name), class_name)
    return MyEnum[obj['value']]
