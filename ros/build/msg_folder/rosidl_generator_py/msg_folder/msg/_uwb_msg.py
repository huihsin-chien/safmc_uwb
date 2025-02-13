# generated from rosidl_generator_py/resource/_idl.py.em
# with input from msg_folder:msg/UwbMsg.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_UwbMsg(type):
    """Metaclass of message 'UwbMsg'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('msg_folder')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'msg_folder.msg.UwbMsg')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__uwb_msg
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__uwb_msg
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__uwb_msg
            cls._TYPE_SUPPORT = module.type_support_msg__msg__uwb_msg
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__uwb_msg

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class UwbMsg(metaclass=Metaclass_UwbMsg):
    """Message class 'UwbMsg'."""

    __slots__ = [
        '_range',
        '_power',
        '_from_address',
        '_anchor_key_x',
        '_anchor_key_y',
    ]

    _fields_and_field_types = {
        'range': 'float',
        'power': 'float',
        'from_address': 'string',
        'anchor_key_x': 'uint16',
        'anchor_key_y': 'uint16',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('uint16'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint16'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.range = kwargs.get('range', float())
        self.power = kwargs.get('power', float())
        self.from_address = kwargs.get('from_address', str())
        self.anchor_key_x = kwargs.get('anchor_key_x', int())
        self.anchor_key_y = kwargs.get('anchor_key_y', int())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.range != other.range:
            return False
        if self.power != other.power:
            return False
        if self.from_address != other.from_address:
            return False
        if self.anchor_key_x != other.anchor_key_x:
            return False
        if self.anchor_key_y != other.anchor_key_y:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property  # noqa: A003
    def range(self):  # noqa: A003
        """Message field 'range'."""
        return self._range

    @range.setter  # noqa: A003
    def range(self, value):  # noqa: A003
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'range' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'range' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._range = value

    @builtins.property
    def power(self):
        """Message field 'power'."""
        return self._power

    @power.setter
    def power(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'power' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'power' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._power = value

    @builtins.property
    def from_address(self):
        """Message field 'from_address'."""
        return self._from_address

    @from_address.setter
    def from_address(self, value):
        if __debug__:
            assert \
                isinstance(value, str), \
                "The 'from_address' field must be of type 'str'"
        self._from_address = value

    @builtins.property
    def anchor_key_x(self):
        """Message field 'anchor_key_x'."""
        return self._anchor_key_x

    @anchor_key_x.setter
    def anchor_key_x(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'anchor_key_x' field must be of type 'int'"
            assert value >= 0 and value < 65536, \
                "The 'anchor_key_x' field must be an unsigned integer in [0, 65535]"
        self._anchor_key_x = value

    @builtins.property
    def anchor_key_y(self):
        """Message field 'anchor_key_y'."""
        return self._anchor_key_y

    @anchor_key_y.setter
    def anchor_key_y(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'anchor_key_y' field must be of type 'int'"
            assert value >= 0 and value < 65536, \
                "The 'anchor_key_y' field must be an unsigned integer in [0, 65535]"
        self._anchor_key_y = value
