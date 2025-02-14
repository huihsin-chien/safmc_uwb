// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from msg_folder:msg/Range.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "msg_folder/msg/detail/range__rosidl_typesupport_introspection_c.h"
#include "msg_folder/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "msg_folder/msg/detail/range__functions.h"
#include "msg_folder/msg/detail/range__struct.h"


// Include directives for member types
// Member `from_id`
// Member `to_id`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  msg_folder__msg__Range__init(message_memory);
}

void msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_fini_function(void * message_memory)
{
  msg_folder__msg__Range__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_member_array[4] = {
  {
    "range",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(msg_folder__msg__Range, range),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "power",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(msg_folder__msg__Range, power),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "from_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(msg_folder__msg__Range, from_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "to_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(msg_folder__msg__Range, to_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_members = {
  "msg_folder__msg",  // message namespace
  "Range",  // message name
  4,  // number of fields
  sizeof(msg_folder__msg__Range),
  msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_member_array,  // message members
  msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_init_function,  // function to initialize message memory (memory has to be allocated)
  msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle = {
  0,
  &msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_msg_folder
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, msg_folder, msg, Range)() {
  if (!msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle.typesupport_identifier) {
    msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &msg_folder__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
