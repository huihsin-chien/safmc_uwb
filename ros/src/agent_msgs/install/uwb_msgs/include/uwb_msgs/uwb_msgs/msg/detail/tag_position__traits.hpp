// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from uwb_msgs:msg/TagPosition.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__TAG_POSITION__TRAITS_HPP_
#define UWB_MSGS__MSG__DETAIL__TAG_POSITION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "uwb_msgs/msg/detail/tag_position__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace uwb_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const TagPosition & msg,
  std::ostream & out)
{
  out << "{";
  // member: x
  {
    out << "x: ";
    rosidl_generator_traits::value_to_yaml(msg.x, out);
    out << ", ";
  }

  // member: y
  {
    out << "y: ";
    rosidl_generator_traits::value_to_yaml(msg.y, out);
    out << ", ";
  }

  // member: z
  {
    out << "z: ";
    rosidl_generator_traits::value_to_yaml(msg.z, out);
    out << ", ";
  }

  // member: eui
  {
    out << "eui: ";
    rosidl_generator_traits::value_to_yaml(msg.eui, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const TagPosition & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: x
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "x: ";
    rosidl_generator_traits::value_to_yaml(msg.x, out);
    out << "\n";
  }

  // member: y
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "y: ";
    rosidl_generator_traits::value_to_yaml(msg.y, out);
    out << "\n";
  }

  // member: z
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "z: ";
    rosidl_generator_traits::value_to_yaml(msg.z, out);
    out << "\n";
  }

  // member: eui
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "eui: ";
    rosidl_generator_traits::value_to_yaml(msg.eui, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const TagPosition & msg, bool use_flow_style = false)
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

}  // namespace uwb_msgs

namespace rosidl_generator_traits
{

[[deprecated("use uwb_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const uwb_msgs::msg::TagPosition & msg,
  std::ostream & out, size_t indentation = 0)
{
  uwb_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use uwb_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const uwb_msgs::msg::TagPosition & msg)
{
  return uwb_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<uwb_msgs::msg::TagPosition>()
{
  return "uwb_msgs::msg::TagPosition";
}

template<>
inline const char * name<uwb_msgs::msg::TagPosition>()
{
  return "uwb_msgs/msg/TagPosition";
}

template<>
struct has_fixed_size<uwb_msgs::msg::TagPosition>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<uwb_msgs::msg::TagPosition>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<uwb_msgs::msg::TagPosition>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // UWB_MSGS__MSG__DETAIL__TAG_POSITION__TRAITS_HPP_
