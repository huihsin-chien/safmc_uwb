// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__UWB_MSG__TRAITS_HPP_
#define MSG_FOLDER__MSG__DETAIL__UWB_MSG__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "msg_folder/msg/detail/uwb_msg__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace msg_folder
{

namespace msg
{

inline void to_flow_style_yaml(
  const UwbMsg & msg,
  std::ostream & out)
{
  out << "{";
  // member: range
  {
    out << "range: ";
    rosidl_generator_traits::value_to_yaml(msg.range, out);
    out << ", ";
  }

  // member: power
  {
    out << "power: ";
    rosidl_generator_traits::value_to_yaml(msg.power, out);
    out << ", ";
  }

  // member: from_address
  {
    out << "from_address: ";
    rosidl_generator_traits::value_to_yaml(msg.from_address, out);
    out << ", ";
  }

  // member: anchor_key_x
  {
    out << "anchor_key_x: ";
    rosidl_generator_traits::value_to_yaml(msg.anchor_key_x, out);
    out << ", ";
  }

  // member: anchor_key_y
  {
    out << "anchor_key_y: ";
    rosidl_generator_traits::value_to_yaml(msg.anchor_key_y, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const UwbMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: range
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "range: ";
    rosidl_generator_traits::value_to_yaml(msg.range, out);
    out << "\n";
  }

  // member: power
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "power: ";
    rosidl_generator_traits::value_to_yaml(msg.power, out);
    out << "\n";
  }

  // member: from_address
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "from_address: ";
    rosidl_generator_traits::value_to_yaml(msg.from_address, out);
    out << "\n";
  }

  // member: anchor_key_x
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "anchor_key_x: ";
    rosidl_generator_traits::value_to_yaml(msg.anchor_key_x, out);
    out << "\n";
  }

  // member: anchor_key_y
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "anchor_key_y: ";
    rosidl_generator_traits::value_to_yaml(msg.anchor_key_y, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const UwbMsg & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace msg_folder

namespace rosidl_generator_traits
{

[[deprecated("use msg_folder::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const msg_folder::msg::UwbMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  msg_folder::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use msg_folder::msg::to_yaml() instead")]]
inline std::string to_yaml(const msg_folder::msg::UwbMsg & msg)
{
  return msg_folder::msg::to_yaml(msg);
}

template<>
inline const char * data_type<msg_folder::msg::UwbMsg>()
{
  return "msg_folder::msg::UwbMsg";
}

template<>
inline const char * name<msg_folder::msg::UwbMsg>()
{
  return "msg_folder/msg/UwbMsg";
}

template<>
struct has_fixed_size<msg_folder::msg::UwbMsg>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<msg_folder::msg::UwbMsg>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<msg_folder::msg::UwbMsg>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MSG_FOLDER__MSG__DETAIL__UWB_MSG__TRAITS_HPP_
