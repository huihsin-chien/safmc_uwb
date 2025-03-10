// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from msg_folder:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_H_
#define MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'pos'
#include "geometry_msgs/msg/detail/point__struct.h"
// Member 'tag'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Pose in the package msg_folder.
typedef struct msg_folder__msg__Pose
{
  geometry_msgs__msg__Point pos;
  rosidl_runtime_c__String tag;
} msg_folder__msg__Pose;

// Struct for a sequence of msg_folder__msg__Pose.
typedef struct msg_folder__msg__Pose__Sequence
{
  msg_folder__msg__Pose * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} msg_folder__msg__Pose__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_H_
