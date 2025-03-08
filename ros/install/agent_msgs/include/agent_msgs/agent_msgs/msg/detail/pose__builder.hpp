// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from agent_msgs:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef AGENT_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_
#define AGENT_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "agent_msgs/msg/detail/pose__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace agent_msgs
{

namespace msg
{

namespace builder
{

class Init_Pose_tag
{
public:
  explicit Init_Pose_tag(::agent_msgs::msg::Pose & msg)
  : msg_(msg)
  {}
  ::agent_msgs::msg::Pose tag(::agent_msgs::msg::Pose::_tag_type arg)
  {
    msg_.tag = std::move(arg);
    return std::move(msg_);
  }

private:
  ::agent_msgs::msg::Pose msg_;
};

class Init_Pose_pos
{
public:
  Init_Pose_pos()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Pose_tag pos(::agent_msgs::msg::Pose::_pos_type arg)
  {
    msg_.pos = std::move(arg);
    return Init_Pose_tag(msg_);
  }

private:
  ::agent_msgs::msg::Pose msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::agent_msgs::msg::Pose>()
{
  return agent_msgs::msg::builder::Init_Pose_pos();
}

}  // namespace agent_msgs

#endif  // AGENT_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_
