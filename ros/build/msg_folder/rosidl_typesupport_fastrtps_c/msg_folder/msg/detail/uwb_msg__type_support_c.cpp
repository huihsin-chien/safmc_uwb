// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice
#include "msg_folder/msg/detail/uwb_msg__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "msg_folder/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "msg_folder/msg/detail/uwb_msg__struct.h"
#include "msg_folder/msg/detail/uwb_msg__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rosidl_runtime_c/string.h"  // from_address
#include "rosidl_runtime_c/string_functions.h"  // from_address

// forward declare type support functions


using _UwbMsg__ros_msg_type = msg_folder__msg__UwbMsg;

static bool _UwbMsg__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _UwbMsg__ros_msg_type * ros_message = static_cast<const _UwbMsg__ros_msg_type *>(untyped_ros_message);
  // Field name: range
  {
    cdr << ros_message->range;
  }

  // Field name: power
  {
    cdr << ros_message->power;
  }

  // Field name: from_address
  {
    const rosidl_runtime_c__String * str = &ros_message->from_address;
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
  }

  // Field name: anchor_key_x
  {
    cdr << ros_message->anchor_key_x;
  }

  // Field name: anchor_key_y
  {
    cdr << ros_message->anchor_key_y;
  }

  return true;
}

static bool _UwbMsg__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _UwbMsg__ros_msg_type * ros_message = static_cast<_UwbMsg__ros_msg_type *>(untyped_ros_message);
  // Field name: range
  {
    cdr >> ros_message->range;
  }

  // Field name: power
  {
    cdr >> ros_message->power;
  }

  // Field name: from_address
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->from_address.data) {
      rosidl_runtime_c__String__init(&ros_message->from_address);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->from_address,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'from_address'\n");
      return false;
    }
  }

  // Field name: anchor_key_x
  {
    cdr >> ros_message->anchor_key_x;
  }

  // Field name: anchor_key_y
  {
    cdr >> ros_message->anchor_key_y;
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_msg_folder
size_t get_serialized_size_msg_folder__msg__UwbMsg(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _UwbMsg__ros_msg_type * ros_message = static_cast<const _UwbMsg__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name range
  {
    size_t item_size = sizeof(ros_message->range);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name power
  {
    size_t item_size = sizeof(ros_message->power);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name from_address
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->from_address.size + 1);
  // field.name anchor_key_x
  {
    size_t item_size = sizeof(ros_message->anchor_key_x);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name anchor_key_y
  {
    size_t item_size = sizeof(ros_message->anchor_key_y);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _UwbMsg__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_msg_folder__msg__UwbMsg(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_msg_folder
size_t max_serialized_size_msg_folder__msg__UwbMsg(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: range
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: power
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: from_address
  {
    size_t array_size = 1;

    full_bounded = false;
    is_plain = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }
  // member: anchor_key_x
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
  }
  // member: anchor_key_y
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = msg_folder__msg__UwbMsg;
    is_plain =
      (
      offsetof(DataType, anchor_key_y) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _UwbMsg__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_msg_folder__msg__UwbMsg(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_UwbMsg = {
  "msg_folder::msg",
  "UwbMsg",
  _UwbMsg__cdr_serialize,
  _UwbMsg__cdr_deserialize,
  _UwbMsg__get_serialized_size,
  _UwbMsg__max_serialized_size
};

static rosidl_message_type_support_t _UwbMsg__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_UwbMsg,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, msg_folder, msg, UwbMsg)() {
  return &_UwbMsg__type_support;
}

#if defined(__cplusplus)
}
#endif
