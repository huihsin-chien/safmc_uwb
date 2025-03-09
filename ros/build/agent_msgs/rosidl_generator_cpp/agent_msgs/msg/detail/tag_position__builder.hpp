// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from agent_msgs:msg/TagPosition.idl
// generated code does not contain a copyright notice

#ifndef AGENT_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_
#define AGENT_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "agent_msgs/msg/detail/tag_position__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace agent_msgs
{

namespace msg
{

namespace builder
{

class Init_TagPosition_timestamp
{
public:
  explicit Init_TagPosition_timestamp(::agent_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  ::agent_msgs::msg::TagPosition timestamp(::agent_msgs::msg::TagPosition::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::agent_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_eui
{
public:
  explicit Init_TagPosition_eui(::agent_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  Init_TagPosition_timestamp eui(::agent_msgs::msg::TagPosition::_eui_type arg)
  {
    msg_.eui = std::move(arg);
    return Init_TagPosition_timestamp(msg_);
  }

private:
  ::agent_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_z
{
public:
  explicit Init_TagPosition_z(::agent_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  Init_TagPosition_eui z(::agent_msgs::msg::TagPosition::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_TagPosition_eui(msg_);
  }

private:
  ::agent_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_y
{
public:
  explicit Init_TagPosition_y(::agent_msgs::msg::TagPosition & msg)
  : msg_(msg)
  {}
  Init_TagPosition_z y(::agent_msgs::msg::TagPosition::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_TagPosition_z(msg_);
  }

private:
  ::agent_msgs::msg::TagPosition msg_;
};

class Init_TagPosition_x
{
public:
  Init_TagPosition_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TagPosition_y x(::agent_msgs::msg::TagPosition::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_TagPosition_y(msg_);
  }

private:
  ::agent_msgs::msg::TagPosition msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::agent_msgs::msg::TagPosition>()
{
  return agent_msgs::msg::builder::Init_TagPosition_x();
}

}  // namespace agent_msgs

#endif  // AGENT_MSGS__MSG__DETAIL__TAG_POSITION__BUILDER_HPP_
