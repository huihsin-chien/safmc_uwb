// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from uwb_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
#define UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_H_

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

/// Struct defined in msg/Range in the package uwb_msgs.
typedef struct uwb_msgs__msg__Range
{
  double range;
  double power;
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
} uwb_msgs__msg__Range;

// Struct for a sequence of uwb_msgs__msg__Range.
typedef struct uwb_msgs__msg__Range__Sequence
{
  uwb_msgs__msg__Range * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} uwb_msgs__msg__Range__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
