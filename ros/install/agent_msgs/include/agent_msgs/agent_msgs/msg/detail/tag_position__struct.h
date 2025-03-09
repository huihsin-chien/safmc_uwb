// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from agent_msgs:msg/TagPosition.idl
// generated code does not contain a copyright notice

#ifndef AGENT_MSGS__MSG__DETAIL__TAG_POSITION__STRUCT_H_
#define AGENT_MSGS__MSG__DETAIL__TAG_POSITION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'eui'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/TagPosition in the package agent_msgs.
typedef struct agent_msgs__msg__TagPosition
{
  double x;
  double y;
  double z;
  rosidl_runtime_c__String eui;
  /// in nano-second
  int64_t timestamp;
} agent_msgs__msg__TagPosition;

// Struct for a sequence of agent_msgs__msg__TagPosition.
typedef struct agent_msgs__msg__TagPosition__Sequence
{
  agent_msgs__msg__TagPosition * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} agent_msgs__msg__TagPosition__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AGENT_MSGS__MSG__DETAIL__TAG_POSITION__STRUCT_H_
