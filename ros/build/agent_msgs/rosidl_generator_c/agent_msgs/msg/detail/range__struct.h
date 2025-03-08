// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from agent_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef AGENT_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
#define AGENT_MSGS__MSG__DETAIL__RANGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'from_id'
// Member 'to_id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Range in the package agent_msgs.
typedef struct agent_msgs__msg__Range
{
  double range;
  double power;
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
} agent_msgs__msg__Range;

// Struct for a sequence of agent_msgs__msg__Range.
typedef struct agent_msgs__msg__Range__Sequence
{
  agent_msgs__msg__Range * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} agent_msgs__msg__Range__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AGENT_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
