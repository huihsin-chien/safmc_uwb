// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from msg_folder:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__POSE__BUILDER_HPP_
#define MSG_FOLDER__MSG__DETAIL__POSE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "msg_folder/msg/detail/pose__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace msg_folder
{

namespace msg
{

namespace builder
{

class Init_Pose_tag
{
public:
  explicit Init_Pose_tag(::msg_folder::msg::Pose & msg)
  : msg_(msg)
  {}
  ::msg_folder::msg::Pose tag(::msg_folder::msg::Pose::_tag_type arg)
  {
    msg_.tag = std::move(arg);
    return std::move(msg_);
  }

private:
  ::msg_folder::msg::Pose msg_;
};

class Init_Pose_pos
{
public:
  Init_Pose_pos()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Pose_tag pos(::msg_folder::msg::Pose::_pos_type arg)
  {
    msg_.pos = std::move(arg);
    return Init_Pose_tag(msg_);
  }

private:
  ::msg_folder::msg::Pose msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::msg_folder::msg::Pose>()
{
  return msg_folder::msg::builder::Init_Pose_pos();
}

}  // namespace msg_folder

#endif  // MSG_FOLDER__MSG__DETAIL__POSE__BUILDER_HPP_
