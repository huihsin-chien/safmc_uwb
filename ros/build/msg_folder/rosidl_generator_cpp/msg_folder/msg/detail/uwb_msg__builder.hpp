// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__UWB_MSG__BUILDER_HPP_
#define MSG_FOLDER__MSG__DETAIL__UWB_MSG__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "msg_folder/msg/detail/uwb_msg__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace msg_folder
{

namespace msg
{

namespace builder
{

class Init_UwbMsg_anchor_key_y
{
public:
  explicit Init_UwbMsg_anchor_key_y(::msg_folder::msg::UwbMsg & msg)
  : msg_(msg)
  {}
  ::msg_folder::msg::UwbMsg anchor_key_y(::msg_folder::msg::UwbMsg::_anchor_key_y_type arg)
  {
    msg_.anchor_key_y = std::move(arg);
    return std::move(msg_);
  }

private:
  ::msg_folder::msg::UwbMsg msg_;
};

class Init_UwbMsg_anchor_key_x
{
public:
  explicit Init_UwbMsg_anchor_key_x(::msg_folder::msg::UwbMsg & msg)
  : msg_(msg)
  {}
  Init_UwbMsg_anchor_key_y anchor_key_x(::msg_folder::msg::UwbMsg::_anchor_key_x_type arg)
  {
    msg_.anchor_key_x = std::move(arg);
    return Init_UwbMsg_anchor_key_y(msg_);
  }

private:
  ::msg_folder::msg::UwbMsg msg_;
};

class Init_UwbMsg_from_address
{
public:
  explicit Init_UwbMsg_from_address(::msg_folder::msg::UwbMsg & msg)
  : msg_(msg)
  {}
  Init_UwbMsg_anchor_key_x from_address(::msg_folder::msg::UwbMsg::_from_address_type arg)
  {
    msg_.from_address = std::move(arg);
    return Init_UwbMsg_anchor_key_x(msg_);
  }

private:
  ::msg_folder::msg::UwbMsg msg_;
};

class Init_UwbMsg_power
{
public:
  explicit Init_UwbMsg_power(::msg_folder::msg::UwbMsg & msg)
  : msg_(msg)
  {}
  Init_UwbMsg_from_address power(::msg_folder::msg::UwbMsg::_power_type arg)
  {
    msg_.power = std::move(arg);
    return Init_UwbMsg_from_address(msg_);
  }

private:
  ::msg_folder::msg::UwbMsg msg_;
};

class Init_UwbMsg_range
{
public:
  Init_UwbMsg_range()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_UwbMsg_power range(::msg_folder::msg::UwbMsg::_range_type arg)
  {
    msg_.range = std::move(arg);
    return Init_UwbMsg_power(msg_);
  }

private:
  ::msg_folder::msg::UwbMsg msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::msg_folder::msg::UwbMsg>()
{
  return msg_folder::msg::builder::Init_UwbMsg_range();
}

}  // namespace msg_folder

#endif  // MSG_FOLDER__MSG__DETAIL__UWB_MSG__BUILDER_HPP_
