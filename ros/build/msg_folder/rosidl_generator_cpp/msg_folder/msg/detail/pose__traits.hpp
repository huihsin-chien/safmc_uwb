// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from msg_folder:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__POSE__TRAITS_HPP_
#define MSG_FOLDER__MSG__DETAIL__POSE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "msg_folder/msg/detail/pose__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'pos'
#include "geometry_msgs/msg/detail/point__traits.hpp"

namespace msg_folder
{

namespace msg
{

inline void to_flow_style_yaml(
  const Pose & msg,
  std::ostream & out)
{
  out << "{";
  // member: pos
  {
    out << "pos: ";
    to_flow_style_yaml(msg.pos, out);
    out << ", ";
  }

  // member: tag
  {
    out << "tag: ";
    rosidl_generator_traits::value_to_yaml(msg.tag, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Pose & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: pos
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pos:\n";
    to_block_style_yaml(msg.pos, out, indentation + 2);
  }

  // member: tag
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tag: ";
    rosidl_generator_traits::value_to_yaml(msg.tag, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Pose & msg, bool use_flow_style = false)
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
  const msg_folder::msg::Pose & msg,
  std::ostream & out, size_t indentation = 0)
{
  msg_folder::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use msg_folder::msg::to_yaml() instead")]]
inline std::string to_yaml(const msg_folder::msg::Pose & msg)
{
  return msg_folder::msg::to_yaml(msg);
}

template<>
inline const char * data_type<msg_folder::msg::Pose>()
{
  return "msg_folder::msg::Pose";
}

template<>
inline const char * name<msg_folder::msg::Pose>()
{
  return "msg_folder/msg/Pose";
}

template<>
struct has_fixed_size<msg_folder::msg::Pose>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<msg_folder::msg::Pose>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<msg_folder::msg::Pose>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MSG_FOLDER__MSG__DETAIL__POSE__TRAITS_HPP_
