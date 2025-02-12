// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_H_
#define MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'from_address'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/UwbMsg in the package msg_folder.
typedef struct msg_folder__msg__UwbMsg
{
  float range;
  float power;
  rosidl_runtime_c__String from_address;
  uint16_t anchor_key_x;
  uint16_t anchor_key_y;
} msg_folder__msg__UwbMsg;

// Struct for a sequence of msg_folder__msg__UwbMsg.
typedef struct msg_folder__msg__UwbMsg__Sequence
{
  msg_folder__msg__UwbMsg * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} msg_folder__msg__UwbMsg__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_H_
