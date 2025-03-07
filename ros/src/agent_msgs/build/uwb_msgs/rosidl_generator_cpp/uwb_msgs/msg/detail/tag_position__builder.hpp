// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from uwb_msgs:msg/TagPosition.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_
#define UWB_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "uwb_msgs/msg/detail/tag_position__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace uwb_msgs
{

namespace msg
{

namespace builder
{

class Init_TagPosition_eui
{
public:
  explicit Init_TagPosition_eui(::uwb_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  ::uwb_msgs::msg::TagPosition eui(::uwb_msgs::msg::TagPosition::_eui_type arg)
  {
    msg_.eui = std::move(arg);
    return std::move(msg_);
  }

private:
  ::uwb_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_z
{
public:
  explicit Init_TagPosition_z(::uwb_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  Init_TagPosition_eui z(::uwb_msgs::msg::TagPosition::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_TagPosition_eui(msg_);
  }

private:
  ::uwb_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_y
{
public:
  explicit Init_TagPosition_y(::uwb_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  Init_TagPosition_z y(::uwb_msgs::msg::TagPosition::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_TagPosition_z(msg_);
  }

private:
  ::uwb_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_x
{
public:
  Init_TagPosition_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TagPosition_y x(::uwb_msgs::msg::TagPosition::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_TagPosition_y(msg_);
  }

private:
  ::uwb_msgs::msg::TagPosition msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::uwb_msgs::msg::TagPosition>()
{
  return uwb_msgs::msg::builder::Init_TagPosition_x();
}

}  // namespace uwb_msgs

#endif  // UWB_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_
