// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from uwb_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__RANGE__TRAITS_HPP_
#define UWB_MSGS__MSG__DETAIL__RANGE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "uwb_msgs/msg/detail/range__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace uwb_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Range & msg,
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

  // member: from_id
  {
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << ", ";
  }

  // member: to_id
  {
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Range & msg,
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

  // member: from_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << "\n";
  }

  // member: to_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Range & msg, bool use_flow_style = false)
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
  const uwb_msgs::msg::Range & msg,
  std::ostream & out, size_t indentation = 0)
{
  uwb_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use uwb_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const uwb_msgs::msg::Range & msg)
{
  return uwb_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<uwb_msgs::msg::Range>()
{
  return "uwb_msgs::msg::Range";
}

template<>
inline const char * name<uwb_msgs::msg::Range>()
{
  return "uwb_msgs/msg/Range";
}

template<>
struct has_fixed_size<uwb_msgs::msg::Range>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<uwb_msgs::msg::Range>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<uwb_msgs::msg::Range>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // UWB_MSGS__MSG__DETAIL__RANGE__TRAITS_HPP_
