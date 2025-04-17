// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from uwb_msgs:msg/Pose.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "uwb_msgs/msg/detail/pose__rosidl_typesupport_introspection_c.h"
#include "uwb_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "uwb_msgs/msg/detail/pose__functions.h"
#include "uwb_msgs/msg/detail/pose__struct.h"


// Include directives for member types
// Member `pos`
#include "geometry_msgs/msg/point.h"
// Member `pos`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"
// Member `tag`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  uwb_msgs__msg__Pose__init(message_memory);
}

void uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_fini_function(void * message_memory)
{
  uwb_msgs__msg__Pose__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_member_array[2] = {
  {
    "pos",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(uwb_msgs__msg__Pose, pos),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "tag",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(uwb_msgs__msg__Pose, tag),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_members = {
  "uwb_msgs__msg",  // message namespace
  "Pose",  // message name
  2,  // number of fields
  sizeof(uwb_msgs__msg__Pose),
  uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_member_array,  // message members
  uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_init_function,  // function to initialize message memory (memory has to be allocated)
  uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_type_support_handle = {
  0,
  &uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_uwb_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, uwb_msgs, msg, Pose)() {
  uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  if (!uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_type_support_handle.typesupport_identifier) {
    uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &uwb_msgs__msg__Pose__rosidl_typesupport_introspection_c__Pose_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
