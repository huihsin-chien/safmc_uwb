// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from uwb_msgs:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_
#define UWB_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "uwb_msgs/msg/detail/pose__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace uwb_msgs
{

namespace msg
{

namespace builder
{

class Init_Pose_tag
{
public:
  explicit Init_Pose_tag(::uwb_msgs::msg::Pose & msg)
  : msg_(msg)
  {}
  ::uwb_msgs::msg::Pose tag(::uwb_msgs::msg::Pose::_tag_type arg)
  {
    msg_.tag = std::move(arg);
    return std::move(msg_);
  }

private:
  ::uwb_msgs::msg::Pose msg_;
};

class Init_Pose_pos
{
public:
  Init_Pose_pos()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Pose_tag pos(::uwb_msgs::msg::Pose::_pos_type arg)
  {
    msg_.pos = std::move(arg);
    return Init_Pose_tag(msg_);
  }

private:
  ::uwb_msgs::msg::Pose msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::uwb_msgs::msg::Pose>()
{
  return uwb_msgs::msg::builder::Init_Pose_pos();
}

}  // namespace uwb_msgs

#endif  // UWB_MSGS__MSG__DETAIL__POSE__BUILDER_HPP_
