// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from agent_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "agent_msgs/msg/detail/range__rosidl_typesupport_introspection_c.h"
#include "agent_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "agent_msgs/msg/detail/range__functions.h"
#include "agent_msgs/msg/detail/range__struct.h"


// Include directives for member types
// Member `from_id`
// Member `to_id`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  agent_msgs__msg__Range__init(message_memory);
}

void agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_fini_function(void * message_memory)
{
  agent_msgs__msg__Range__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_member_array[4] = {
  {
    "range",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(agent_msgs__msg__Range, range),  // bytes offset in struct
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
    offsetof(agent_msgs__msg__Range, power),  // bytes offset in struct
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
    offsetof(agent_msgs__msg__Range, from_id),  // bytes offset in struct
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
    offsetof(agent_msgs__msg__Range, to_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_members = {
  "agent_msgs__msg",  // message namespace
  "Range",  // message name
  4,  // number of fields
  sizeof(agent_msgs__msg__Range),
  agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_member_array,  // message members
  agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_init_function,  // function to initialize message memory (memory has to be allocated)
  agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle = {
  0,
  &agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_agent_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, agent_msgs, msg, Range)() {
  if (!agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle.typesupport_identifier) {
    agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &agent_msgs__msg__Range__rosidl_typesupport_introspection_c__Range_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
