// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from msg_folder:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__RANGE__BUILDER_HPP_
#define MSG_FOLDER__MSG__DETAIL__RANGE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "msg_folder/msg/detail/range__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace msg_folder
{

namespace msg
{

namespace builder
{

class Init_Range_to_id
{
public:
  explicit Init_Range_to_id(::msg_folder::msg::Range & msg)
  : msg_(msg)
  {}
  ::msg_folder::msg::Range to_id(::msg_folder::msg::Range::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::msg_folder::msg::Range msg_;
};

class Init_Range_from_id
{
public:
  explicit Init_Range_from_id(::msg_folder::msg::Range & msg)
  : msg_(msg)
  {}
  Init_Range_to_id from_id(::msg_folder::msg::Range::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_Range_to_id(msg_);
  }

private:
  ::msg_folder::msg::Range msg_;
};

class Init_Range_power
{
public:
  explicit Init_Range_power(::msg_folder::msg::Range & msg)
  : msg_(msg)
  {}
  Init_Range_from_id power(::msg_folder::msg::Range::_power_type arg)
  {
    msg_.power = std::move(arg);
    return Init_Range_from_id(msg_);
  }

private:
  ::msg_folder::msg::Range msg_;
};

class Init_Range_range
{
public:
  Init_Range_range()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Range_power range(::msg_folder::msg::Range::_range_type arg)
  {
    msg_.range = std::move(arg);
    return Init_Range_power(msg_);
  }

private:
  ::msg_folder::msg::Range msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::msg_folder::msg::Range>()
{
  return msg_folder::msg::builder::Init_Range_range();
}

}  // namespace msg_folder

#endif  // MSG_FOLDER__MSG__DETAIL__RANGE__BUILDER_HPP_
