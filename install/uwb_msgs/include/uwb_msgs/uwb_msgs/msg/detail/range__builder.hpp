// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from uwb_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__RANGE__BUILDER_HPP_
#define UWB_MSGS__MSG__DETAIL__RANGE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "uwb_msgs/msg/detail/range__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace uwb_msgs
{

namespace msg
{

namespace builder
{

class Init_Range_to_id
{
public:
  explicit Init_Range_to_id(::uwb_msgs::msg::Range & msg)
  : msg_(msg)
  {}
  ::uwb_msgs::msg::Range to_id(::uwb_msgs::msg::Range::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::uwb_msgs::msg::Range msg_;
};

class Init_Range_from_id
{
public:
  explicit Init_Range_from_id(::uwb_msgs::msg::Range & msg)
  : msg_(msg)
  {}
  Init_Range_to_id from_id(::uwb_msgs::msg::Range::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_Range_to_id(msg_);
  }

private:
  ::uwb_msgs::msg::Range msg_;
};

class Init_Range_power
{
public:
  explicit Init_Range_power(::uwb_msgs::msg::Range & msg)
  : msg_(msg)
  {}
  Init_Range_from_id power(::uwb_msgs::msg::Range::_power_type arg)
  {
    msg_.power = std::move(arg);
    return Init_Range_from_id(msg_);
  }

private:
  ::uwb_msgs::msg::Range msg_;
};

class Init_Range_range
{
public:
  Init_Range_range()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Range_power range(::uwb_msgs::msg::Range::_range_type arg)
  {
    msg_.range = std::move(arg);
    return Init_Range_power(msg_);
  }

private:
  ::uwb_msgs::msg::Range msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::uwb_msgs::msg::Range>()
{
  return uwb_msgs::msg::builder::Init_Range_range();
}

}  // namespace uwb_msgs

#endif  // UWB_MSGS__MSG__DETAIL__RANGE__BUILDER_HPP_
